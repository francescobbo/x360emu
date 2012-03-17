#pragma once

#include <Stream.h>
#include <string>

#ifdef _FMW_DLL_
#define DLLPREFIX __declspec(dllexport)
#else
#define DLLPREFIX __declspec(dllimport)
#endif

class BinaryReader
{
public:
	DLLPREFIX BinaryReader(Stream &stream);

	DLLPREFIX void ReadBytes(u8 *buffer, u64 length);

	DLLPREFIX u8 ReadByte();
	DLLPREFIX virtual u16 ReadUInt16();
	DLLPREFIX virtual u32 ReadUInt32();
	DLLPREFIX virtual u64 ReadUInt64();

	DLLPREFIX char ReadChar();
	DLLPREFIX virtual s16 ReadInt16();
	DLLPREFIX virtual s32 ReadInt32();
	DLLPREFIX virtual s64 ReadInt64();

	DLLPREFIX bool ReadBool();
	DLLPREFIX virtual double ReadDouble();
	DLLPREFIX virtual float ReadFloat();

	DLLPREFIX void ReadString(std::string &str);
	DLLPREFIX void ReadStringN(std::string &str, u64 length);

	//void RoundPad(int pad);

	Stream &BaseStream;

private:
	/* Avoid a warning */
	BinaryReader &operator=(const BinaryReader &copy);
};

#undef DLLPREFIX
