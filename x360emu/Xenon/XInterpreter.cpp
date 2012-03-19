/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "Xenon.h"
#include "XInterpreter.h"
#include "../Memory.h"
#include "../HLE/HLE.h"
#include <XenonParser.h>
#include <Exceptions.h>
#include <iostream>
#include <vector>

using namespace std;

XInterpreter::handler XInterpreter::CallTable[PPC_OP_LAST];

XInterpreter::XInterpreter(Xenon::CpuState &state) : xState(state)
{
    SetupXenonTables();
    SetupCallTable();
}

void XInterpreter::Init()
{
}

vector<u32> Branches;

void XInterpreter::Run()
{
    xState.State = Xenon::CpuStates::Running;
    Branches.push_back(xState.pc);

    try
    {
        while (xState.State == Xenon::CpuStates::Running)
        {
            xState.npc = xState.pc + 4;

            xState.CurrentInstruction.hex = Memory::Read32(xState.pc);
            OpcodeInfo *info = GetInfo(xState.CurrentInstruction.hex);
            CallTable[info->Id](&xState);

            xState.pc = xState.npc;
            xState.CyclesSinceUpdate += info->Cycles;

            /* The Xbox 360 updates the TB each 64 cycles, or 50,000,000 times/s */
            if (xState.CyclesSinceUpdate >= 64)
            {
                /* Since we're LittleEndian, and TU is just after TL, we can use it as 64bit */
                u64 *tb = (u64 *) &xState.spr[268];
                (*tb)++;

                /* Since the timer period is a power of two, instead of "modulo", we use "and" (faster) */
                xState.CyclesSinceUpdate &= 63;
            }
        }
    }
    catch (Exception &exc)
    {
        cout << "Got Exception (" << exc.GetMessage() << ") at PC = 0x" << hex << xState.pc << endl;
        cout << "Backtrace:\n";

        int start = 0;
        if (Branches.size() > 10)
            start = Branches.size() - 10;

        for (int i = start; i < Branches.size(); i++)
        {
            cout << Branches[i];
            
            if (i + 1 != Branches.size())
                cout << " called " << Branches[i + 1];
            cout << endl;
        }
    }
    catch (...)
    {
        cout << "Got Exception (Unknown OpCode?) at PC = 0x" << hex << xState.pc << endl;
        cout << "Backtrace:\n";

        int start = 0;
        if (Branches.size() > 10)
            start = Branches.size() - 10;

        for (int i = start; i < Branches.size(); i++)
        {
            cout << Branches[i];
            
            if (i + 1 != Branches.size())
                cout << " called " << Branches[i + 1];
            cout << endl;
        }
    }
}

void XInterpreter::Stop()
{
}

void XInterpreter::OpInvalid(Xenon::CpuState *xState)
{
    OpcodeInfo *i = GetInfo(xState->CurrentInstruction.hex);
    if (i->Flags & OP_UNKNOWN)
        throw Exception("XInterpreter: Invalid OpCode");
    else
    {
        std::string err = "XInterpreter: OpCode ";
        err += i->Name;
        err += " not implemented yet!";
        throw Exception(err);
    }
}

void HleHandler(Xenon::CpuState *xState)
{
    HLE::Exec(xState, xState->CurrentInstruction.hex & 0xFFFFFF);
}

static bool TablesDone = false;
void XInterpreter::SetupCallTable()
{
    if (TablesDone)
        return;

    for (int i = 0; i < PPC_OP_LAST; i++)
        CallTable[i] = OpInvalid;

    CallTable[PPC_OP_HLE_CALL] = HleHandler;

    CallTable[PPC_OP_ADD] = OpAdd;
    CallTable[PPC_OP_ADDC] = OpAddc;
    CallTable[PPC_OP_ADDE] = OpAdde;
    CallTable[PPC_OP_ADDI] = OpAddi;
    CallTable[PPC_OP_ADDIC] = OpAddic;
    CallTable[PPC_OP_ADDIC_UP] = OpAddicUp;
    CallTable[PPC_OP_ADDIS] = OpAddis;
    CallTable[PPC_OP_ADDME] = OpAddme;
    CallTable[PPC_OP_ADDZE] = OpAddze;
    CallTable[PPC_OP_AND] = OpAnd;
    CallTable[PPC_OP_ANDC] = OpAndc;
    CallTable[PPC_OP_ANDI_UP] = OpAndiUp;
    CallTable[PPC_OP_ANDIS_UP] = OpAndisUp;
    CallTable[PPC_OP_B] = OpB;
    CallTable[PPC_OP_BC] = OpBc;
    CallTable[PPC_OP_BCCTR] = OpBcctr;
    CallTable[PPC_OP_BCLR] = OpBclr;
    CallTable[PPC_OP_CMP] = OpCmp;
    CallTable[PPC_OP_CMPI] = OpCmpi;
    CallTable[PPC_OP_CMPL] = OpCmpl;
    CallTable[PPC_OP_CMPLI] = OpCmpli;
    CallTable[PPC_OP_CNTLZD] = OpCntlzd;
    CallTable[PPC_OP_CNTLZW] = OpCntlzw;
    CallTable[PPC_OP_CRAND] = OpCrand;
    CallTable[PPC_OP_CRANDC] = OpCrandc;
    CallTable[PPC_OP_CREQV] = OpCreqv;
    CallTable[PPC_OP_CRNAND] = OpCrnand;
    CallTable[PPC_OP_CRNOR] = OpCrnor;
    CallTable[PPC_OP_CROR] = OpCror;
    CallTable[PPC_OP_CRORC] = OpCrorc;
    CallTable[PPC_OP_CRXOR] = OpCrxor;
#if 0
    CallTable[PPC_OP_DCBF] = OpDcbf;
    CallTable[PPC_OP_DCBST] = OpDcbst;
    CallTable[PPC_OP_DCBT] = OpDcbt;
    CallTable[PPC_OP_DCBTST] = OpDcbtst;
    CallTable[PPC_OP_DCBZ] = OpDcbz;
#endif
    CallTable[PPC_OP_DIVD] = OpDivd;
    CallTable[PPC_OP_DIVDU] = OpDivdu;
    CallTable[PPC_OP_DIVW] = OpDivw;
    CallTable[PPC_OP_DIVWU] = OpDivwu;
#if 0
    CallTable[PPC_OP_ECIWX] = OpEciwx;
    CallTable[PPC_OP_ECOWX] = OpEcowx;
    CallTable[PPC_OP_EIEIO] = OpEieio;
#endif
    CallTable[PPC_OP_EQV] = OpEqv;
    CallTable[PPC_OP_EXTSB] = OpExtsb;
    CallTable[PPC_OP_EXTSH] = OpExtsh;
    CallTable[PPC_OP_EXTSW] = OpExtsw;
#if 0
    CallTable[PPC_OP_FABS] = OpFabs;
    CallTable[PPC_OP_FADD] = OpFadd;
    CallTable[PPC_OP_FADDS] = OpFadds;
    CallTable[PPC_OP_FCFID] = OpFcfid;
    CallTable[PPC_OP_FCMPO] = OpFcmpo;
    CallTable[PPC_OP_FCMPU] = OpFcmpu;
    CallTable[PPC_OP_FCTID] = OpFctid;
    CallTable[PPC_OP_FCTIDZ] = OpFctidz;
    CallTable[PPC_OP_FCTIW] = OpFctiw;
    CallTable[PPC_OP_FCTIWZ] = OpFctiwz;
    CallTable[PPC_OP_FDIV] = OpFdiv;
    CallTable[PPC_OP_FDIVS] = OpFdivs;
    CallTable[PPC_OP_FMADD] = OpFmadd;
    CallTable[PPC_OP_FMADDS] = OpFmadds;
    CallTable[PPC_OP_FMR] = OpFmr;
    CallTable[PPC_OP_FMSUB] = OpFmsub;
    CallTable[PPC_OP_FMSUBS] = OpFmsubs;
    CallTable[PPC_OP_FMUL] = OpFmul;
    CallTable[PPC_OP_FMULS] = OpFmuls;
    CallTable[PPC_OP_FNABS] = OpFnabs;
    CallTable[PPC_OP_FNEG] = OpFneg;
    CallTable[PPC_OP_FNMADD] = OpFnmadd;
    CallTable[PPC_OP_FNMADDS] = OpFnmadds;
    CallTable[PPC_OP_FNMSUB] = OpFnmsub;
    CallTable[PPC_OP_FNMSUBS] = OpFnmsubs;
    CallTable[PPC_OP_FRES] = OpFres;
    CallTable[PPC_OP_FRSP] = OpFrsp;
    CallTable[PPC_OP_FRSQRTE] = OpFrsqrte;
    CallTable[PPC_OP_FSEL] = OpFsel;
    CallTable[PPC_OP_FSQRT] = OpFsqrt;
    CallTable[PPC_OP_FSQRTS] = OpFsqrts;
    CallTable[PPC_OP_FSUB] = OpFsub;
    CallTable[PPC_OP_FSUBS] = OpFsubs;
    CallTable[PPC_OP_ICBI] = OpIcbi;
    CallTable[PPC_OP_ISYNC] = OpIsync;
#endif
    CallTable[PPC_OP_LBZ] = OpLbz;
    CallTable[PPC_OP_LBZU] = OpLbzu;
    CallTable[PPC_OP_LBZUX] = OpLbzux;
    CallTable[PPC_OP_LBZX] = OpLbzx;
    CallTable[PPC_OP_LD] = OpLd;
    CallTable[PPC_OP_LDARX] = OpLdarx;
    CallTable[PPC_OP_LDU] = OpLdu;
    CallTable[PPC_OP_LDUX] = OpLdux;
    CallTable[PPC_OP_LDX] = OpLdx;
    CallTable[PPC_OP_LFD] = OpLfd;
    CallTable[PPC_OP_LFDU] = OpLfdu;
    CallTable[PPC_OP_LFDUX] = OpLfdux;
    CallTable[PPC_OP_LFDX] = OpLfdx;
    CallTable[PPC_OP_LFS] = OpLfs;
    CallTable[PPC_OP_LFSU] = OpLfsu;
    CallTable[PPC_OP_LFSUX] = OpLfsux;
    CallTable[PPC_OP_LFSX] = OpLfsx;
    CallTable[PPC_OP_LHA] = OpLha;
    CallTable[PPC_OP_LHAU] = OpLhau;
    CallTable[PPC_OP_LHAUX] = OpLhaux;
    CallTable[PPC_OP_LHAX] = OpLhax;
    CallTable[PPC_OP_LHBRX] = OpLhbrx;
    CallTable[PPC_OP_LHZ] = OpLhz;
    CallTable[PPC_OP_LHZU] = OpLhzu;
    CallTable[PPC_OP_LHZUX] = OpLhzux;
    CallTable[PPC_OP_LHZX] = OpLhzx;
    CallTable[PPC_OP_LMW] = OpLmw;
    CallTable[PPC_OP_LSWI] = OpLswi;
    CallTable[PPC_OP_LSWX] = OpLswx;
    CallTable[PPC_OP_LWA] = OpLwa;
    CallTable[PPC_OP_LWARX] = OpLwarx;
    CallTable[PPC_OP_LWAUX] = OpLwaux;
    CallTable[PPC_OP_LWAX] = OpLwax;
    CallTable[PPC_OP_LWBRX] = OpLwbrx;
    CallTable[PPC_OP_LWZ] = OpLwz;
    CallTable[PPC_OP_LWZU] = OpLwzu;
    CallTable[PPC_OP_LWZUX] = OpLwzux;
    CallTable[PPC_OP_LWZX] = OpLwzx;
#if 0
    CallTable[PPC_OP_MCRF] = OpMcrf;
    CallTable[PPC_OP_MCRFS] = OpMcrfs;
    CallTable[PPC_OP_MFCR] = OpMfcr;
    CallTable[PPC_OP_MFFS] = OpMffs;
#endif
    CallTable[PPC_OP_MFMSR] = OpMfmsr;
    CallTable[PPC_OP_MFSPR] = OpMfspr;
#if 0
    CallTable[PPC_OP_MFSR] = OpMfsr;
    CallTable[PPC_OP_MFSRIN] = OpMfsrin;
#endif
    CallTable[PPC_OP_MFTB] = OpMftb;
#if 0
    CallTable[PPC_OP_MTCRF] = OpMtcrf;
    CallTable[PPC_OP_MTFSB0] = OpMtfsb0;
    CallTable[PPC_OP_MTFSB1] = OpMtfsb1;
    CallTable[PPC_OP_MTFSF] = OpMtfsf;
    CallTable[PPC_OP_MTFSFI] = OpMtfsfi;
    CallTable[PPC_OP_MTMSR] = OpMtmsr;
#endif
    CallTable[PPC_OP_MTMSRD] = OpMtmsrd;
    CallTable[PPC_OP_MTSPR] = OpMtspr;
#if 0
    CallTable[PPC_OP_MTSR] = OpMtsr;
    CallTable[PPC_OP_MTSRIN] = OpMtsrin;
    CallTable[PPC_OP_MULHD] = OpMulthd;
    CallTable[PPC_OP_MULHDU] = OpMulthdu;
    CallTable[PPC_OP_MULHW] = OpMulhw;
    CallTable[PPC_OP_MULHWU] = OpMulhwu;
    CallTable[PPC_OP_MULLD] = OpMulld;
    CallTable[PPC_OP_MULLDO] = OpMulldo;
#endif
    CallTable[PPC_OP_MULLI] = OpMulli;
    CallTable[PPC_OP_MULLW] = OpMullw;
    CallTable[PPC_OP_NAND] = OpNand;
    CallTable[PPC_OP_NEG] = OpNeg;
    CallTable[PPC_OP_NOR] = OpNor;
    CallTable[PPC_OP_OR] = OpOr;
    CallTable[PPC_OP_ORC] = OpOrc;
    CallTable[PPC_OP_ORI] = OpOri;
    CallTable[PPC_OP_ORIS] = OpOris;
#if 0
    CallTable[PPC_OP_RFID] = OpRfid;
    CallTable[PPC_OP_RLDCL] = OpRldcl;
    CallTable[PPC_OP_RLDCR] = OpRldcr;
    CallTable[PPC_OP_RLDIC] = OpRldic;
    CallTable[PPC_OP_RLDICL] = OpRldicl;
    CallTable[PPC_OP_RLDICR] = OpRldicr;
    CallTable[PPC_OP_RLDIMI] = OpRldimi;
#endif
    CallTable[PPC_OP_RLWIMI] = OpRlwimi;
    CallTable[PPC_OP_RLWINM] = OpRlwinm;
#if 0
    CallTable[PPC_OP_RLWNM] = OpRlwnm;
    CallTable[PPC_OP_SC] = OpSc;
    CallTable[PPC_OP_SLBIA] = OpSlbia;
    CallTable[PPC_OP_SLBIE] = OpSlbie;
    CallTable[PPC_OP_SLBMFEE] = OpSlbmfee;
    CallTable[PPC_OP_SLBMFEV] = OpSlbmfev;
    CallTable[PPC_OP_SLBMTE] = OpSlbmte;
#endif
    CallTable[PPC_OP_SLD] = OpSld;
    CallTable[PPC_OP_SLW] = OpSlw;
#if 0
    CallTable[PPC_OP_SRAD] = OpSrad;
    CallTable[PPC_OP_SRADI] = OpSradi;
    CallTable[PPC_OP_SRAW] = OpSraw;
#endif
    CallTable[PPC_OP_SRAWI] = OpSrawi;
#if 0
    CallTable[PPC_OP_SRD] = OpSrd;
    CallTable[PPC_OP_SRW] = OpSrw;
#endif
    CallTable[PPC_OP_STB] = OpStb;
    CallTable[PPC_OP_STBU] = OpStbu;
    CallTable[PPC_OP_STBUX] = OpStbux;
    CallTable[PPC_OP_STBX] = OpStbx;
    CallTable[PPC_OP_STD] = OpStd;      // includes STDU
    CallTable[PPC_OP_STDCX_UP] = OpStdcx;
    CallTable[PPC_OP_STDUX] = OpStdux;
    CallTable[PPC_OP_STDX] = OpStdx;
    CallTable[PPC_OP_STFD] = OpStfd;
    CallTable[PPC_OP_STFDU] = OpStfdu;
    CallTable[PPC_OP_STFDUX] = OpStfdux;
    CallTable[PPC_OP_STFDX] = OpStfdx;
    CallTable[PPC_OP_STFIWX] = OpStfiwx;
    CallTable[PPC_OP_STFS] = OpStfs;
    CallTable[PPC_OP_STFSU] = OpStfsu;
    CallTable[PPC_OP_STFSUX] = OpStfsux;
    CallTable[PPC_OP_STFSX] = OpStfsx;
    CallTable[PPC_OP_STH] = OpSth;
    CallTable[PPC_OP_STHBRX] = OpSthbrx;
    CallTable[PPC_OP_STHU] = OpSthu;
    CallTable[PPC_OP_STHUX] = OpSthux;
    CallTable[PPC_OP_STHX] = OpSthx;
    CallTable[PPC_OP_STMW] = OpStmw;
    CallTable[PPC_OP_STSWI] = OpStswi;
    CallTable[PPC_OP_STSWX] = OpStswx;
    CallTable[PPC_OP_STW] = OpStw;
    CallTable[PPC_OP_STWBRX] = OpStwbrx;
    CallTable[PPC_OP_STWCX_UP] = OpStwcx;
    CallTable[PPC_OP_STWU] = OpStwu;
    CallTable[PPC_OP_STWUX] = OpStwux;
    CallTable[PPC_OP_STWX] = OpStwx;
    CallTable[PPC_OP_SUBF] = OpSubf;
    CallTable[PPC_OP_SUBFC] = OpSubfc;
    CallTable[PPC_OP_SUBFE] = OpSubfe;
    CallTable[PPC_OP_SUBFIC] = OpSubfic;
    CallTable[PPC_OP_SUBFME] = OpSubfme;
    CallTable[PPC_OP_SUBFZE] = OpSubfze;
#if 0
    CallTable[PPC_OP_SYNC] = OpSync;
    CallTable[PPC_OP_TD] = OpTd;
    CallTable[PPC_OP_TDI] = OpTdi;
    CallTable[PPC_OP_TLBIA] = OpTlbia;
    CallTable[PPC_OP_TLBIE] = OpTlbie;
    CallTable[PPC_OP_TLBIEL] = OpTlbiel;
    CallTable[PPC_OP_TLBSYNC] = OpTlbsync;
    CallTable[PPC_OP_TW] = OpTw;
#endif
    CallTable[PPC_OP_TWI] = OpTwi;
    CallTable[PPC_OP_XOR] = OpXor;
    CallTable[PPC_OP_XORI] = OpXori;
    CallTable[PPC_OP_XORIS] = OpXoris;

    CallTable[PPC_OP_INVALID] = OpInvalid;

    TablesDone = true;
}
