/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "../XInterpreter.h"
#include <iostream>

using namespace std;

void XInterpreter::OpCrand(Xenon::CpuState *xState)
{
	xState->SetCrBit(INSTR.CRBD, xState->GetCrBit(INSTR.CRBA) & xState->GetCrBit(INSTR.CRBB));
}


void XInterpreter::OpCrandc(Xenon::CpuState *xState)
{
	xState->SetCrBit(INSTR.CRBD, xState->GetCrBit(INSTR.CRBA) & (1 ^ xState->GetCrBit(INSTR.CRBB)));
}

void XInterpreter::OpCreqv(Xenon::CpuState *xState)
{
	xState->SetCrBit(INSTR.CRBD, 1 ^ (xState->GetCrBit(INSTR.CRBA) ^ xState->GetCrBit(INSTR.CRBB)));
}

void XInterpreter::OpCrnand(Xenon::CpuState *xState)
{
	xState->SetCrBit(INSTR.CRBD, 1 ^ (xState->GetCrBit(INSTR.CRBA) & xState->GetCrBit(INSTR.CRBB)));
}

void XInterpreter::OpCrnor(Xenon::CpuState *xState)
{
	xState->SetCrBit(INSTR.CRBD, 1 ^ (xState->GetCrBit(INSTR.CRBA) | xState->GetCrBit(INSTR.CRBB)));
}

void XInterpreter::OpCror(Xenon::CpuState *xState)
{
	xState->SetCrBit(INSTR.CRBD, (xState->GetCrBit(INSTR.CRBA) | xState->GetCrBit(INSTR.CRBB)));
}

void XInterpreter::OpCrorc(Xenon::CpuState *xState)
{
	xState->SetCrBit(INSTR.CRBD, (xState->GetCrBit(INSTR.CRBA) | (1 ^ xState->GetCrBit(INSTR.CRBB))));
}

void XInterpreter::OpCrxor(Xenon::CpuState *xState)
{
	xState->SetCrBit(INSTR.CRBD, (xState->GetCrBit(INSTR.CRBA) ^ xState->GetCrBit(INSTR.CRBB)));
}

void XInterpreter::OpMfspr(Xenon::CpuState *xState)
{
	u32 iIndex = ((INSTR.SPR & 0x1F) << 5) + ((INSTR.SPR >> 5) & 0x1F);
	rGPR[INSTR.RD] = rSPR[iIndex];
}

void XInterpreter::OpMtspr(Xenon::CpuState *xState)
{
	u32 iIndex = (INSTR.SPRU << 5) | (INSTR.SPRL & 0x1F);
	rSPR[iIndex] = rGPR[INSTR.RD];
}

void XInterpreter::OpTwi(Xenon::CpuState *xState)
{
	s64 a = rGPR[INSTR.RA];
	s64 b = INSTR.SIMM_16;
	s64 TO = INSTR.TO;

	if ( ((a < b) && (TO & 0x10))
		|| ((a > b) && (TO & 0x08))
		|| ((a == b) && (TO & 0x04))
		|| (((u64) a < (u64) b) && (TO & 0x02))
		|| (((u64) a > (u64) b) && (TO & 0x01)))
	{
		throw "Oh shit! It's a TRAP!";
	}
}
