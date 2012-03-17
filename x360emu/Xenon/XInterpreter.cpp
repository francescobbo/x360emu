#include "Xenon.h"
#include "XInterpreter.h"
#include "../Memory.h"

XenonParser mainTables;

XInterpreter::XInterpreter(Xenon::CpuState &state) : xState(state)
{
	SetupTables();

	xp.SetParam(&state);
	xp.CopyTables(mainTables);
}

void XInterpreter::Init()
{
}

#include <iostream>
#include <vector>
using namespace std;

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
			xp.Parse(xState.CurrentInstruction.hex);

			xState.pc = xState.npc;
		}
	}
	catch (...)
	{
		cout << "Got Exception (Unknown OpCode?) at PC = 0x" << hex << xState.pc << endl;
		cout << "Backtrace:\n";
		for (int i = 0; i < Branches.size(); i++)
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

#include "../HLE/HLE.h"
void HleHandler(Xenon::CpuState *xState)
{
	HLE::Exec(xState, xState->CurrentInstruction.hex & 0xFFFFFF);
}

static bool TablesDone = false;
void XInterpreter::SetupTables()
{
	if (TablesDone)
		return;

	mainTables.Register(Mnemonics::PPC_OP_HLE_CALL, (handler) HleHandler);

	mainTables.Register(Mnemonics::PPC_OP_ADD, (handler) OpAdd);
	mainTables.Register(Mnemonics::PPC_OP_ADDO, (handler) OpAddo);
	mainTables.Register(Mnemonics::PPC_OP_ADDC, (handler) OpAddc);
	mainTables.Register(Mnemonics::PPC_OP_ADDCO, (handler) OpAddco);
	mainTables.Register(Mnemonics::PPC_OP_ADDE, (handler) OpAdde);
	mainTables.Register(Mnemonics::PPC_OP_ADDEO, (handler) OpAddeo);
	mainTables.Register(Mnemonics::PPC_OP_ADDI, (handler) OpAddi);
	mainTables.Register(Mnemonics::PPC_OP_ADDIC, (handler) OpAddic);
	mainTables.Register(Mnemonics::PPC_OP_ADDIC_UP, (handler) OpAddicUp);
	mainTables.Register(Mnemonics::PPC_OP_ADDIS, (handler) OpAddis);
	mainTables.Register(Mnemonics::PPC_OP_ADDME, (handler) OpAddme);
	mainTables.Register(Mnemonics::PPC_OP_ADDMEO, (handler) OpAddmeo);
	mainTables.Register(Mnemonics::PPC_OP_ADDZE, (handler) OpAddze);
	mainTables.Register(Mnemonics::PPC_OP_ADDZEO, (handler) OpAddzeo);
	mainTables.Register(Mnemonics::PPC_OP_AND, (handler) OpAnd);
	mainTables.Register(Mnemonics::PPC_OP_ANDC, (handler) OpAndc);
	mainTables.Register(Mnemonics::PPC_OP_ANDI_UP, (handler) OpAndiUp);
	mainTables.Register(Mnemonics::PPC_OP_ANDIS_UP, (handler) OpAndisUp);
	mainTables.Register(Mnemonics::PPC_OP_B, (handler) OpB);
	mainTables.Register(Mnemonics::PPC_OP_BC, (handler) OpBc);
	mainTables.Register(Mnemonics::PPC_OP_BCCTR, (handler) OpBcctr);
	mainTables.Register(Mnemonics::PPC_OP_BCLR, (handler) OpBclr);
	mainTables.Register(Mnemonics::PPC_OP_CMP, (handler) OpCmp);
	mainTables.Register(Mnemonics::PPC_OP_CMPI, (handler) OpCmpi);
	mainTables.Register(Mnemonics::PPC_OP_CMPL, (handler) OpCmpl);
	mainTables.Register(Mnemonics::PPC_OP_CMPLI, (handler) OpCmpli);
	mainTables.Register(Mnemonics::PPC_OP_CNTLZD, (handler) OpCntlzd);
	mainTables.Register(Mnemonics::PPC_OP_CNTLZW, (handler) OpCntlzw);
	mainTables.Register(Mnemonics::PPC_OP_CRAND, (handler) OpCrand);
	mainTables.Register(Mnemonics::PPC_OP_CRANDC, (handler) OpCrandc);
	mainTables.Register(Mnemonics::PPC_OP_CREQV, (handler) OpCreqv);
	mainTables.Register(Mnemonics::PPC_OP_CRNAND, (handler) OpCrnand);
	mainTables.Register(Mnemonics::PPC_OP_CRNOR, (handler) OpCrnor);
	mainTables.Register(Mnemonics::PPC_OP_CROR, (handler) OpCror);
	mainTables.Register(Mnemonics::PPC_OP_CRORC, (handler) OpCrorc);
	mainTables.Register(Mnemonics::PPC_OP_CRXOR, (handler) OpCrxor);
#if 0
	mainTables.Register(Mnemonics::PPC_OP_DCBF, (handler) OpDcbf);
	mainTables.Register(Mnemonics::PPC_OP_DCBST, (handler) OpDcbst);
	mainTables.Register(Mnemonics::PPC_OP_DCBT, (handler) OpDcbt);
	mainTables.Register(Mnemonics::PPC_OP_DCBTST, (handler) OpDcbtst);
	mainTables.Register(Mnemonics::PPC_OP_DCBZ, (handler) OpDcbz);
#endif
	mainTables.Register(Mnemonics::PPC_OP_DIVD, (handler) OpDivd);
	mainTables.Register(Mnemonics::PPC_OP_DIVDO, (handler) OpDivdo);
	mainTables.Register(Mnemonics::PPC_OP_DIVDU, (handler) OpDivdu);
	mainTables.Register(Mnemonics::PPC_OP_DIVDUO, (handler) OpDivduo);
	mainTables.Register(Mnemonics::PPC_OP_DIVW, (handler) OpDivw);
	mainTables.Register(Mnemonics::PPC_OP_DIVWO, (handler) OpDivwo);
	mainTables.Register(Mnemonics::PPC_OP_DIVWU, (handler) OpDivwu);
	mainTables.Register(Mnemonics::PPC_OP_DIVWUO, (handler) OpDivwuo);
#if 0
	mainTables.Register(Mnemonics::PPC_OP_ECIWX, (handler) OpEciwx);
	mainTables.Register(Mnemonics::PPC_OP_ECOWX, (handler) OpEcowx);
	mainTables.Register(Mnemonics::PPC_OP_EIEIO, (handler) OpEieio);
#endif
	mainTables.Register(Mnemonics::PPC_OP_EQV, (handler) OpEqv);
	mainTables.Register(Mnemonics::PPC_OP_EXTSB, (handler) OpExtsb);
	mainTables.Register(Mnemonics::PPC_OP_EXTSH, (handler) OpExtsh);
	mainTables.Register(Mnemonics::PPC_OP_EXTSW, (handler) OpExtsw);
#if 0
	mainTables.Register(Mnemonics::PPC_OP_FABS, (handler) OpFabs);
	mainTables.Register(Mnemonics::PPC_OP_FADD, (handler) OpFadd);
	mainTables.Register(Mnemonics::PPC_OP_FADDS, (handler) OpFadds);
	mainTables.Register(Mnemonics::PPC_OP_FCFID, (handler) OpFcfid);
	mainTables.Register(Mnemonics::PPC_OP_FCMPO, (handler) OpFcmpo);
	mainTables.Register(Mnemonics::PPC_OP_FCMPU, (handler) OpFcmpu);
	mainTables.Register(Mnemonics::PPC_OP_FCTID, (handler) OpFctid);
	mainTables.Register(Mnemonics::PPC_OP_FCTIDZ, (handler) OpFctidz);
	mainTables.Register(Mnemonics::PPC_OP_FCTIW, (handler) OpFctiw);
	mainTables.Register(Mnemonics::PPC_OP_FCTIWZ, (handler) OpFctiwz);
	mainTables.Register(Mnemonics::PPC_OP_FDIV, (handler) OpFdiv);
	mainTables.Register(Mnemonics::PPC_OP_FDIVS, (handler) OpFdivs);
	mainTables.Register(Mnemonics::PPC_OP_FMADD, (handler) OpFmadd);
	mainTables.Register(Mnemonics::PPC_OP_FMADDS, (handler) OpFmadds);
	mainTables.Register(Mnemonics::PPC_OP_FMR, (handler) OpFmr);
	mainTables.Register(Mnemonics::PPC_OP_FMSUB, (handler) OpFmsub);
	mainTables.Register(Mnemonics::PPC_OP_FMSUBS, (handler) OpFmsubs);
	mainTables.Register(Mnemonics::PPC_OP_FMUL, (handler) OpFmul);
	mainTables.Register(Mnemonics::PPC_OP_FMULS, (handler) OpFmuls);
	mainTables.Register(Mnemonics::PPC_OP_FNABS, (handler) OpFnabs);
	mainTables.Register(Mnemonics::PPC_OP_FNEG, (handler) OpFneg);
	mainTables.Register(Mnemonics::PPC_OP_FNMADD, (handler) OpFnmadd);
	mainTables.Register(Mnemonics::PPC_OP_FNMADDS, (handler) OpFnmadds);
	mainTables.Register(Mnemonics::PPC_OP_FNMSUB, (handler) OpFnmsub);
	mainTables.Register(Mnemonics::PPC_OP_FNMSUBS, (handler) OpFnmsubs);
	mainTables.Register(Mnemonics::PPC_OP_FRES, (handler) OpFres);
	mainTables.Register(Mnemonics::PPC_OP_FRSP, (handler) OpFrsp);
	mainTables.Register(Mnemonics::PPC_OP_FRSQRTE, (handler) OpFrsqrte);
	mainTables.Register(Mnemonics::PPC_OP_FSEL, (handler) OpFsel);
	mainTables.Register(Mnemonics::PPC_OP_FSQRT, (handler) OpFsqrt);
	mainTables.Register(Mnemonics::PPC_OP_FSQRTS, (handler) OpFsqrts);
	mainTables.Register(Mnemonics::PPC_OP_FSUB, (handler) OpFsub);
	mainTables.Register(Mnemonics::PPC_OP_FSUBS, (handler) OpFsubs);
	mainTables.Register(Mnemonics::PPC_OP_ICBI, (handler) OpIcbi);
	mainTables.Register(Mnemonics::PPC_OP_ISYNC, (handler) OpIsync);
#endif
	mainTables.Register(Mnemonics::PPC_OP_LBZ, (handler) OpLbz);
	mainTables.Register(Mnemonics::PPC_OP_LBZU, (handler) OpLbzu);
	mainTables.Register(Mnemonics::PPC_OP_LBZUX, (handler) OpLbzux);
	mainTables.Register(Mnemonics::PPC_OP_LBZX, (handler) OpLbzx);
	mainTables.Register(Mnemonics::PPC_OP_LD, (handler) OpLd);
	mainTables.Register(Mnemonics::PPC_OP_LDARX, (handler) OpLdarx);
	mainTables.Register(Mnemonics::PPC_OP_LDU, (handler) OpLdu);
	mainTables.Register(Mnemonics::PPC_OP_LDUX, (handler) OpLdux);
	mainTables.Register(Mnemonics::PPC_OP_LDX, (handler) OpLdx);
	mainTables.Register(Mnemonics::PPC_OP_LFD, (handler) OpLfd);
	mainTables.Register(Mnemonics::PPC_OP_LFDU, (handler) OpLfdu);
	mainTables.Register(Mnemonics::PPC_OP_LFDUX, (handler) OpLfdux);
	mainTables.Register(Mnemonics::PPC_OP_LFDX, (handler) OpLfdx);
	mainTables.Register(Mnemonics::PPC_OP_LFS, (handler) OpLfs);
	mainTables.Register(Mnemonics::PPC_OP_LFSU, (handler) OpLfsu);
	mainTables.Register(Mnemonics::PPC_OP_LFSUX, (handler) OpLfsux);
	mainTables.Register(Mnemonics::PPC_OP_LFSX, (handler) OpLfsx);
	mainTables.Register(Mnemonics::PPC_OP_LHA, (handler) OpLha);
	mainTables.Register(Mnemonics::PPC_OP_LHAU, (handler) OpLhau);
	mainTables.Register(Mnemonics::PPC_OP_LHAUX, (handler) OpLhaux);
	mainTables.Register(Mnemonics::PPC_OP_LHAX, (handler) OpLhax);
	mainTables.Register(Mnemonics::PPC_OP_LHBRX, (handler) OpLhbrx);
	mainTables.Register(Mnemonics::PPC_OP_LHZ, (handler) OpLhz);
	mainTables.Register(Mnemonics::PPC_OP_LHZU, (handler) OpLhzu);
	mainTables.Register(Mnemonics::PPC_OP_LHZUX, (handler) OpLhzux);
	mainTables.Register(Mnemonics::PPC_OP_LHZX, (handler) OpLhzx);
	mainTables.Register(Mnemonics::PPC_OP_LMW, (handler) OpLmw);
	mainTables.Register(Mnemonics::PPC_OP_LSWI, (handler) OpLswi);
	mainTables.Register(Mnemonics::PPC_OP_LSWX, (handler) OpLswx);
	mainTables.Register(Mnemonics::PPC_OP_LWA, (handler) OpLwa);
	mainTables.Register(Mnemonics::PPC_OP_LWARX, (handler) OpLwarx);
	mainTables.Register(Mnemonics::PPC_OP_LWAUX, (handler) OpLwaux);
	mainTables.Register(Mnemonics::PPC_OP_LWAX, (handler) OpLwax);
	mainTables.Register(Mnemonics::PPC_OP_LWBRX, (handler) OpLwbrx);
	mainTables.Register(Mnemonics::PPC_OP_LWZ, (handler) OpLwz);
	mainTables.Register(Mnemonics::PPC_OP_LWZU, (handler) OpLwzu);
	mainTables.Register(Mnemonics::PPC_OP_LWZUX, (handler) OpLwzux);
	mainTables.Register(Mnemonics::PPC_OP_LWZX, (handler) OpLwzx);
#if 0
	mainTables.Register(Mnemonics::PPC_OP_MCRF, (handler) OpMcrf);
	mainTables.Register(Mnemonics::PPC_OP_MCRFS, (handler) OpMcrfs);
	mainTables.Register(Mnemonics::PPC_OP_MFCR, (handler) OpMfcr);
	mainTables.Register(Mnemonics::PPC_OP_MFFS, (handler) OpMffs);
	mainTables.Register(Mnemonics::PPC_OP_MFMSR, (handler) OpMfmsr);
#endif
	mainTables.Register(Mnemonics::PPC_OP_MFSPR, (handler) OpMfspr);
#if 0
	mainTables.Register(Mnemonics::PPC_OP_MFSR, (handler) OpMfsr);
	mainTables.Register(Mnemonics::PPC_OP_MFSRIN, (handler) OpMfsrin);
	mainTables.Register(Mnemonics::PPC_OP_MFTB, (handler) OpMftb);
	mainTables.Register(Mnemonics::PPC_OP_MTCRF, (handler) OpMtcrf);
	mainTables.Register(Mnemonics::PPC_OP_MTFSB0, (handler) OpMtfsb0);
	mainTables.Register(Mnemonics::PPC_OP_MTFSB1, (handler) OpMtfsb1);
	mainTables.Register(Mnemonics::PPC_OP_MTFSF, (handler) OpMtfsf);
	mainTables.Register(Mnemonics::PPC_OP_MTFSFI, (handler) OpMtfsfi);
	mainTables.Register(Mnemonics::PPC_OP_MTMSR, (handler) OpMtmsr);
	mainTables.Register(Mnemonics::PPC_OP_MTMSRD, (handler) OpMtmsrd);
#endif
	mainTables.Register(Mnemonics::PPC_OP_MTSPR, (handler) OpMtspr);
#if 0
	mainTables.Register(Mnemonics::PPC_OP_MTSR, (handler) OpMtsr);
	mainTables.Register(Mnemonics::PPC_OP_MTSRIN, (handler) OpMtsrin);
	mainTables.Register(Mnemonics::PPC_OP_MULHD, (handler) OpMulthd);
	mainTables.Register(Mnemonics::PPC_OP_MULHDU, (handler) OpMulthdu);
	mainTables.Register(Mnemonics::PPC_OP_MULHW, (handler) OpMulhw);
	mainTables.Register(Mnemonics::PPC_OP_MULHWU, (handler) OpMulhwu);
	mainTables.Register(Mnemonics::PPC_OP_MULLD, (handler) OpMulld);
	mainTables.Register(Mnemonics::PPC_OP_MULLDO, (handler) OpMulldo);
#endif
	mainTables.Register(Mnemonics::PPC_OP_MULLI, (handler) OpMulli);
	mainTables.Register(Mnemonics::PPC_OP_MULLW, (handler) OpMullw);
	mainTables.Register(Mnemonics::PPC_OP_MULLWO, (handler) OpMullwo);
#if 0
	mainTables.Register(Mnemonics::PPC_OP_NAND, (handler) OpNand);
	mainTables.Register(Mnemonics::PPC_OP_NEG, (handler) OpNeg);
	mainTables.Register(Mnemonics::PPC_OP_NEGO, (handler) OpNego);
	mainTables.Register(Mnemonics::PPC_OP_NOR, (handler) OpNor);
#endif
	mainTables.Register(Mnemonics::PPC_OP_OR, (handler) OpOr);
	mainTables.Register(Mnemonics::PPC_OP_ORC, (handler) OpOrc);
	mainTables.Register(Mnemonics::PPC_OP_ORI, (handler) OpOri);
	mainTables.Register(Mnemonics::PPC_OP_ORIS, (handler) OpOris);
#if 0
	mainTables.Register(Mnemonics::PPC_OP_RFID, (handler) OpRfid);
	mainTables.Register(Mnemonics::PPC_OP_RLDCL, (handler) OpRldcl);
	mainTables.Register(Mnemonics::PPC_OP_RLDCR, (handler) OpRldcr);
	mainTables.Register(Mnemonics::PPC_OP_RLDIC, (handler) OpRldic);
	mainTables.Register(Mnemonics::PPC_OP_RLDICL, (handler) OpRldicl);
	mainTables.Register(Mnemonics::PPC_OP_RLDICR, (handler) OpRldicr);
	mainTables.Register(Mnemonics::PPC_OP_RLDIMI, (handler) OpRldimi);
#endif
	mainTables.Register(Mnemonics::PPC_OP_RLWIMI, (handler) OpRlwimi);
	mainTables.Register(Mnemonics::PPC_OP_RLWINM, (handler) OpRlwinm);
#if 0
	mainTables.Register(Mnemonics::PPC_OP_RLWNM, (handler) OpRlwnm);
	mainTables.Register(Mnemonics::PPC_OP_SC, (handler) OpSc);
	mainTables.Register(Mnemonics::PPC_OP_SLBIA, (handler) OpSlbia);
	mainTables.Register(Mnemonics::PPC_OP_SLBIE, (handler) OpSlbie);
	mainTables.Register(Mnemonics::PPC_OP_SLBMFEE, (handler) OpSlbmfee);
	mainTables.Register(Mnemonics::PPC_OP_SLBMFEV, (handler) OpSlbmfev);
	mainTables.Register(Mnemonics::PPC_OP_SLBMTE, (handler) OpSlbmte);
#endif
	mainTables.Register(Mnemonics::PPC_OP_SLD, (handler) OpSld);
	mainTables.Register(Mnemonics::PPC_OP_SLW, (handler) OpSlw);
#if 0
	mainTables.Register(Mnemonics::PPC_OP_SRAD, (handler) OpSrad);
	mainTables.Register(Mnemonics::PPC_OP_SRADI, (handler) OpSradi);
	mainTables.Register(Mnemonics::PPC_OP_SRADI1, (handler) OpSradi);		/* Ugly hack. This is actually the same instruction differing just for 1 bit. See PPC spec for details */
	mainTables.Register(Mnemonics::PPC_OP_SRAW, (handler) OpSraw);
#endif
	mainTables.Register(Mnemonics::PPC_OP_SRAWI, (handler) OpSrawi);
#if 0
	mainTables.Register(Mnemonics::PPC_OP_SRD, (handler) OpSrd);
	mainTables.Register(Mnemonics::PPC_OP_SRW, (handler) OpSrw);
#endif
	mainTables.Register(Mnemonics::PPC_OP_STB, (handler) OpStb);
	mainTables.Register(Mnemonics::PPC_OP_STBU, (handler) OpStbu);
	mainTables.Register(Mnemonics::PPC_OP_STBUX, (handler) OpStbux);
	mainTables.Register(Mnemonics::PPC_OP_STBX, (handler) OpStbx);
	mainTables.Register(Mnemonics::PPC_OP_STD, (handler) OpStd);
	mainTables.Register(Mnemonics::PPC_OP_STDCX_UP, (handler) OpStdcx);
	mainTables.Register(Mnemonics::PPC_OP_STDU, (handler) OpStdu);
	mainTables.Register(Mnemonics::PPC_OP_STDUX, (handler) OpStdux);
	mainTables.Register(Mnemonics::PPC_OP_STDX, (handler) OpStdx);
	mainTables.Register(Mnemonics::PPC_OP_STFD, (handler) OpStfd);
	mainTables.Register(Mnemonics::PPC_OP_STFDU, (handler) OpStfdu);
	mainTables.Register(Mnemonics::PPC_OP_STFDUX, (handler) OpStfdux);
	mainTables.Register(Mnemonics::PPC_OP_STFDX, (handler) OpStfdx);
	mainTables.Register(Mnemonics::PPC_OP_STFIWX, (handler) OpStfiwx);
	mainTables.Register(Mnemonics::PPC_OP_STFS, (handler) OpStfs);
	mainTables.Register(Mnemonics::PPC_OP_STFSU, (handler) OpStfsu);
	mainTables.Register(Mnemonics::PPC_OP_STFSUX, (handler) OpStfsux);
	mainTables.Register(Mnemonics::PPC_OP_STFSX, (handler) OpStfsx);
	mainTables.Register(Mnemonics::PPC_OP_STH, (handler) OpSth);
	mainTables.Register(Mnemonics::PPC_OP_STHBRX, (handler) OpSthbrx);
	mainTables.Register(Mnemonics::PPC_OP_STHU, (handler) OpSthu);
	mainTables.Register(Mnemonics::PPC_OP_STHUX, (handler) OpSthux);
	mainTables.Register(Mnemonics::PPC_OP_STHX, (handler) OpSthx);
	mainTables.Register(Mnemonics::PPC_OP_STMW, (handler) OpStmw);
	mainTables.Register(Mnemonics::PPC_OP_STSWI, (handler) OpStswi);
	mainTables.Register(Mnemonics::PPC_OP_STSWX, (handler) OpStswx);
	mainTables.Register(Mnemonics::PPC_OP_STW, (handler) OpStw);
	mainTables.Register(Mnemonics::PPC_OP_STWBRX, (handler) OpStwbrx);
	mainTables.Register(Mnemonics::PPC_OP_STWCX_UP, (handler) OpStwcx);
	mainTables.Register(Mnemonics::PPC_OP_STWU, (handler) OpStwu);
	mainTables.Register(Mnemonics::PPC_OP_STWUX, (handler) OpStwux);
	mainTables.Register(Mnemonics::PPC_OP_STWX, (handler) OpStwx);
	mainTables.Register(Mnemonics::PPC_OP_SUBF, (handler) OpSubf);
	mainTables.Register(Mnemonics::PPC_OP_SUBFO, (handler) OpSubfo);
	mainTables.Register(Mnemonics::PPC_OP_SUBFC, (handler) OpSubfc);
	mainTables.Register(Mnemonics::PPC_OP_SUBFCO, (handler) OpSubfco);
	mainTables.Register(Mnemonics::PPC_OP_SUBFE, (handler) OpSubfe);
	mainTables.Register(Mnemonics::PPC_OP_SUBFEO, (handler) OpSubfeo);
	mainTables.Register(Mnemonics::PPC_OP_SUBFIC, (handler) OpSubfic);
	mainTables.Register(Mnemonics::PPC_OP_SUBFME, (handler) OpSubfme);
	mainTables.Register(Mnemonics::PPC_OP_SUBFMEO, (handler) OpSubfmeo);
	mainTables.Register(Mnemonics::PPC_OP_SUBFZE, (handler) OpSubfze);
	mainTables.Register(Mnemonics::PPC_OP_SUBFZEO, (handler) OpSubfzeo);
#if 0
	mainTables.Register(Mnemonics::PPC_OP_SYNC, (handler) OpSync);
	mainTables.Register(Mnemonics::PPC_OP_TD, (handler) OpTd);
	mainTables.Register(Mnemonics::PPC_OP_TDI, (handler) OpTdi);
	mainTables.Register(Mnemonics::PPC_OP_TLBIA, (handler) OpTlbia);
	mainTables.Register(Mnemonics::PPC_OP_TLBIE, (handler) OpTlbie);
	mainTables.Register(Mnemonics::PPC_OP_TLBIEL, (handler) OpTlbiel);
	mainTables.Register(Mnemonics::PPC_OP_TLBSYNC, (handler) OpTlbsync);
	mainTables.Register(Mnemonics::PPC_OP_TW, (handler) OpTw);
#endif
	mainTables.Register(Mnemonics::PPC_OP_TWI, (handler) OpTwi);
	mainTables.Register(Mnemonics::PPC_OP_XOR, (handler) OpXor);
	mainTables.Register(Mnemonics::PPC_OP_XORI, (handler) OpXori);
	mainTables.Register(Mnemonics::PPC_OP_XORIS, (handler) OpXoris);

	TablesDone = true;
}
