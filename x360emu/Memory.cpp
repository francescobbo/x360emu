#include "Memory.h"
#include <Windows.h>

/*
 * As far as I know, the Xbox 360 has a 64-bit CPU, but all addresses are 
 * still truncated to 32 bit. So it should be save to use "u32 Address",
 * instead of "u64 Address". (Maybe even faster??)
 */

namespace Memory
{
	u8 *mem;

	void Init()
	{
		mem = (u8 *) VirtualAlloc(NULL, 512 * 1024 * 1024, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!mem)
			throw "VirtualAlloc failed: Out of memory.";
	}

	void Shutdown()
	{
		if (mem)
		{
			BOOL ret = VirtualFree(mem, 0, MEM_RELEASE);
			if (!ret)
				throw "VirtualFree failed!";
		}
	}

	u8 Read8(u32 Address)
	{
		u8 *addr = GetPointer(Address);
		return *addr;
	}

	u16 Read16(u32 Address)
	{
		u16 *addr = (u16 *) GetPointer(Address);
		return _byteswap_ushort(*addr);
	}

	u32 Read32(u32 Address)
	{
		u32 *addr = (u32 *) GetPointer(Address);
		return _byteswap_ulong(*addr);
	}

	u64 Read64(u32 Address)
	{
		u64 *addr = (u64 *) GetPointer(Address);
		return _byteswap_uint64(*addr);
	}

	void Write8(u32 Address, u8 Value)
	{
		u8 *addr = GetPointer(Address);
		*addr = Value;
	}

	void Write16(u32 Address, u16 Value)
	{
		u16 *addr = (u16 *) GetPointer(Address);
		*addr = _byteswap_ushort(Value);
	}

	void Write32(u32 Address, u32 Value)
	{
		u32 *addr = (u32 *) GetPointer(Address);
		*addr = _byteswap_ulong(Value);
	}

	void Write64(u32 Address, u64 Value)
	{
		u64 *addr = (u64 *) GetPointer(Address);
		*addr = _byteswap_uint64(Value);
	}

	u8 *GetPointer(u32 Address)
	{
		if (!mem)
			throw "Memory hasn't been allocated yet!";

		/* Partially taken from Dolphin... Hope this works... */
		u8 region = (Address >> 28);
		if (region == 0 || (region >= 0x8 && region < 0xD))
			return &mem[Address & 0x1FFFFFFF];
		else
			throw "I don't know how to handle this address!";
	}
}
