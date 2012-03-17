#pragma once

#include <Stream.h>

#ifdef _FMW_DLL_
#define DLLPREFIX __declspec(dllexport)
#else
#define DLLPREFIX __declspec(dllimport)
#endif

/* some constants defined by the LZX specification */
#define LZX_MIN_MATCH				2
#define LZX_MAX_MATCH				257
#define LZX_NUM_CHARS				256
#define LZX_BLOCKTYPE_INVALID		0	/* also blocktypes 4-7 invalid */
#define LZX_BLOCKTYPE_VERBATIM		1
#define LZX_BLOCKTYPE_ALIGNED		2
#define LZX_BLOCKTYPE_UNCOMPRESSED	3
#define LZX_PRETREE_NUM_ELEMENTS	20
#define LZX_ALIGNED_NUM_ELEMENTS	8	/* aligned offset tree #elements */
#define LZX_NUM_PRIMARY_LENGTHS		7	/* this one missing from spec! */
#define LZX_NUM_SECONDARY_LENGTHS	249	/* length tree #elements */

/* LZX huffman defines: tweak tablebits as desired */
#define LZX_PRETREE_MAXSYMBOLS	LZX_PRETREE_NUM_ELEMENTS
#define LZX_PRETREE_TABLEBITS	6
#define LZX_MAINTREE_MAXSYMBOLS	(LZX_NUM_CHARS + 50 * 8)
#define LZX_MAINTREE_TABLEBITS	12
#define LZX_LENGTH_MAXSYMBOLS	(LZX_NUM_SECONDARY_LENGTHS + 1)
#define LZX_LENGTH_TABLEBITS	12
#define LZX_ALIGNED_MAXSYMBOLS	LZX_ALIGNED_NUM_ELEMENTS
#define LZX_ALIGNED_TABLEBITS	7
#define LZX_LENTABLE_SAFETY		64		/* table decoding overruns are allowed */

#define LZX_FRAME_SIZE			32768	/* the size of a frame in LZX */

class Lzx
{
public:
	DLLPREFIX Lzx(Stream *input, Stream *output, int windowBits, u64 inputLen, u64 decompressedLen);
	DLLPREFIX ~Lzx();

	DLLPREFIX void Decompress(u64 out_bytes);

private:
	void ResetState();
	void ReadLens(unsigned char *lens, unsigned int first, unsigned int last);
	void ReadInput();

	Stream *input;
	Stream *output;

	struct State
	{
		u64   offset;					/* number of bytes actually output         */
		u64   length;					/* overall decompressed length of stream   */

		unsigned char *window;			/* decoding window                         */
		unsigned int   window_size;		/* window size                             */
		unsigned int   window_posn;     /* decompression offset within window      */
		unsigned int   frame_posn;      /* current frame offset within in window   */
		unsigned int   frame;           /* the number of 32kb frames processed     */

		unsigned int   R0, R1, R2;      /* for the LRU offset system               */
		unsigned int   block_length;    /* uncompressed length of this LZX block   */
		unsigned int   block_remaining; /* uncompressed bytes still left to decode */

		signed int     intel_filesize;  /* magic header value used for transform   */
		signed int     intel_curpos;    /* current offset in transform space       */

		unsigned char  intel_started;   /* has intel E8 decoding started?          */
		unsigned char  block_type;      /* type of the current block               */
		unsigned char  header_read;     /* have we started decoding at all yet?    */
		unsigned char  posn_slots;      /* how many posn slots in stream?          */
		unsigned char  input_end;       /* have we reached the end of input?       */

		/* I/O buffering */
		unsigned char *inbuf, *i_ptr, *i_end, *o_ptr, *o_end;
		unsigned int  bit_buffer, bits_left, inbuf_size;

		/* huffman code lengths */
		unsigned char PRETREE_len  [LZX_PRETREE_MAXSYMBOLS  + LZX_LENTABLE_SAFETY];
		unsigned char MAINTREE_len [LZX_MAINTREE_MAXSYMBOLS + LZX_LENTABLE_SAFETY];
		unsigned char LENGTH_len   [LZX_LENGTH_MAXSYMBOLS   + LZX_LENTABLE_SAFETY];
		unsigned char ALIGNED_len  [LZX_ALIGNED_MAXSYMBOLS  + LZX_LENTABLE_SAFETY];

		/* huffman decoding tables */
		unsigned short PRETREE_table [(1 << LZX_PRETREE_TABLEBITS)  + (LZX_PRETREE_MAXSYMBOLS * 2)];
		unsigned short MAINTREE_table[(1 << LZX_MAINTREE_TABLEBITS) + (LZX_MAINTREE_MAXSYMBOLS * 2)];
		unsigned short LENGTH_table  [(1 << LZX_LENGTH_TABLEBITS)   + (LZX_LENGTH_MAXSYMBOLS * 2)];
		unsigned short ALIGNED_table [(1 << LZX_ALIGNED_TABLEBITS)  + (LZX_ALIGNED_MAXSYMBOLS * 2)];

		/* this is used purely for doing the intel E8 transform */
		unsigned char e8_buf[LZX_FRAME_SIZE];
	};

	/*
	 * We keep a pointer to the state, instead of a reference, since the
	 * structure is HUGE, and we don't want to risk a stack overflow.
	 */
	State *state;
};

#undef DLLPREFIX
