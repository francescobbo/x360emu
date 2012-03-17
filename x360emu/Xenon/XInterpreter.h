#pragma once

#include "Xenon.h"
#include <XenonParser.h>

#define PC		xState->pc
#define NPC		xState->npc
#define rGPR	xState->gpr
#define rFP		xState->fpr
#define rSPR	xState->spr
#define INSTR	xState->CurrentInstruction
#define LR		xState->spr[8]
#define CTR		xState->spr[9]
#define XER		((RegXER &) xState->spr[1])

class XInterpreter : public XenonEmulator
{
public:
	XInterpreter(Xenon::CpuState &state);

	void Init();

	void Run();
	void Stop();

private:
	void SetupTables();

	Xenon::CpuState &xState;
	XenonParser xp;

	/* OpCode handlers */
	static void OpAdd(Xenon::CpuState *xState);
	static void OpAddo(Xenon::CpuState *xState);
	static void OpAddc(Xenon::CpuState *xState);
	static void OpAddco(Xenon::CpuState *xState);
	static void OpAdde(Xenon::CpuState *xState);
	static void OpAddeo(Xenon::CpuState *xState);
	static void OpAddi(Xenon::CpuState *xState);
	static void OpAddic(Xenon::CpuState *xState);
	static void OpAddicUp(Xenon::CpuState *xState);
	static void OpAddis(Xenon::CpuState *xState);
	static void OpAddme(Xenon::CpuState *xState);
	static void OpAddmeo(Xenon::CpuState *xState);
	static void OpAddze(Xenon::CpuState *xState);
	static void OpAddzeo(Xenon::CpuState *xState);
	static void OpAnd(Xenon::CpuState *xState);
	static void OpAndc(Xenon::CpuState *xState);
	static void OpAndiUp(Xenon::CpuState *xState);
	static void OpAndisUp(Xenon::CpuState *xState);
	static void OpB(Xenon::CpuState *xState);
	static void OpBc(Xenon::CpuState *xState);
	static void OpBcctr(Xenon::CpuState *xState);
	static void OpBclr(Xenon::CpuState *xState);
	static void OpCmp(Xenon::CpuState *xState);
	static void OpCmpi(Xenon::CpuState *xState);
	static void OpCmpl(Xenon::CpuState *xState);
	static void OpCmpli(Xenon::CpuState *xState);
	static void OpCntlzd(Xenon::CpuState *xState);
	static void OpCntlzw(Xenon::CpuState *xState);
	static void OpCrand(Xenon::CpuState *xState);
	static void OpCrandc(Xenon::CpuState *xState);
	static void OpCreqv(Xenon::CpuState *xState);
	static void OpCrnand(Xenon::CpuState *xState);
	static void OpCrnor(Xenon::CpuState *xState);
	static void OpCror(Xenon::CpuState *xState);
	static void OpCrorc(Xenon::CpuState *xState);
	static void OpCrxor(Xenon::CpuState *xState);
	static void OpDcbf(Xenon::CpuState *xState);
	static void OpDcbst(Xenon::CpuState *xState);
	static void OpDcbt(Xenon::CpuState *xState);
	static void OpDcbtst(Xenon::CpuState *xState);
	static void OpDcbz(Xenon::CpuState *xState);
	static void OpDivd(Xenon::CpuState *xState);
	static void OpDivdo(Xenon::CpuState *xState);
	static void OpDivdu(Xenon::CpuState *xState);
	static void OpDivduo(Xenon::CpuState *xState);
	static void OpDivw(Xenon::CpuState *xState);
	static void OpDivwo(Xenon::CpuState *xState);
	static void OpDivwu(Xenon::CpuState *xState);
	static void OpDivwuo(Xenon::CpuState *xState);
	static void OpEciwx(Xenon::CpuState *xState);
	static void OpEcowx(Xenon::CpuState *xState);
	static void OpEieio(Xenon::CpuState *xState);
	static void OpEqv(Xenon::CpuState *xState);
	static void OpExtsb(Xenon::CpuState *xState);
	static void OpExtsh(Xenon::CpuState *xState);
	static void OpExtsw(Xenon::CpuState *xState);
	static void OpFabs(Xenon::CpuState *xState);
	static void OpFadd(Xenon::CpuState *xState);
	static void OpFadds(Xenon::CpuState *xState);
	static void OpFcfid(Xenon::CpuState *xState);
	static void OpFcmpo(Xenon::CpuState *xState);
	static void OpFcmpu(Xenon::CpuState *xState);
	static void OpFctid(Xenon::CpuState *xState);
	static void OpFctidz(Xenon::CpuState *xState);
	static void OpFctiw(Xenon::CpuState *xState);
	static void OpFctiwz(Xenon::CpuState *xState);
	static void OpFdiv(Xenon::CpuState *xState);
	static void OpFdivs(Xenon::CpuState *xState);
	static void OpFmadd(Xenon::CpuState *xState);
	static void OpFmadds(Xenon::CpuState *xState);
	static void OpFmr(Xenon::CpuState *xState);
	static void OpFmsub(Xenon::CpuState *xState);
	static void OpFmsubs(Xenon::CpuState *xState);
	static void OpFmul(Xenon::CpuState *xState);
	static void OpFmuls(Xenon::CpuState *xState);
	static void OpFnabs(Xenon::CpuState *xState);
	static void OpFneg(Xenon::CpuState *xState);
	static void OpFnmadd(Xenon::CpuState *xState);
	static void OpFnmadds(Xenon::CpuState *xState);
	static void OpFnmsub(Xenon::CpuState *xState);
	static void OpFnmsubs(Xenon::CpuState *xState);
	static void OpFres(Xenon::CpuState *xState);
	static void OpFrsp(Xenon::CpuState *xState);
	static void OpFrsqrte(Xenon::CpuState *xState);
	static void OpFsel(Xenon::CpuState *xState);
	static void OpFsqrt(Xenon::CpuState *xState);
	static void OpFsqrts(Xenon::CpuState *xState);
	static void OpFsub(Xenon::CpuState *xState);
	static void OpFsubs(Xenon::CpuState *xState);
	static void OpIcbi(Xenon::CpuState *xState);
	static void OpIsync(Xenon::CpuState *xState);
	static void OpLbz(Xenon::CpuState *xState);
	static void OpLbzu(Xenon::CpuState *xState);
	static void OpLbzux(Xenon::CpuState *xState);
	static void OpLbzx(Xenon::CpuState *xState);
	static void OpLd(Xenon::CpuState *xState);
	static void OpLdarx(Xenon::CpuState *xState);
	static void OpLdu(Xenon::CpuState *xState);
	static void OpLdux(Xenon::CpuState *xState);
	static void OpLdx(Xenon::CpuState *xState);
	static void OpLfd(Xenon::CpuState *xState);
	static void OpLfdu(Xenon::CpuState *xState);
	static void OpLfdux(Xenon::CpuState *xState);
	static void OpLfdx(Xenon::CpuState *xState);
	static void OpLfs(Xenon::CpuState *xState);
	static void OpLfsu(Xenon::CpuState *xState);
	static void OpLfsux(Xenon::CpuState *xState);
	static void OpLfsx(Xenon::CpuState *xState);
	static void OpLha(Xenon::CpuState *xState);
	static void OpLhau(Xenon::CpuState *xState);
	static void OpLhaux(Xenon::CpuState *xState);
	static void OpLhax(Xenon::CpuState *xState);
	static void OpLhbrx(Xenon::CpuState *xState);
	static void OpLhz(Xenon::CpuState *xState);
	static void OpLhzu(Xenon::CpuState *xState);
	static void OpLhzux(Xenon::CpuState *xState);
	static void OpLhzx(Xenon::CpuState *xState);
	static void OpLmw(Xenon::CpuState *xState);
	static void OpLswi(Xenon::CpuState *xState);
	static void OpLswx(Xenon::CpuState *xState);
	static void OpLwa(Xenon::CpuState *xState);
	static void OpLwarx(Xenon::CpuState *xState);
	static void OpLwaux(Xenon::CpuState *xState);
	static void OpLwax(Xenon::CpuState *xState);
	static void OpLwbrx(Xenon::CpuState *xState);
	static void OpLwz(Xenon::CpuState *xState);
	static void OpLwzu(Xenon::CpuState *xState);
	static void OpLwzux(Xenon::CpuState *xState);
	static void OpLwzx(Xenon::CpuState *xState);
	static void OpMcrf(Xenon::CpuState *xState);
	static void OpMcrfs(Xenon::CpuState *xState);
	static void OpMfcr(Xenon::CpuState *xState);
	static void OpMffs(Xenon::CpuState *xState);
	static void OpMfmsr(Xenon::CpuState *xState);
	static void OpMfspr(Xenon::CpuState *xState);
	static void OpMfsr(Xenon::CpuState *xState);
	static void OpMfsrin(Xenon::CpuState *xState);
	static void OpMftb(Xenon::CpuState *xState);
	static void OpMtcrf(Xenon::CpuState *xState);
	static void OpMtfsb0(Xenon::CpuState *xState);
	static void OpMtfsb1(Xenon::CpuState *xState);
	static void OpMtfsf(Xenon::CpuState *xState);
	static void OpMtfsfi(Xenon::CpuState *xState);
	static void OpMtmsr(Xenon::CpuState *xState);
	static void OpMtmsrd(Xenon::CpuState *xState);
	static void OpMtspr(Xenon::CpuState *xState);
	static void OpMtsr(Xenon::CpuState *xState);
	static void OpMtsrin(Xenon::CpuState *xState);
	static void OpMulthd(Xenon::CpuState *xState);
	static void OpMulthdu(Xenon::CpuState *xState);
	static void OpMulhw(Xenon::CpuState *xState);
	static void OpMulhwu(Xenon::CpuState *xState);
	static void OpMulld(Xenon::CpuState *xState);
	static void OpMulldo(Xenon::CpuState *xState);
	static void OpMulli(Xenon::CpuState *xState);
	static void OpMullw(Xenon::CpuState *xState);
	static void OpMullwo(Xenon::CpuState *xState);
	static void OpNand(Xenon::CpuState *xState);
	static void OpNeg(Xenon::CpuState *xState);
	static void OpNego(Xenon::CpuState *xState);
	static void OpNor(Xenon::CpuState *xState);
	static void OpOr(Xenon::CpuState *xState);
	static void OpOrc(Xenon::CpuState *xState);
	static void OpOri(Xenon::CpuState *xState);
	static void OpOris(Xenon::CpuState *xState);
	static void OpRfid(Xenon::CpuState *xState);
	static void OpRldcl(Xenon::CpuState *xState);
	static void OpRldcr(Xenon::CpuState *xState);
	static void OpRldic(Xenon::CpuState *xState);
	static void OpRldicl(Xenon::CpuState *xState);
	static void OpRldicr(Xenon::CpuState *xState);
	static void OpRldimi(Xenon::CpuState *xState);
	static void OpRlwimi(Xenon::CpuState *xState);
	static void OpRlwinm(Xenon::CpuState *xState);
	static void OpRlwnm(Xenon::CpuState *xState);
	static void OpSc(Xenon::CpuState *xState);
	static void OpSlbia(Xenon::CpuState *xState);
	static void OpSlbie(Xenon::CpuState *xState);
	static void OpSlbmfee(Xenon::CpuState *xState);
	static void OpSlbmfev(Xenon::CpuState *xState);
	static void OpSlbmte(Xenon::CpuState *xState);
	static void OpSld(Xenon::CpuState *xState);
	static void OpSlw(Xenon::CpuState *xState);
	static void OpSrad(Xenon::CpuState *xState);
	static void OpSradi(Xenon::CpuState *xState);
	static void OpSraw(Xenon::CpuState *xState);
	static void OpSrawi(Xenon::CpuState *xState);
	static void OpSrd(Xenon::CpuState *xState);
	static void OpSrw(Xenon::CpuState *xState);
	static void OpStb(Xenon::CpuState *xState);
	static void OpStbu(Xenon::CpuState *xState);
	static void OpStbux(Xenon::CpuState *xState);
	static void OpStbx(Xenon::CpuState *xState);
	static void OpStd(Xenon::CpuState *xState);
	static void OpStdcx(Xenon::CpuState *xState);
	static void OpStdu(Xenon::CpuState *xState);
	static void OpStdux(Xenon::CpuState *xState);
	static void OpStdx(Xenon::CpuState *xState);
	static void OpStfd(Xenon::CpuState *xState);
	static void OpStfdu(Xenon::CpuState *xState);
	static void OpStfdux(Xenon::CpuState *xState);
	static void OpStfdx(Xenon::CpuState *xState);
	static void OpStfiwx(Xenon::CpuState *xState);
	static void OpStfs(Xenon::CpuState *xState);
	static void OpStfsu(Xenon::CpuState *xState);
	static void OpStfsux(Xenon::CpuState *xState);
	static void OpStfsx(Xenon::CpuState *xState);
	static void OpSth(Xenon::CpuState *xState);
	static void OpSthbrx(Xenon::CpuState *xState);
	static void OpSthu(Xenon::CpuState *xState);
	static void OpSthux(Xenon::CpuState *xState);
	static void OpSthx(Xenon::CpuState *xState);
	static void OpStmw(Xenon::CpuState *xState);
	static void OpStswi(Xenon::CpuState *xState);
	static void OpStswx(Xenon::CpuState *xState);
	static void OpStw(Xenon::CpuState *xState);
	static void OpStwbrx(Xenon::CpuState *xState);
	static void OpStwcx(Xenon::CpuState *xState);
	static void OpStwu(Xenon::CpuState *xState);
	static void OpStwux(Xenon::CpuState *xState);
	static void OpStwx(Xenon::CpuState *xState);
	static void OpSubf(Xenon::CpuState *xState);
	static void OpSubfo(Xenon::CpuState *xState);
	static void OpSubfc(Xenon::CpuState *xState);
	static void OpSubfco(Xenon::CpuState *xState);
	static void OpSubfe(Xenon::CpuState *xState);
	static void OpSubfeo(Xenon::CpuState *xState);
	static void OpSubfic(Xenon::CpuState *xState);
	static void OpSubfme(Xenon::CpuState *xState);
	static void OpSubfmeo(Xenon::CpuState *xState);
	static void OpSubfze(Xenon::CpuState *xState);
	static void OpSubfzeo(Xenon::CpuState *xState);
	static void OpSync(Xenon::CpuState *xState);
	static void OpTd(Xenon::CpuState *xState);
	static void OpTdi(Xenon::CpuState *xState);
	static void OpTlbia(Xenon::CpuState *xState);
	static void OpTlbie(Xenon::CpuState *xState);
	static void OpTlbiel(Xenon::CpuState *xState);
	static void OpTlbsync(Xenon::CpuState *xState);
	static void OpTw(Xenon::CpuState *xState);
	static void OpTwi(Xenon::CpuState *xState);
	static void OpXor(Xenon::CpuState *xState);
	static void OpXori(Xenon::CpuState *xState);
	static void OpXoris(Xenon::CpuState *xState);
};

/* This was from Dolphin. */
inline s32 SignExt26(u32 x)
{
	struct { signed x : 26; } sext;
	sext.x = x;
	return sext.x;
}

/* Remember that all addresses are truncated to 32 bit */

inline u32 GetEA(const Xenon::CpuState *xState)
{
	return (u32) (INSTR.RA ? (rGPR[INSTR.RA] + INSTR.SIMM_16) : (u64) INSTR.SIMM_16);
}

inline u32 GetEAu(const Xenon::CpuState *xState)
{
	return (u32) (rGPR[INSTR.RA] + INSTR.SIMM_16);
}

inline u32 GetEAX(const Xenon::CpuState *xState)
{
	return (u32) (INSTR.RA ? (rGPR[INSTR.RA] + rGPR[INSTR.RB]) : rGPR[INSTR.RB]);
}

inline u32 GetEAXu(const Xenon::CpuState *xState)
{
	return (u32) (rGPR[INSTR.RA] + rGPR[INSTR.RB]);
}

union RegXER
{
	struct
	{
		u32 BYTE_COUNT	: 7;
		u32				: 22;
		u32 CA			: 1;
		u32 OV			: 1;
		u32 SO			: 1;
	};
	u32 Hex;
};

#include <vector>
extern std::vector<u32> Branches;