/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#pragma once

#include <Types.h>

struct XBOX_HARDWARE_INFO
{
	/*
	 * Found 0x2000817 in some kernels. Here are the bit fields:
	 *		bit 27: debug mode on
	 *		remaining: unknown!
	 */
	u32 Flags;

	/* Fixed to 6 */
	u8 CpuCount;

	/* ?? */
	u8 PCIBridgeRevisionID;
	u8 Reserved[6];

	/*
	 * This is set by the kernel to r3 and r4 values at booting. Maybe the
	 * hypervisor sets r3 and r4 before calling the kernel.
	 */
	u16 BootLoaderMagic;
	u16 BootLoaderFlags;
};

extern XBOX_HARDWARE_INFO XboxHardwareInfo;

struct KERNEL_VERSION
{
	u16 Major;
	u16 Minor;
	u16 Build;

	/* I found 0x80 somewhere... what does this mean? */
	u8 ApprovalType;

	/* WTH is QFE?? Probably Quick Fix Engineering, a term used by MSFT say Hotfix, Patch, Small Update */
	u8 QFE;
};

extern KERNEL_VERSION XboxKrnlVersion;
extern KERNEL_VERSION XboxKrnlBaseVersion;

extern uint32_t VdGpuClockInMHz;

#include <Windows.h>
#include "../Xenon/Xenon.h"
#include <XexParser.h>

void LinkLibraries(XexParser &xp);

namespace HLE
{
	struct RTL_CRITICAL_SECTION
	{
		LONG Unknown[4];
		LONG LockCount;
		LONG RecursionCount;
		LONG OwningThread;
	};

	void Init();

	void Exec(Xenon::CpuState *xState, uint32_t funcId);

	void NtAllocateVirtualMemory(Xenon::CpuState *xState);
	void NtFreeVirtualMemory(Xenon::CpuState *xState);

	void RtlInitializeCriticalSection(Xenon::CpuState *xState);
	void RtlInitializeCriticalSectionAndSpinCount(Xenon::CpuState *xState);
	void RtlEnterCriticalSection(Xenon::CpuState *xState);
	void RtlLeaveCriticalSection(Xenon::CpuState *xState);
}
