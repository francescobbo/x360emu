/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include <BinaryReader.h>
#include <Exceptions.h>
#include <string>

BinaryReader::BinaryReader(Stream &stream) : BaseStream(stream)
{
    if (!stream.IsOpen() || !stream.CanRead())
        throw ArgumentException();
}

u8 BinaryReader::ReadByte()
{
    u8 ret;
    BaseStream.Read((u8 *) &ret, 1);
    return ret;
}

u16 BinaryReader::ReadUInt16()
{
    u16 ret;
    BaseStream.Read((u8 *) &ret, 2);
    return ret;
}

u32 BinaryReader::ReadUInt32()
{
    u32 ret;
    BaseStream.Read((u8 *) &ret, 4);
    return ret;
}

u64 BinaryReader::ReadUInt64()
{
    u64 ret;
    BaseStream.Read((u8 *) &ret, 8);
    return ret;
}

char BinaryReader::ReadChar()
{
    char ret;
    BaseStream.Read((u8 *) &ret, 1);
    return ret;
}

s16 BinaryReader::ReadInt16()
{
    s16 ret;
    BaseStream.Read((u8 *) &ret, 2);
    return ret;
}

s32 BinaryReader::ReadInt32()
{
    s32 ret;
    BaseStream.Read((u8 *) &ret, 4);
    return ret;
}

s64 BinaryReader::ReadInt64()
{
    s64 ret;
    BaseStream.Read((u8 *) &ret, 8);
    return ret;
}

void BinaryReader::ReadBytes(u8 *buffer, u64 length)
{
    BaseStream.Read(buffer, length);
}

bool BinaryReader::ReadBool()
{
    bool ret;
    BaseStream.Read((u8 *) &ret, 1);
    return ret;
}

double BinaryReader::ReadDouble()
{
    double ret;
    BaseStream.Read((u8 *) &ret, 8);
    return ret;
}

float BinaryReader::ReadFloat()
{
    float ret;
    BaseStream.Read((u8 *) &ret, 4);
    return ret;
}

void BinaryReader::ReadString(std::string &str)
{
    str = "";
    
    char c = ReadChar();
    while (c)
    {
        str += c;
        c = ReadChar();
    }
}

void BinaryReader::ReadStringN(std::string &str, u64 length)
{
    str = "";
    
    for (int i = 0; i < length; i++)
        str += ReadChar();
}
