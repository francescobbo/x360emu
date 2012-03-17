#pragma once

#include <Types.h>

#ifdef _FMW_DLL_
#define DLLPREFIX __declspec(dllexport)
#else
#define DLLPREFIX __declspec(dllimport)
#endif

class Sha1
{
public:
	DLLPREFIX Sha1();

	DLLPREFIX void Reset();
	DLLPREFIX void Input(const u8 *message_array, u32 length);
	DLLPREFIX bool Result(u32 *message_digest_array);

private:
	void ProcessMessageBlock();
	void PadMessage();
	inline u32 CircularShift(int bits, u32 word);

	u32 H[5];					// Message digest buffers
	u64 Length;
	u8 Message_Block[64];		// 512-bit message blocks
	int Message_Block_Index;	// Index into message block array
	
	bool Computed;
	bool Corrupted;
};

#undef DLLPREFIX
