/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "HLE.h"
#include "../Memory.h"

#include <ntstatus.h>

/**
 * The funny thing about this file is that to keep consistency we need to use
 * some atomic functions that are generally available on Win32, but they're
 * made to work with Little Endian numbers. So we have to create some
 * "InterlockedIncrement", "InterlockedDecrement", "...", to work with Big
 * Endians, and since they have to be atomic, we have to use the host's
 * RtlEnterCriticalSection and RtlLeaveCriticalSection...
 */

inline LONG BeDecrement(LONG &val)
{
	LONG tmp = _byteswap_ulong(val) - 1;
	val = _byteswap_ulong(tmp);
	return val;
}

inline LONG BeIncrement(LONG &val)
{
	LONG tmp = _byteswap_ulong(val) + 1;
	val = _byteswap_ulong(tmp);
	return val;
}

namespace HLE
{
	::CRITICAL_SECTION cs;

	void RtlInitializeCriticalSection(Xenon::CpuState *xState)
	{
		xState->gpr[4] = 0;
		RtlInitializeCriticalSectionAndSpinCount(xState);
	}

	void RtlInitializeCriticalSectionAndSpinCount(Xenon::CpuState *xState)
	{
		RTL_CRITICAL_SECTION *crit = (RTL_CRITICAL_SECTION *) Memory::GetPointer((u32) xState->gpr[3]);

		crit->Unknown[0] = _byteswap_ulong(1);
		crit->Unknown[1] = 0;
		crit->Unknown[2] = crit->Unknown[3] = (LONG) xState->gpr[3] + 8;

		crit->RecursionCount = crit->OwningThread = 0;
		crit->LockCount = 0xFFFFFFFF;

		/* I don't know why do the Xbox Kernel do this with the SpinCount */
		((u8 *) crit)[1] = ((xState->gpr[4] + 0xFF) >> 8) & 1;
	}

	void RtlEnterCriticalSection(Xenon::CpuState *xState)
	{
		RTL_CRITICAL_SECTION *crit = (RTL_CRITICAL_SECTION *) Memory::GetPointer((u32) xState->gpr[3]);

		::EnterCriticalSection(&cs);

		// Reverse engineered from xboxkrnl.exe
		LONG Thread = Memory::Read32((u32) xState->gpr[13] + 0x100);

		if (BeIncrement(crit->LockCount))
		{
			if (Thread == _byteswap_ulong(crit->OwningThread))
				BeIncrement(crit->RecursionCount);
//			else
//				RtlpWaitForCriticalSection(crit);
		}
		else
		{
			crit->OwningThread = _byteswap_ulong(Thread);
			crit->RecursionCount = _byteswap_ulong(1);
		}

		::LeaveCriticalSection(&cs);
		xState->gpr[3] = STATUS_SUCCESS;
	}

	void RtlLeaveCriticalSection(Xenon::CpuState *xState)
	{
		RTL_CRITICAL_SECTION *crit = (RTL_CRITICAL_SECTION *) Memory::GetPointer((u32) xState->gpr[3]);

		::EnterCriticalSection(&cs);

		if (BeDecrement(crit->RecursionCount))
			BeDecrement(crit->LockCount);
		else
		{
			/* 0 is 0x00000000 both in Big Endian and in Little Endian */
			crit->OwningThread = 0;

			BeDecrement(crit->LockCount);
			/* -1 is 0xFFFFFFFF both in Big Endian and in Little Endian */
//			if (crit->LockCount == -1)
//				RtlpUnWaitCriticalSection(crit);
		}

		::LeaveCriticalSection(&cs);

		xState->gpr[3] = STATUS_SUCCESS;
	}
}
