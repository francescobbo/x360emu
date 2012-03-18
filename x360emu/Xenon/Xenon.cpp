/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "Xenon.h"
#include "XInterpreter.h"
#include <string.h>

namespace Xenon
{
	CpuState xState[6];

	/* Different threads may have different emulation methods. (Eg: T0 uses interpreter while T1 uses a JIT, etc...) */
	XenonEmulator *xcpu[6];

	void Init()
	{
		memset(xState, 0, sizeof(CpuState) * 6);

		for (int i = 0; i < 6; i++)
		{
			/*
			 * Since the Xbox 360 includes an operating systems and games are 
			 * just applications, they expect a working stack. So now create a
			 * 4 MB stack for each thread from the end of physical memory.
			 * The r1 register is used as a stack pointer in the PowerPC arch.
			 * It seems that the stack must be 16-bytes aligned at entry point.
			 */
			xState[i].gpr[1] = 0x9FFFFFF0 - i * 1024 * 1024 * 4;

			xcpu[i] = new XInterpreter(xState[i]);
		}
	}

	void Boot()
	{
		xcpu[0]->Run();
	}
}
