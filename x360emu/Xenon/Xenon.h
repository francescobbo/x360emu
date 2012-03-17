#pragma once

#include <Types.h>
#include "XInstruction.h"

/*
 * It's better to avoid C++ classes where performance is a must. The "this"
 * pointer takes another register slot and slows everything down. In this case
 * The xState structure is accessed a LOT of times during each instruction.
 * Adding the "this" pointer won't be sane...
 */

namespace Xenon
{
	enum CpuStates
	{
		Stopped = 0,
		Stepping = 1,
		Running = 2,
	};

	struct CpuState
	{
		u64 gpr[32];
		u64 fpr[32];

		u32 pc;
		u32 npc;

		u32 cr;				// flags
		u8 crFast[8];		// make cr handling faster dividing the register in pieces
		u64 msr;			// machine specific register
		u32 fpscr;			// floating point flags/status bits

		u32 sr[16];			// Segment registers.
		u64 spr[1024];		// Special pourpose register. The behaviour of each is totally unknown...

		volatile u32 Exceptions;

		CpuStates State;
		XInstruction CurrentInstruction;

		bool ReserveD;
		u32 ReserveAddressD;
		bool ReserveW;
		u32 ReserveAddressW;

		bool GetCrBit(int bit)
		{
			// return (crFast[bit >> 2] & (8 >> (bit & 3));
			return (crFast[bit >> 2] >> (3 - (bit & 3))) & 1;
		}

		void SetCrBit(int bit, int val)
		{
			if (val)
				crFast[bit >> 2] |= 0x8 >> (bit & 3);
			else
				crFast[bit >> 2] &= ~(0x8 >> (bit & 3));
		}

		void SetCrField(int field, u8 value)
		{
			crFast[field] = value;
		}
	};

	extern CpuState xState[6];

	void Init();
	void Boot();
}

class XenonEmulator
{
public:
	virtual void Run() = 0;
	virtual void Stop() = 0;
};
