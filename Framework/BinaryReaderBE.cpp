#include <BinaryReaderBE.h>

BinaryReaderBE::BinaryReaderBE(Stream &stream) : BinaryReader(stream)
{
}

u16 BinaryReaderBE::ReadUInt16()
{
	return _byteswap_ushort(BinaryReader::ReadUInt16());
}

u32 BinaryReaderBE::ReadUInt32()
{
	return _byteswap_ulong(BinaryReader::ReadUInt32());
}

u64 BinaryReaderBE::ReadUInt64()
{
	return _byteswap_uint64(BinaryReader::ReadUInt64());
}

s16 BinaryReaderBE::ReadInt16()
{
	return (s16) _byteswap_ushort((u16) BinaryReader::ReadInt16());
}

s32 BinaryReaderBE::ReadInt32()
{
	return (s32) _byteswap_ulong((u32) BinaryReader::ReadInt32());
}

s64 BinaryReaderBE::ReadInt64()
{
	return (s64) _byteswap_uint64((u64) BinaryReader::ReadInt64());
}

double BinaryReaderBE::ReadDouble()
{
	u64 ret = _byteswap_uint64(BinaryReader::ReadUInt64());
	return *((double *) &ret);
}

float BinaryReaderBE::ReadFloat()
{
	u32 ret = _byteswap_ulong(BinaryReader::ReadUInt32());
	return *((float *) &ret);
}

