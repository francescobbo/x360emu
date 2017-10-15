/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "../XInterpreter.h"

bool ConditionCheck(Xenon::CpuState *xState)
{
    if ((INSTR.BO & 4) == 0)
        CTR--;

    bool crWanted = ((INSTR.BO >> 3) & 1);
    bool skipCr = ((INSTR.BO >> 4) & 1);
    bool skipCounter = ((INSTR.BO >> 2) & 1);
    int ctrCheck = ((CTR != 0) ^ (INSTR.BO >> 1)) & 1;
    bool counter = skipCounter || ctrCheck;
    bool condition = skipCr || (xState->GetCrBit(INSTR.BI) == crWanted);

    return counter && condition;
}

void XInterpreter::OpB(Xenon::CpuState *xState)
{
    if (INSTR.LK)
        LR = NPC;
    if (INSTR.AA)
        NPC = SignExt26(INSTR.LI << 2);
    else
        NPC = PC + SignExt26(INSTR.LI << 2);

    if (INSTR.LK)
        Branches.push_back(NPC);
}

void XInterpreter::OpBc(Xenon::CpuState *xState)
{
    if (ConditionCheck(xState))
    {
        if (INSTR.LK)
            LR = NPC;
        if (INSTR.AA)
            NPC = (s32) (INSTR.BD << 2);
        else
            NPC = PC + (s16) (INSTR.BD << 2);

        if (INSTR.LK)
            Branches.push_back(NPC);
    }
}

void XInterpreter::OpBcctr(Xenon::CpuState *xState)
{
    if (((INSTR.BO_2 >> 4) & 1) | ((xState->GetCrBit(INSTR.BI_2) & 1) == ((INSTR.BO_2 >> 3) & 1)))
    {
        NPC = CTR & (~3);
        if (INSTR.LK_3)
        {
            LR = PC + 4;
            Branches.push_back(NPC);
        }
    }
}

void XInterpreter::OpBclr(Xenon::CpuState *xState)
{
    if (ConditionCheck(xState))
    {
        NPC = LR & (~3);
        if (INSTR.LK_3)
            LR = PC + 4;
        else
            Branches.pop_back();
    }
}
