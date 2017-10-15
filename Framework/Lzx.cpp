/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "lzx.h"
#include <Exceptions.h>

/* LZX decompressor input macros
 *
 * STORE_BITS        stores bitstream state in LzxStatus structure
 * RESTORE_BITS      restores bitstream state from LzxStatus structure
 * READ_BITS(var,n)  takes N bits from the buffer and puts them in var
 * ENSURE_BITS(n)    ensures there are at least N bits in the bit buffer.
 * PEEK_BITS(n)      extracts without removing N bits from the bit buffer
 * REMOVE_BITS(n)    removes N bits from the bit buffer
 *
 * These bit access routines work by using the area beyond the MSB and the
 * LSB as a free source of zeroes when shifting. This avoids having to
 * mask any bits. So we have to know the bit width of the bit buffer
 * variable.
 *
 * The bit buffer datatype should be at least 32 bits wide: it must be
 * possible to ENSURE_BITS(16), so it must be possible to add 16 new bits
 * to the bit buffer when the bit buffer already has 1 to 15 bits left.
 */

#define BITBUF_WIDTH (sizeof(bit_buffer) * CHAR_BIT)

#define STORE_BITS do {                                                 \
    state->i_ptr      = i_ptr;                                              \
    state->i_end      = i_end;                                              \
    state->bit_buffer = bit_buffer;                                         \
    state->bits_left  = bits_left;                                          \
} while (0)

#define RESTORE_BITS do {                                               \
    i_ptr      = state->i_ptr;                                              \
    i_end      = state->i_end;                                              \
    bit_buffer = state->bit_buffer;                                         \
    bits_left  = state->bits_left;                                          \
} while (0)

#define ENSURE_BITS(nbits)                                              \
    while (bits_left < (nbits)) {                                         \
        if (i_ptr >= i_end) {                                               \
            ReadInput();                      \
            i_ptr = state->i_ptr;                                               \
            i_end = state->i_end;                                               \
        }                                                                   \
        bit_buffer |= ((i_ptr[1] << 8) | i_ptr[0])                          \
                    << (BITBUF_WIDTH - 16 - bits_left);                   \
        bits_left  += 16;                                                   \
        i_ptr      += 2;                                                    \
    }

#define PEEK_BITS(nbits) (bit_buffer >> (BITBUF_WIDTH - (nbits)))

#define REMOVE_BITS(nbits) ((bit_buffer <<= (nbits)), (bits_left -= (nbits)))

#define READ_BITS(val, nbits) do {                                      \
    ENSURE_BITS(nbits);                                                   \
    (val) = PEEK_BITS(nbits);                                             \
    REMOVE_BITS(nbits);                                                   \
} while (0)

void Lzx::ReadInput()
{
    u64 read = input->Read(&state->inbuf[0], state->inbuf_size);

    /**
     * huff decode's ENSURE_BYTES(16) might overrun the input stream, even
     * if those bits aren't used, so fake 2 more bytes
     */
    if (read == 0)
    {
        if (state->input_end)
            throw Exception("LZX: Error at reading!");
        else
        {
            read = 2;
            state->inbuf[0] = state->inbuf[1] = 0;
            state->input_end = 1;
        }
    }

    state->i_ptr = &state->inbuf[0];
    state->i_end = &state->inbuf[read];
}

/* Huffman decoding macros */

/* READ_HUFFSYM(tablename, var) decodes one huffman symbol from the
 * bitstream using the stated table and puts it in var.
 */
#define READ_HUFFSYM(tbl, var) do {                                     \
    /* huffman symbols can be up to 16 bits long */                       \
    ENSURE_BITS(16);                                                      \
    /* immediate table lookup of [tablebits] bits of the code */          \
    sym = state->tbl##_table[PEEK_BITS(LZX_##tbl##_TABLEBITS)];             \
    /* is the symbol is longer than [tablebits] bits? (i=node index) */   \
    if (sym >= LZX_##tbl##_MAXSYMBOLS) {                                  \
        /* decode remaining bits by tree traversal */                       \
        i = 1 << (BITBUF_WIDTH - LZX_##tbl##_TABLEBITS);                    \
        do {                                                                \
            /* one less bit. error if we run out of bits before decode */     \
            i >>= 1;                                                          \
            if (i == 0)                                                     \
                throw Exception("LZX: Error during decompression.");         \
            /* double node index and add 0 (left branch) or 1 (right) */      \
            sym <<= 1; sym |= (bit_buffer & i) ? 1 : 0;                       \
            /* hop to next node index / decoded symbol */                     \
            sym = state->tbl##_table[sym];                                      \
            /* while we are still in node indicies, not decoded symbols */    \
        } while (sym >= LZX_##tbl##_MAXSYMBOLS);                            \
    }                                                                     \
    /* result */                                                          \
    (var) = sym;                                                          \
    /* look up the code length of that symbol and discard those bits */   \
    i = state->tbl##_len[sym];                                              \
    REMOVE_BITS(i);                                                       \
} while (0)

/* BUILD_TABLE(tbl) builds a huffman lookup table from code lengths */
#define BUILD_TABLE(tbl)                                                \
    if (make_decode_table(LZX_##tbl##_MAXSYMBOLS, LZX_##tbl##_TABLEBITS,  \
            &state->tbl##_len[0], &state->tbl##_table[0]))      \
        throw Exception("LZX: Error during decompression.");

/* make_decode_table(nsyms, nbits, length[], table[])
 *
 * This function was coded by David Tritscher. It builds a fast huffman
 * decoding table from a canonical huffman code lengths table.
 *
 * nsyms  = total number of symbols in this huffman tree.
 * nbits  = any symbols with a code length of nbits or less can be decoded
 *          in one lookup of the table.
 * length = A table to get code lengths from [0 to syms-1]
 * table  = The table to fill up with decoded symbols and pointers.
 *
 * Returns 0 for OK or 1 for error
 */

static int make_decode_table(unsigned int nsyms, unsigned int nbits, unsigned char *length, unsigned short *table)
{
    register unsigned short sym;
    register unsigned int leaf, fill;
    register unsigned char bit_num;
    unsigned int pos         = 0; /* the current position in the decode table */
    unsigned int table_mask  = 1 << nbits;
    unsigned int bit_mask    = table_mask >> 1; /* don't do 0 length codes */
    unsigned int next_symbol = bit_mask; /* base of allocation for long codes */

    /* fill entries for codes short enough for a direct mapping */
    for (bit_num = 1; bit_num <= nbits; bit_num++)
    {
        for (sym = 0; sym < nsyms; sym++)
        {
            if (length[sym] != bit_num)
                continue;
            leaf = pos;
            if ((pos += bit_mask) > table_mask)
                return 1; /* table overrun */
            /* fill all possible lookups of this symbol with the symbol itself */
            for (fill = bit_mask; fill-- > 0;)
                table[leaf++] = sym;
        }
        bit_mask >>= 1;
    }

    /* full table already? */
    if (pos == table_mask)
        return 0;

    /* clear the remainder of the table */
    for (sym = pos; sym < table_mask; sym++)
        table[sym] = 0xFFFF;

    /* allow codes to be up to nbits+16 long, instead of nbits */
    pos <<= 16;
    table_mask <<= 16;
    bit_mask = 1 << 15;

    for (bit_num = nbits + 1; bit_num <= 16; bit_num++)
    {
        for (sym = 0; sym < nsyms; sym++)
        {
            if (length[sym] != bit_num)
                continue;

            leaf = pos >> 16;
            for (fill = 0; fill < bit_num - nbits; fill++)
            {
                /* if this path hasn't been taken yet, 'allocate' two entries */
                if (table[leaf] == 0xFFFF)
                {
                    table[(next_symbol << 1)] = 0xFFFF;
                    table[(next_symbol << 1) + 1] = 0xFFFF;
                    table[leaf] = next_symbol++;
                }

                /* follow the path and select either left or right for next bit */
                leaf = table[leaf] << 1;
                if ((pos >> (15-fill)) & 1)
                    leaf++;
            }
            
            table[leaf] = sym;

            if ((pos += bit_mask) > table_mask)
                return 1; /* table overflow */
        }
        bit_mask >>= 1;
    }

    /* full table? */
    if (pos == table_mask)
        return 0;

    /* either erroneous table, or all elements are 0 - let's find out. */
    for (sym = 0; sym < nsyms; sym++)
        if (length[sym])
            return 1;

    return 0;
}


/* READ_LENGTHS(tablename, first, last) reads in code lengths for symbols
 * first to last in the given table. The code lengths are stored in their
 * own special LZX way.
 */
#define READ_LENGTHS(tbl, first, last) do {                            \
  STORE_BITS;                                                          \
  ReadLens(&state->tbl##_len[0], (first), (unsigned int)(last));            \
  RESTORE_BITS;                                                        \
} while (0)

void Lzx::ReadLens(unsigned char *lens, unsigned int first, unsigned int last)
{
    /* bit buffer and huffman symbol decode variables */
    register unsigned int bit_buffer;
    register int bits_left, i;
    register unsigned short sym;
    unsigned char *i_ptr, *i_end;

    unsigned int x, y;
    int z;

    RESTORE_BITS;
  
    /* read lengths for pretree (20 symbols, lengths stored in fixed 4 bits) */
    for (x = 0; x < 20; x++)
    {
        READ_BITS(y, 4);
        state->PRETREE_len[x] = y;
    }

    BUILD_TABLE(PRETREE);

    for (x = first; x < last; )
    {
        READ_HUFFSYM(PRETREE, z);
        if (z == 17)
        {
            /* code = 17, run of ([read 4 bits]+4) zeros */
            READ_BITS(y, 4);
            y += 4;
            while (y--)
                lens[x++] = 0;
        }
        else if (z == 18)
        {
            /* code = 18, run of ([read 5 bits]+20) zeros */
            READ_BITS(y, 5);
            y += 20;
            while (y--)
                lens[x++] = 0;
        }
        else if (z == 19)
        {
            /* code = 19, run of ([read 1 bit]+4) [read huffman symbol] */
            READ_BITS(y, 1);
            y += 4;
            READ_HUFFSYM(PRETREE, z);
            z = lens[x] - z;
            if (z < 0)
                z += 17;
            while (y--)
                lens[x++] = z;
        }
        else
        {
            /* code = 0 to 16, delta current length entry */
            z = lens[x] - z;
            if (z < 0)
                z += 17;
            lens[x++] = z;
        }
    }

    STORE_BITS;
}

/* LZX static data tables:
 *
 * LZX uses 'position slots' to represent match offsets.  For every match,
 * a small 'position slot' number and a small offset from that slot are
 * encoded instead of one large offset.
 *
 * position_base[] is an index to the position slot bases
 *
 * extra_bits[] states how many bits of offset-from-base data is needed.
 */
static unsigned int  position_base[51];
static unsigned char extra_bits[51];

static void InitStatic()
{
    int i, j;

    for (i = 0, j = 0; i < 51; i += 2)
    {
        extra_bits[i]   = j; /* 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7... */
        extra_bits[i+1] = j;
        if ((i != 0) && (j < 17))
            j++; /* 0,0,1,2,3,4...15,16,17,17,17,17... */
    }

    for (i = 0, j = 0; i < 51; i++)
    {
        position_base[i] = j; /* 0,1,2,3,4,6,8,12,16,24,32,... */
        j += 1 << extra_bits[i]; /* 1,1,1,1,2,2,4,4,8,8,16,16,32,32,... */
    }
}

void Lzx::ResetState()
{
    int i;

    state->R0              = 1;
    state->R1              = 1;
    state->R2              = 1;
    state->header_read     = 0;
    state->block_remaining = 0;
    state->block_type      = LZX_BLOCKTYPE_INVALID;

    /* initialise tables to 0 (because deltas will be applied to them) */
    for (i = 0; i < LZX_MAINTREE_MAXSYMBOLS; i++)
        state->MAINTREE_len[i] = 0;
    for (i = 0; i < LZX_LENGTH_MAXSYMBOLS; i++)
        state->LENGTH_len[i]   = 0;
}

Lzx::Lzx(Stream *input, Stream *output, int windowBits, u64 inputSize, u64 decompressedLength) : input(input), output(output)
{
    state = new State;

    state->window = 0;
    state->inbuf = 0;

    u32 windowSize = 1 << windowBits;

    /* LZX supports window sizes of 2^15 (32Kb) through 2^21 (2Mb) */
    if (windowBits < 15 || windowBits > 21)
        throw ArgumentOutOfRangeException("LZX: Window Size must be between 32Kb and 2Mb.");

    inputSize = (inputSize + 1) & -2;
    if (!inputSize)
        throw ArgumentException("LZX: invalid input buffer size.");

    InitStatic();

    /* allocate decompression window and input buffer */
    state->window = new u8[windowSize];
    if (!state->window)
        throw OutOfMemoryException();
    state->inbuf  = new u8[inputSize];
    if (!state->inbuf)
        throw OutOfMemoryException();

    /* initialise decompression state */
    state->length = decompressedLength;
    state->offset          = 0;
    state->inbuf_size      = inputSize;
    state->window_size     = windowSize;
    state->window_posn     = 0;
    state->frame_posn      = 0;
    state->frame           = 0;
    state->intel_filesize  = 0;
    state->intel_curpos    = 0;

    /* window bits:    15  16  17  18  19  20  21
    * position slots: 30  32  34  36  38  42  50  */
    state->posn_slots      = ((windowBits == 21) ? 50 : ((windowBits == 20) ? 42 : (windowBits << 1)));
    state->intel_started   = 0;
    state->input_end       = 0;

    state->i_ptr = state->i_end = &state->inbuf[0];
    state->o_ptr = state->o_end = &state->e8_buf[0];
    state->bit_buffer = state->bits_left = 0;

    ResetState();
}

Lzx::~Lzx()
{
    delete[] state->inbuf;
    delete[] state->window;
}

void Lzx::Decompress(u64 out_bytes)
{
    /* bitstream reading and huffman variables */
    register unsigned int bit_buffer;
    register int bits_left, i=0;
    register unsigned short sym;
    unsigned char *i_ptr, *i_end;

    int match_length, length_footer, extra, verbatim_bits, bytes_todo;
    int this_run, main_element, aligned_bits, j;
    unsigned char *window, *runsrc, *rundest, buf[12];
    unsigned int frame_size=0, end_frame, match_offset, window_posn;
    unsigned int R0, R1, R2;

    /* flush out any stored-up bytes before we begin */
    i = state->o_end - state->o_ptr;
    if (i > out_bytes)
        i = (int) out_bytes;

    if (i)
    {
        output->Write(state->o_ptr, i);

        state->o_ptr  += i;
        state->offset += i;
        out_bytes   -= i;
    }

    if (out_bytes == 0)
        return;

    /* restore local state */
    RESTORE_BITS;
    window = state->window;
    window_posn = state->window_posn;
    R0 = state->R0;
    R1 = state->R1;
    R2 = state->R2;

    end_frame = (unsigned int)((state->offset + out_bytes) / LZX_FRAME_SIZE) + 1;

    while (state->frame < end_frame)
    {
        /* read header if necessary */
        if (!state->header_read)
        {
            /* read 1 bit. if bit=0, intel filesize = 0.
            * if bit=1, read intel filesize (32 bits) */
            j = 0;
            READ_BITS(i, 1);
            if (i)
            {
                READ_BITS(i, 16);
                READ_BITS(j, 16);
            }
            state->intel_filesize = (i << 16) | j;
            state->header_read = 1;
        } 

    /* calculate size of frame: all frames are 32k except the final frame
     * which is 32kb or less. this can only be calculated when state->length
     * has been filled in. */
        frame_size = LZX_FRAME_SIZE;
        if (state->length && (state->length - state->offset) < (off_t) frame_size)
            frame_size = state->length - state->offset;

        /* decode until one more frame is available */
        bytes_todo = state->frame_posn + frame_size - window_posn;
        while (bytes_todo > 0)
        {
            /* initialise new block, if one is needed */
            if (state->block_remaining == 0)
            {
                /* realign if previous block was an odd-sized UNCOMPRESSED block */
                if ((state->block_type == LZX_BLOCKTYPE_UNCOMPRESSED) && (state->block_length & 1))
                {
                    if (i_ptr == i_end)
                    {
                        ReadInput();
                        i_ptr = state->i_ptr;
                        i_end = state->i_end;
                    }
                    i_ptr++;
                }
                
                /* read block type (3 bits) and block length (24 bits) */
                READ_BITS(state->block_type, 3);
                READ_BITS(i, 16);
                READ_BITS(j, 8);
                state->block_remaining = state->block_length = (i << 8) | j;
                
                /* read individual block headers */
                switch (state->block_type)
                {
                case LZX_BLOCKTYPE_ALIGNED:
                    /* read lengths of and build aligned huffman decoding tree */
                    for (i = 0; i < 8; i++)
                    {
                        READ_BITS(j, 3);
                        state->ALIGNED_len[i] = j;
                    }
                    BUILD_TABLE(ALIGNED);
                    /* no break -- rest of aligned header is same as verbatim */
                case LZX_BLOCKTYPE_VERBATIM:
                    /* read lengths of and build main huffman decoding tree */
                    READ_LENGTHS(MAINTREE, 0, 256);
                    READ_LENGTHS(MAINTREE, 256, LZX_NUM_CHARS + (state->posn_slots << 3));
                    BUILD_TABLE(MAINTREE);
                    /* if the literal 0xE8 is anywhere in the block... */
                    if (state->MAINTREE_len[0xE8] != 0)
                        state->intel_started = 1;
                    /* read lengths of and build lengths huffman decoding tree */
                    READ_LENGTHS(LENGTH, 0, LZX_NUM_SECONDARY_LENGTHS);
                    BUILD_TABLE(LENGTH);
                    break;
                case LZX_BLOCKTYPE_UNCOMPRESSED:
                    /* because we can't assume otherwise */
                    state->intel_started = 1;

                    /* read 1-16 (not 0-15) bits to align to bytes */
                    ENSURE_BITS(16);
                    if (bits_left > 16)
                        i_ptr -= 2;
                    bits_left = 0;
                    bit_buffer = 0;

                    /* read 12 bytes of stored R0 / R1 / R2 values */
                    for (rundest = &buf[0], i = 0; i < 12; i++)
                    {
                        if (i_ptr == i_end)
                        {
                            ReadInput();
                            i_ptr = state->i_ptr;
                            i_end = state->i_end;
                        }
                        *rundest++ = *i_ptr++;
                    }
                    
                    R0 = buf[0] | (buf[1] << 8) | (buf[2]  << 16) | (buf[3]  << 24);
                    R1 = buf[4] | (buf[5] << 8) | (buf[6]  << 16) | (buf[7]  << 24);
                    R2 = buf[8] | (buf[9] << 8) | (buf[10] << 16) | (buf[11] << 24);
                    break;
                default:
                    throw Exception("LZX: Error during decompression.");
                }
            }

            /* decode more of the block:
            * run = min(what's available, what's needed) */
            this_run = state->block_remaining;
            if (this_run > bytes_todo)
                this_run = bytes_todo;

            /* assume we decode exactly this_run bytes, for now */
            bytes_todo           -= this_run;
            state->block_remaining -= this_run;

            /* decode at least this_run bytes */
            switch (state->block_type)
            {
            case LZX_BLOCKTYPE_VERBATIM:
                while (this_run > 0)
                {
                    READ_HUFFSYM(MAINTREE, main_element);
                    if (main_element < LZX_NUM_CHARS)
                    {
                        /* literal: 0 to LZX_NUM_CHARS-1 */
                        window[window_posn++] = main_element;
                        this_run--;
                    }
                    else
                    {
                        /* match: LZX_NUM_CHARS + ((slot<<3) | length_header (3 bits)) */
                        main_element -= LZX_NUM_CHARS;

                        /* get match length */
                        match_length = main_element & LZX_NUM_PRIMARY_LENGTHS;
                        if (match_length == LZX_NUM_PRIMARY_LENGTHS)
                        {
                            READ_HUFFSYM(LENGTH, length_footer);
                            match_length += length_footer;
                        }
                        match_length += LZX_MIN_MATCH;
      
                        /* get match offset */
                        switch ((match_offset = (main_element >> 3)))
                        {
                        case 0:
                            match_offset = R0;
                            break;
                        case 1:
                            match_offset = R1;
                            R1=R0;
                            R0 = match_offset;
                            break;
                        case 2:
                            match_offset = R2;
                            R2=R0;
                            R0 = match_offset;
                            break;
                        case 3:
                            match_offset = 1;
                            R2=R1;
                            R1=R0;
                            R0 = match_offset;
                            break;
                        default:
                            extra = extra_bits[match_offset];
                            READ_BITS(verbatim_bits, extra);
                            match_offset = position_base[match_offset] - 2 + verbatim_bits;
                            R2 = R1; R1 = R0; R0 = match_offset;
                        }
                        
                        if ((window_posn + match_length) > state->window_size)
                            throw Exception("LZX: Error during decompression.");
        
                        /* copy match */
                        rundest = &window[window_posn];
                        i = match_length;
                        /* does match offset wrap the window? */
                        if (match_offset > window_posn)
                        {
                            /* j = length from match offset to end of window */
                            j = match_offset - window_posn;
                            if (j > (int) state->window_size)
                                throw Exception("LZX: Error during decompression.");

                            runsrc = &window[state->window_size - j];
                            if (j < i)
                            {
                                /* if match goes over the window edge, do two copy runs */
                                i -= j;
                                while (j-- > 0)
                                    *rundest++ = *runsrc++;
                                runsrc = window;
                            }
                            while (i-- > 0)
                                *rundest++ = *runsrc++;
                        }
                        else
                        {
                            runsrc = rundest - match_offset;
                            while (i-- > 0)
                                *rundest++ = *runsrc++;
                        }
                        
                        this_run    -= match_length;
                        window_posn += match_length;
                    }
                } /* while (this_run > 0) */
                break;
            
            case LZX_BLOCKTYPE_ALIGNED:
                while (this_run > 0)
                {
                    READ_HUFFSYM(MAINTREE, main_element);
                    if (main_element < LZX_NUM_CHARS)
                    {
                        /* literal: 0 to LZX_NUM_CHARS-1 */
                        window[window_posn++] = main_element;
                        this_run--;
                    }
                    else
                    {
                        /* match: LZX_NUM_CHARS + ((slot<<3) | length_header (3 bits)) */
                        main_element -= LZX_NUM_CHARS;
                        
                        /* get match length */
                        match_length = main_element & LZX_NUM_PRIMARY_LENGTHS;
                        if (match_length == LZX_NUM_PRIMARY_LENGTHS)
                        {
                            READ_HUFFSYM(LENGTH, length_footer);
                            match_length += length_footer;
                        }
                        match_length += LZX_MIN_MATCH;
                        
                        /* get match offset */
                        switch ((match_offset = (main_element >> 3)))
                        {
                        case 0:
                            match_offset = R0;
                            break;
                        case 1:
                            match_offset = R1;
                            R1 = R0;
                            R0 = match_offset;
                            break;
                        case 2:
                            match_offset = R2;
                            R2 = R0;
                            R0 = match_offset;
                            break;
                        default:
                            extra = extra_bits[match_offset];
                            match_offset = position_base[match_offset] - 2;
                            if (extra > 3)
                            {
                                /* verbatim and aligned bits */
                                extra -= 3;
                                READ_BITS(verbatim_bits, extra);
                                match_offset += (verbatim_bits << 3);
                                READ_HUFFSYM(ALIGNED, aligned_bits);
                                match_offset += aligned_bits;
                            }
                            else if (extra == 3)
                            {
                                /* aligned bits only */
                                READ_HUFFSYM(ALIGNED, aligned_bits);
                                match_offset += aligned_bits;
                            }
                            else if (extra > 0)
                            {
                                /* extra==1, extra==2 */
                                /* verbatim bits only */
                                READ_BITS(verbatim_bits, extra);
                                match_offset += verbatim_bits;
                            }
                            else /* extra == 0 */
                            {
                                /* ??? not defined in LZX specification! */
                                match_offset = 1;
                            }
                            
                            /* update repeated offset LRU queue */
                            R2 = R1; R1 = R0; R0 = match_offset;
                        }
                        
                        if ((window_posn + match_length) > state->window_size)
                            throw Exception("LZX: Error during decompression.");
                        
                        /* copy match */
                        rundest = &window[window_posn];
                        i = match_length;
                        
                        /* does match offset wrap the window? */
                        if (match_offset > window_posn)
                        {
                            /* j = length from match offset to end of window */
                            j = match_offset - window_posn;
                            if (j > (int) state->window_size)
                                throw Exception("LZX: Error during decompression.");
                            
                            runsrc = &window[state->window_size - j];
                            if (j < i)
                            {
                                /* if match goes over the window edge, do two copy runs */
                                i -= j;
                                while (j-- > 0)
                                    *rundest++ = *runsrc++;
                                runsrc = window;
                            }
                            
                            while (i-- > 0)
                                *rundest++ = *runsrc++;
                        }
                        else
                        {
                            runsrc = rundest - match_offset;
                            while (i-- > 0)
                                *rundest++ = *runsrc++;
                        }
                        
                        this_run    -= match_length;
                        window_posn += match_length;
                    }
                } /* while (this_run > 0) */
                break;

            case LZX_BLOCKTYPE_UNCOMPRESSED:
                /* as this_run is limited not to wrap a frame, this also means it
                * won't wrap the window (as the window is a multiple of 32k) */
                rundest = &window[window_posn];
                window_posn += this_run;
                while (this_run > 0)
                {
                    i = i_end - i_ptr;
                    if (i != 0)
                    {
                        if (i > this_run)
                            i = this_run;
                        memcpy(rundest, i_ptr, (size_t) i);
                        rundest  += i;
                        i_ptr    += i;
                        this_run -= i;
                    }
                    else
                    {
                        ReadInput();
                        i_ptr = state->i_ptr;
                        i_end = state->i_end;
                    }
                }
                break;
            
            default:
                throw Exception("LZX: Error during decompression.");
            }
            
            /* did the final match overrun our desired this_run length? */
            if (this_run < 0)
            {
                if ((unsigned int)(-this_run) > state->block_remaining)
                    throw Exception("LZX: Error during decompression.");
                state->block_remaining -= -this_run;
            }
        } /* while (bytes_todo > 0) */

        /* streams don't extend over frame boundaries */
        if ((window_posn - state->frame_posn) != frame_size)
            throw Exception("LZX: Error during decompression.");
    
        /* re-align input bitstream */
        if (bits_left > 0)
            ENSURE_BITS(16);
        if (bits_left & 15)
            REMOVE_BITS(bits_left & 15);

        /* check that we've used all of the previous frame first */
        if (state->o_ptr != state->o_end)
            throw Exception("LZX: Error during decompression.");

        /* does this intel block _really_ need decoding? */
        if (state->intel_started && state->intel_filesize && (state->frame <= 32768) && (frame_size > 10))
        {
            unsigned char *data    = &state->e8_buf[0];
            unsigned char *dataend = &state->e8_buf[frame_size - 10];
            signed int curpos      = state->intel_curpos;
            signed int filesize    = state->intel_filesize;
            signed int abs_off, rel_off;

            /* copy e8 block to the e8 buffer and tweak if needed */
            state->o_ptr = data;
            memcpy(data, &state->window[state->frame_posn], frame_size);

            while (data < dataend)
            {
                if (*data++ != 0xE8)
                {
                    curpos++;
                    continue;
                }
            
                abs_off = data[0] | (data[1]<<8) | (data[2]<<16) | (data[3]<<24);
                if ((abs_off >= -curpos) && (abs_off < filesize))
                {
                    rel_off = (abs_off >= 0) ? abs_off - curpos : abs_off + filesize;
                    data[0] = (unsigned char) rel_off;
                    data[1] = (unsigned char) (rel_off >> 8);
                    data[2] = (unsigned char) (rel_off >> 16);
                    data[3] = (unsigned char) (rel_off >> 24);
                }
                data += 4;
                curpos += 5;
            }
            state->intel_curpos += frame_size;
        }
        else
        {
            state->o_ptr = &state->window[state->frame_posn];
            if (state->intel_filesize)
                state->intel_curpos += frame_size;
        }

        state->o_end = &state->o_ptr[frame_size];

        /* write a frame */
        i = (out_bytes < (off_t) frame_size) ? (unsigned int) out_bytes : frame_size;
        output->Write(state->o_ptr, i);
        state->o_ptr  += i;
        state->offset += i;
        out_bytes   -= i;

        /* advance frame start position */
        state->frame_posn += frame_size;
        state->frame++;

        /* wrap window / frame position pointers */
        if (window_posn == state->window_size)
            window_posn = 0;
        if (state->frame_posn == state->window_size)
            state->frame_posn = 0;
    } /* while (state->frame < end_frame) */

    if (out_bytes)
        throw Exception("LZX: Error during decompression.");

    /* store local state */
    STORE_BITS;
    state->window_posn = window_posn;
    state->R0 = R0;
    state->R1 = R1;
    state->R2 = R2;
}
