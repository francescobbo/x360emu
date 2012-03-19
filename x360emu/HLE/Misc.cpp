#include "HLE.h"
#include <Exceptions.h>

namespace HLE
{
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
}
