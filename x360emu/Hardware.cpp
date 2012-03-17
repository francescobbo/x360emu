#include "Hardware.h"
#include "Memory.h"
#include "Xenon/Xenon.h"

namespace Hardware
{
	void Init()
	{
		Memory::Init();
		Xenon::Init();
	}

	void Shutdown()
	{
		Memory::Shutdown();
	}

	void Boot()
	{
		Xenon::Boot();
	}
}
