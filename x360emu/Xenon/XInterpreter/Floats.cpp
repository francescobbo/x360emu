/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "../XInterpreter.h"
#include <intrin.h>

inline double ForceSingle(double d)
{
	return static_cast<float>(d);
}

void XInterpreter::OpFcfid(Xenon::CpuState *xState)
{
	u64 val = rFP[INSTR.RB];
	if (!val)
		rFP[INSTR.RD] = 0;

	/* Hope this follows the PowerPC specification */
	double d = val;
	rFP[INSTR.RD] = *((u64 *) &d);
}

void XInterpreter::OpFdivs(Xenon::CpuState *xState)
{
	double op1 = rFPD[INSTR.RA];
	double op2 = rFPD[INSTR.RB];
	rFPD[INSTR.RD] = ForceSingle(op1 / op2);
}

void XInterpreter::OpFmuls(Xenon::CpuState *xState)
{
	double op1 = rFPD[INSTR.RA];
	double op2 = rFPD[INSTR.RC];
	rFPD[INSTR.RD] = ForceSingle(op1 * op2);
}

void XInterpreter::OpFrsp(Xenon::CpuState *xState)
{
	double single = ForceSingle(rFPD[INSTR.RB]);
	rFPD[INSTR.RD] = single;
}
