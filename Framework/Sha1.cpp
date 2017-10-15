/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include <Sha1.h>

Sha1::Sha1()
{
    Reset();
}

/*  
 * This function will initialize the sha1 class member variables
 * in preparation for computing a new message digest.
 */
void Sha1::Reset()
{
    Length = 0;
    Message_Block_Index = 0;

    H[0] = 0x67452301;
    H[1] = 0xEFCDAB89;
    H[2] = 0x98BADCFE;
    H[3] = 0x10325476;
    H[4] = 0xC3D2E1F0;

    Computed = false;
    Corrupted = false;
}

/*  
 * This function will return the 160-bit message digest into the
 * array provided.
 */
#include <stdlib.h>
bool Sha1::Result(u32 *message_digest_array)
{
    if (Corrupted)
        return false;

    if (!Computed)
    {
        PadMessage();
        Computed = true;
    }

    for (int i = 0; i < 5; i++)
        message_digest_array[i] = _byteswap_ulong(H[i]);

    return true;
}

/*  
 * This function accepts an array of octets as the next portion of
 * the message.
 */
void Sha1::Input(const u8 *message_array, u32 length)
{
    if (!length)
        return;

    if (Computed || Corrupted)
    {
        Corrupted = true;
        return;
    }

    while (length-- && !Corrupted)
    {
        Length += 8;
        if (Length == 0)
            Corrupted = true;   // Too long
        
        Message_Block[Message_Block_Index++] = (*message_array & 0xFF);

        if (Message_Block_Index == 64)
        {
            ProcessMessageBlock();
            Message_Block_Index = 0;
        }

        message_array++;
    }
}

/*  
 * This function will process the next 512 bits of the message
 * stored in the Message_Block array.
 */
void Sha1::ProcessMessageBlock()
{
    // Constants defined for SHA-1
    const u32 K[] = {
        0x5A827999,
        0x6ED9EBA1,
        0x8F1BBCDC,
        0xCA62C1D6
    };

    int t;              // Loop counter
    u32 temp;           // Temporary word value
    u32 W[80];          // Word sequence
    u32 A, B, C, D, E;  // Word buffers

    /*
     *  Initialize the first 16 words in the array W
     */
    for(t = 0; t < 16; t++)
    {
        W[t] = ((u32) Message_Block[t * 4]) << 24;
        W[t] |= ((u32) Message_Block[t * 4 + 1]) << 16;
        W[t] |= ((u32) Message_Block[t * 4 + 2]) << 8;
        W[t] |= ((u32) Message_Block[t * 4 + 3]);
    }

    for(t = 16; t < 80; t++)
    {
       W[t] = CircularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
    }

    A = H[0];
    B = H[1];
    C = H[2];
    D = H[3];
    E = H[4];

    for (t = 0; t < 20; t++)
    {
        temp = CircularShift(5,A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = CircularShift(30,B);
        B = A;
        A = temp;
    }

    for (t = 20; t < 40; t++)
    {
        temp = CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[1];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = CircularShift(30,B);
        B = A;
        A = temp;
    }

    for (t = 40; t < 60; t++)
    {
        temp = CircularShift(5,A) +
               ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = CircularShift(30,B);
        B = A;
        A = temp;
    }

    for (t = 60; t < 80; t++)
    {
        temp = CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[3];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = CircularShift(30,B);
        B = A;
        A = temp;
    }

    H[0] = (H[0] + A) & 0xFFFFFFFF;
    H[1] = (H[1] + B) & 0xFFFFFFFF;
    H[2] = (H[2] + C) & 0xFFFFFFFF;
    H[3] = (H[3] + D) & 0xFFFFFFFF;
    H[4] = (H[4] + E) & 0xFFFFFFFF;
}

/*  
 * According to the standard, the message must be padded to an even
 * 512 bits.  The first padding bit must be a '1'.  The last 64 bits
 * represent the length of the original message.  All bits in between
 * should be 0.  This function will pad the message according to those
 * rules by filling the message_block array accordingly.  It will also
 * call ProcessMessageBlock() appropriately.  When it returns, it
 * can be assumed that the message digest has been computed.
 */
void Sha1::PadMessage()
{
    /*
     *  Check to see if the current message block is too small to hold
     *  the initial padding bits and length.  If so, we will pad the
     *  block, process it, and then continue padding into a second block.
     */
    if (Message_Block_Index > 55)
    {
        Message_Block[Message_Block_Index++] = 0x80;
        while(Message_Block_Index < 64)
            Message_Block[Message_Block_Index++] = 0;

        ProcessMessageBlock();

        while (Message_Block_Index < 56)
            Message_Block[Message_Block_Index++] = 0;
    }
    else
    {
        Message_Block[Message_Block_Index++] = 0x80;
        while(Message_Block_Index < 56)
            Message_Block[Message_Block_Index++] = 0;
    }

    /*
     *  Store the message length as the last 8 octets
     */
    Message_Block[56] = (Length >> 56) & 0xFF;
    Message_Block[57] = (Length >> 48) & 0xFF;
    Message_Block[58] = (Length >> 40) & 0xFF;
    Message_Block[59] = (Length >> 32) & 0xFF;
    Message_Block[60] = (Length >> 24) & 0xFF;
    Message_Block[61] = (Length >> 16) & 0xFF;
    Message_Block[62] = (Length >> 8) & 0xFF;
    Message_Block[63] = (Length) & 0xFF;

    ProcessMessageBlock();
}

/*  
 * This member function will perform a circular shifting operation.
 */
u32 Sha1::CircularShift(int bits, u32 word)
{
    return ((word << bits) & 0xFFFFFFFF) | ((word & 0xFFFFFFFF) >> (32 - bits));
}
