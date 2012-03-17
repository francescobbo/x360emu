#pragma once

#include <Types.h>

namespace Memory
{
	void Init();
	void Shutdown();

	u8 Read8(u32 Address);
	u16 Read16(u32 Address);
	u32 Read32(u32 Address);
	u64 Read64(u32 Address);
	
	void Write8(u32 Address, u8 Value);
	void Write16(u32 Address, u16 Value);
	void Write32(u32 Address, u32 Value);
	void Write64(u32 Address, u64 Value);

	u8 *GetPointer(u32 Address);
}
