#include "HLE.h"
#include "../Memory.h"
#include <Exceptions.h>

namespace HLE
{
	/* LONGLONG KeQueryPerformanceTimer(); */
	void KeQueryPerformanceFrequency(Xenon::CpuState *xState)
	{
		xState->gpr[3] = 50000000;
	}

	void KeBugCheck(Xenon::CpuState *xState)
	{
		xState->gpr[4] = xState->gpr[5] = xState->gpr[6] = xState->gpr[7] = 0;
		KeBugCheckEx(xState);
	}

	void KeBugCheckEx(Xenon::CpuState *xState)
	{
		throw Exception("HLE: KeBugCheckEx called! This implies a serious flaw in the emulator.");
	}

	/* BOOL XexCheckExecutablePrivilege(DWORD Priv); */
	void XexCheckExecutablePrivilege(Xenon::CpuState *xState)
	{
		/* Unitl some research is done, report everything as unprivileged */
		xState->gpr[3] = 0;
	}
}
