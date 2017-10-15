#include <XenonParser.h>
#include <iostream>
#include <stdint.h>

using namespace std;

#define SHOW_SIMPLE

#define DOP (((p->Instruction) >> 21) & 0x1F)
#define SOP DOP
#define AOP (((p->Instruction) >> 16) & 0x1F)
#define BOP (((p->Instruction) >> 11) & 0x1F)
#define COP (((p->Instruction) >> 6) & 0x1F)
#define UIMM ((uint16_t) (p->Instruction) & 0xFFFF)
#define SIMM ((int16_t) (UIMM))
#define BOOP DOP
#define BIOP AOP
#define BHOP ((p->Instruction >> 11) & 3)
#define LOP ((p->Instruction >> 21) & 1)
#define CRFD ((p->Instruction >> 23) & 7)
#define CRFS ((p->Instruction >> 18) & 7)

struct ParseStatus
{
	uint Instruction;
} ps;

XenonParser xp(&ps);

void OpAdd(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

	cout << "add";
	if (p->Instruction & 1)
		cout << ".";

	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpAddo(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

	cout << "addo";
	if (p->Instruction & 1)
		cout << ".";

	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpAddc(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

	cout << "addc";
	if (p->Instruction & 1)
		cout << ".";

	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpAddco(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

	cout << "addco";
	if (p->Instruction & 1)
		cout << ".";

	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpAdde(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

	cout << "adde";
	if (p->Instruction & 1)
		cout << ".";

	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpAddeo(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

	cout << "addeo";
	if (p->Instruction & 1)
		cout << ".";

	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpAddi(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

#ifndef SHOW_SIMPLE
	cout << "addi\t\tr" << DOP << ", ";
	if (AOP)
		cout << "r" << AOP;
	else
		cout << "0";
	cout << ", " << SIMM << endl;
#else
	if (!AOP)
		cout << "li\t\tr" << DOP << ", " << SIMM;
	else
	{
		if (SIMM >= 0)
			cout << "addi\t\tr" << DOP << ", r" << AOP << ", " << SIMM;
		else
			cout << "subi\t\tr" << DOP << ", r" << AOP << ", " << -SIMM;
	}
	cout << endl;
#endif
}

void OpAddic(void *ps)
{
		ParseStatus *p = (ParseStatus *) ps;

#ifndef SHOW_SIMPLE
	cout << "addic\t\tr" << DOP << ", r" << AOP << ", " << SIMM;
#else
	if (SIMM >= 0)
		cout << "addic\t\tr" << DOP << ", r" << AOP << ", " << SIMM;
	else
		cout << "subic\t\tr" << DOP << ", r" << AOP << ", " << -SIMM;
	cout << endl;
#endif
}

void OpAddicUp(void *ps)
{
		ParseStatus *p = (ParseStatus *) ps;

#ifndef SHOW_SIMPLE
	cout << "addic.\t\tr" << DOP << ", r" << AOP << ", " << SIMM;
#else
	if (SIMM >= 0)
		cout << "addic.\t\tr" << DOP << ", r" << AOP << ", " << SIMM;
	else
		cout << "subic.\t\tr" << DOP << ", r" << AOP << ", " << -SIMM;
	cout << endl;
#endif
}

void OpAddis(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	int32_t eSIMM = SIMM << 16;
#ifndef SHOW_SIMPLE
	cout << "addis\t\tr" << DOP << ", ";
	if (AOP)
		cout << "r" << AOP;
	else
		cout << "0";
	cout << ", " << eSIMM << endl;
#else
	if (!AOP)
		cout << "lis\t\tr" << DOP << ", " << eSIMM;
	else
	{
		if (SIMM >= 0)
			cout << "addis\t\tr" << DOP << ", r" << AOP << ", " << eSIMM;
		else
			cout << "subis\t\tr" << DOP << ", r" << AOP << ", " << -eSIMM;
	}
	cout << endl;
#endif
}

void OpAddme(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

	cout << "addme";
	if (p->Instruction & 1)
		cout << ".";

	cout << "\t\tr" << DOP << ", r" << AOP << endl;
}

void OpAddmeo(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

	cout << "addmeo";
	if (p->Instruction & 1)
		cout << ".";

	cout << "\t\tr" << DOP << ", r" << AOP << endl;
}

void OpAddze(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

	cout << "addze";
	if (p->Instruction & 1)
		cout << ".";

	cout << "\t\tr" << DOP << ", r" << AOP << endl;
}

void OpAddzeo(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

	cout << "addzeo";
	if (p->Instruction & 1)
		cout << ".";

	cout << "\t\tr" << DOP << ", r" << AOP << endl;
}

void OpAnd(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

	cout << "and";
	if (p->Instruction & 1)
		cout << ".";

	cout << "\t\tr" << AOP << ", r" << SOP << ", r" << BOP << endl;
}

void OpAndc(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

	cout << "andc";
	if (p->Instruction & 1)
		cout << ".";

	cout << "\t\tr" << AOP << ", r" << SOP << ", r" << BOP << endl;
}

void OpAndiUp(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

	cout << "andi\t\tr" << AOP << ", r" << SOP << ", " << UIMM << endl;
}

void OpAndisUp(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	uint32_t eUIMM = UIMM << 16;

	cout << "andis\t\tr" << AOP << ", r" << SOP << ", " << eUIMM << endl;
}

void OpB(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	uint32_t li = (p->Instruction) & 0xFFFFFC;

	/* Sign extend 24bit to 64bit */
	signed char upperLi = (li >> 16) & 0xFF;
	int64_t realLi = upperLi;
	realLi <<= 16;
	realLi |= li & 0xFFFF;

	cout << "b";
	if (p->Instruction & 1)
		cout << "l";
	if (p->Instruction & 2)
		cout << "a";

	cout << "\t\t";
	if (p->Instruction & 2)
		cout << (uint32_t) realLi;
	else
	{
		cout << "CIA ";
		if (realLi > 0)
			cout << "+ " << realLi;
		else
			cout << "- " << -realLi;
	}
	
	cout << endl;
}

void OpBc(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	uint32_t li = p->Instruction & 0xFFFC;

	/* Sign extend 24bit to 64bit */
	int32_t realLi = li;

	cout << "bc";
	if (p->Instruction & 1)
		cout << "l";
	if (p->Instruction & 2)
		cout << "a";

	cout << "\t\t";
	cout << BOOP << ", " << BIOP << ", ";
	if (p->Instruction & 2)
		cout << (uint32_t) realLi;
	else
	{
		cout << "CIA ";
		if (realLi > 0)
			cout << "+ " << realLi;
		else
			cout << "- " << -realLi;
	}
	cout << endl;
}

void OpBcctr(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	uint32_t li = p->Instruction & 0xFFFC;

	/* Sign extend 24bit to 64bit */
	int32_t realLi = li;

	cout << "bcctr";
	if (p->Instruction & 1)
		cout << "l";

	cout << "\t\t";
	cout << BOOP << ", " << BIOP << ", " << BHOP << ", " << realLi << endl;
}

void OpBclr(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	uint32_t li = p->Instruction & 0xFFFC;

	/* Sign extend 24bit to 64bit */
	int32_t realLi = li;

	cout << "bclr";
	if (p->Instruction & 1)
		cout << "l";

	cout << "\t\t";
	cout << BOOP << ", " << BIOP << ", " << BHOP << ", " << realLi << endl;
}

void OpCmp(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

#ifndef SHOW_SIMPLE
	cout << "cmp\t\tcrf" << CRFD << ", " << LOP << ", r" << AOP << ", r" << BOP << endl;
#else
	cout << "cmp";
	if (!LOP)
		cout << "w";
	else
		cout << "d";
	cout << "\t\t";
	if (CRFD != 0)
		cout << "crf" << CRFD << ", ";
	cout << "r" << AOP << ", r" << BOP << endl;
#endif
}

void OpCmpi(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

#ifndef SHOW_SIMPLE
	cout << "cmpi\t\tcrf" << CRFD << ", " << LOP << ", r" << AOP << ", " << SIMM << endl;
#else
	cout << "cmp";
	if (!LOP)
		cout << "w";
	else
		cout << "d";
	cout << "i\t\t";
	if (CRFD != 0)
		cout << "crf" << CRFD << ", ";
	cout << "r" << AOP << ", " << SIMM << endl;
#endif
}

void OpCmpl(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

#ifndef SHOW_SIMPLE
	cout << "cmp\t\tcrf" << CRFD << ", " << LOP << ", r" << AOP << ", r" << BOP << endl;
#else
	cout << "cmpl";
	if (!LOP)
		cout << "w";
	else
		cout << "d";
	cout << "\t\t";
	if (CRFD != 0)
		cout << "crf" << CRFD << ", ";
	cout << "r" << AOP << ", r" << BOP << endl;
#endif
}

void OpCmpli(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;

#ifndef SHOW_SIMPLE
	cout << "cmpli\t\tcrf" << CRFD << ", " << LOP << ", r" << AOP << ", " << UIMM << endl;
#else
	cout << "cmpl";
	if (!LOP)
		cout << "w";
	else
		cout << "d";
	cout << "i\t\t";
	if (CRFD != 0)
		cout << "crf" << CRFD << ", ";
	cout << "r" << AOP << ", " << UIMM << endl;
#endif
}

void OpCntlzd(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "cntlzd";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", " << SOP << endl;
}

void OpCntlzw(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "cntlzw";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", " << SOP << endl;
}

void OpCrand(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "crand\t\tcrb" << CRFD << ", crb" << AOP << ", crb" << BOP << endl;
}

void OpCrandc(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "crandc\t\tcrb" << CRFD << ", crb" << AOP << ", crb" << BOP << endl;
}

void OpCreqv(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "creqv\t\tcrb" << CRFD << ", crb" << AOP << ", crb" << BOP << endl;
}

void OpCrnand(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "crnand\t\tcrb" << CRFD << ", crb" << AOP << ", crb" << BOP << endl;
}

void OpCrnor(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "crnor\t\tcrb" << CRFD << ", crb" << AOP << ", crb" << BOP << endl;
}

void OpCror(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "cror\t\tcrb" << CRFD << ", crb" << AOP << ", crb" << BOP << endl;
}

void OpCrorc(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "crorc\t\tcrb" << CRFD << ", crb" << AOP << ", crb" << BOP << endl;
}

void OpCrxor(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "crxor\t\tcrb" << CRFD << ", crb" << AOP << ", crb" << BOP << endl;
}

void OpDcbf(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "dcbf\t\tr" << AOP << ", r" << BOP << endl;
}

void OpDcbst(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "dcbst\t\tr" << AOP << ", r" << BOP << endl;
}

void OpDcbt(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "dcbt\t\tr" << AOP << ", r" << BOP << ", " << (DOP & 3) << endl;
}

void OpDcbtst(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "dcbtst\t\tr" << AOP << ", r" << BOP << endl;
}

void OpDcbz(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "dcbz\t\tr" << AOP << ", r" << BOP << endl;
}

void OpDivd(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "divd";

	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpDivdo(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "divdo";

	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpDivdu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "divdu";

	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpDivduo(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "divduo";

	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpDivw(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "divw";

	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpDivwo(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "divwo";

	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpDivwu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "divwu";

	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpDivwuo(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "divwuo";

	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpEciwx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "eciwx\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpEcowx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "ecowx\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpEieio(void *)
{
	cout << "eieio" << endl;
}

void OpEqv(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "eqv";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", r" << SOP << ", r" << BOP << endl;
}

void OpExtsb(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "extsb";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", r" << SOP << endl;
}

void OpExtsh(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "extsh";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", r" << SOP << endl;
}

void OpExtsw(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "extsw";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", r" << SOP << endl;
}

void OpFabs(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fabs";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << BOP << endl;
}

void OpFadd(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fadd";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << BOP << endl;
}

void OpFadds(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fadds";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << BOP << endl;
}

void OpFcfid(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fcfid";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << BOP << endl;
}

void OpFcmpo(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fcmpo\t\tcrf" << CRFD << ", fr" << AOP << ", fr" << BOP << endl;
}

void OpFcmpu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fcmpu\t\tcrf" << CRFD << ", fr" << AOP << ", fr" << BOP << endl;
}

void OpFctid(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fctid";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << BOP << endl;
}

void OpFctidz(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fctidz";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << BOP << endl;
}

void OpFctiw(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fctiw";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << BOP << endl;
}

void OpFctiwz(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fctiwz";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << BOP << endl;
}

void OpFdiv(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fdiv";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << BOP << endl;
}

void OpFdivs(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fdivs";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << BOP << endl;
}

void OpFmadd(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fmadd";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << COP << ", fr" << BOP << endl;
}

void OpFmadds(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fmadds";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << COP << ", fr" << BOP << endl;
}

void OpFmr(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fmr";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << BOP << endl;
}

void OpFmsub(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fmsub";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << COP << ", fr" << BOP << endl;
}

void OpFmsubs(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fmsubs";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << COP << ", fr" << BOP << endl;
}

void OpFmul(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fmul";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << COP << endl;
}

void OpFmuls(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fmuls";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << COP << endl;
}

void OpFnabs(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fnabs";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << BOP << endl;
}

void OpFneg(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fneg";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << BOP << endl;
}

void OpFnmadd(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fnmadd";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << COP << ", fr" << BOP << endl;
}

void OpFnmadds(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fnmadds";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << COP << ", fr" << BOP << endl;
}

void OpFnmsub(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fnmsub";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << COP << ", fr" << BOP << endl;
}

void OpFnmsubs(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fnmsubs";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << COP << ", fr" << BOP << endl;
}

void OpFres(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fres";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << BOP << endl;
}

void OpFrsp(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "frsp";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << BOP << endl;
}

void OpFrsqrte(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "frsqrte";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << BOP << endl;
}

void OpFsel(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fsel";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << COP << ", fr" << BOP << endl;
}

void OpFsqrt(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fsqrt";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << BOP << endl;
}

void OpFsqrts(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fsqrts";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << BOP << endl;
}

void OpFsub(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fsub";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << BOP << endl;
}

void OpFsubs(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "fsubs";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << ", fr" << AOP << ", fr" << BOP << endl;
}

void OpIcbi(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "icbi\t\tr" << AOP << ", r" << BOP << endl;
}

void OpIsync(void *)
{
	cout << "isync" << endl;
}

void OpLbz(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lbz\t\tr" << DOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpLbzu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lbzu\t\tr" << DOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpLbzux(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lbzux\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLbzx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lbzx\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLd(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "ld\t\tr" << DOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpLdarx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "ldarx\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLdu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "ld\t\tr" << DOP << ", " << (SIMM & ~3) << "(r" << AOP << ")" << endl;
}

void OpLdux(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "ldux\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLdx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "ldx\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLfd(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lfd\t\tfr" << DOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpLfdu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lfdu\t\tfr" << DOP << ", " << SIMM << "(r" << AOP << ")" << endl;

}

void OpLfdux(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lfdux\t\tfr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLfdx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lfdx\t\tfr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLfs(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lfs\t\tfr" << DOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpLfsu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lfsu\t\tfr" << DOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpLfsux(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lfsux\t\tfr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLfsx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lfsx\t\tfr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLha(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lha\t\tr" << DOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpLhau(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lhau\t\tr" << DOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpLhaux(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lhaux\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLhax(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lhax\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLhbrx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lhbrx\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLhz(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lhz\t\tr" << DOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpLhzu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lhzu\t\tr" << DOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpLhzux(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lhzux\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLhzx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lhzx\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLmw(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lmw\t\tr" << DOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpLswi(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lswi\t\tr" << DOP << ", r" << AOP << ", " << BOP << endl;
}

void OpLswx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lswi\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLwa(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lwa\t\tr" << DOP << ", " << (SIMM & ~3) << "(r" << AOP << ")" << endl;
}

void OpLwarx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lswarx\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLwaux(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lwaux\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLwax(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lwax\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLwbrx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lwbrx\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLwz(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lwz\t\tr" << DOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpLwzu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lwzu\t\tr" << DOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpLwzux(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lwzux\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpLwzx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "lwzx\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpMcrf(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mcrf\t\tcrf" << CRFD << ", crf" << CRFS << endl;
}

void OpMcrfs(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mcrfs\t\tcrf" << CRFD << ", crf" << CRFS << endl;
}

void OpMfcr(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mfcr\t\tr" << DOP << endl;
}

void OpMffs(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mffs";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tfr" << DOP << endl;
}

void OpMfmsr(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mfmsr\t\tr" << DOP << endl;
}

void OpMfspr(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	uint spr = (p->Instruction >> 11) & 0x3FF;
	spr = ((spr & 0x1F) << 5) | ((spr >> 5) & 0x1F);
	cout << "mfspr\t\tr" << DOP << ", " << spr << endl;
}

void OpMfsr(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mfsr\t\tr" << DOP << ", " << AOP << endl;
}

void OpMfsrin(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mfsrin\t\tr" << DOP << ", r" << BOP << endl;
}

void OpMftb(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mftb\t\tr" << DOP << ", " << ((BOP << 5) | AOP ) << endl;
}

void OpMtcrf(void *)
{
	cout << "mtcrf\t\tSorryForParams" << endl;
}

void OpMtfsb0(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mtfsb0";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tcrb" << CRFD << endl;
}

void OpMtfsb1(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mtfsb1";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tcrb" << CRFD << endl;
}

void OpMtfsf(void *)
{
	cout << "mtfsf\t\tSorryForParams";
}

void OpMtfsfi(void *)
{
	cout << "mtfsfi\t\tSorryForParams";
}

void OpMtmsr(void *)
{
	cout << "mtmsr\t\tSorryForParams";
}

void OpMtmsrd(void *)
{
	cout << "mtfsd\t\tSorryForParams";
}

void OpMtspr(void *)
{
	cout << "mtfspr\t\tSorryForParams";
}

void OpMtsr(void *)
{
	cout << "mtsr\t\tSorryForParams";
}

void OpMtsrin(void *)
{
	cout << "mtsrin\t\tSorryForParams";
}

void OpMulthd(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mulhd";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpMulthdu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mulhdu";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpMulhw(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mulhw";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpMulhwu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mulhwu";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpMulld(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mulld";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;

}

void OpMulldo(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mulldo";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpMulli(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mulli";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", " << SIMM << endl;
}

void OpMullw(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mullw";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", " << SIMM << endl;
}

void OpMullwo(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "mullwo";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", " << SIMM << endl;
}

void OpNand(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "nand";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", r" << SOP << ", r" << BOP << endl;
}

void OpNeg(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "neg";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << endl;
}

void OpNego(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "nego";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << endl;
}

void OpNor(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "nor";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", r" << SOP << ", r" << BOP << endl;
}

void OpOr(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
#ifndef SHOW_SIMPLE
	cout << "or";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", r" << SOP << ", r" << BOP << endl;
#endif
	if (BOP == SOP)
	{
		cout << "mr";
		if (p->Instruction & 1)
			cout << ".";
		cout << "\t\tr" << AOP << ", r" << SOP << endl;
	}
	else
	{
		cout << "or";
		if (p->Instruction & 1)
			cout << ".";
		cout << "\t\tr" << AOP << ", r" << SOP << ", r" << BOP << endl;
	}
}

void OpOrc(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "orc";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", r" << SOP << ", r" << BOP << endl;
}

void OpOri(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
#ifndef SHOW_SIMPLE
	cout << "ori\t\tr" << AOP << ", r" << SOP << ", " << UIMM << endl;
#else
	if (AOP == SOP == UIMM == 0)
		cout << "nop" << endl;
	else
		cout << "ori\t\tr" << AOP << ", r" << SOP << ", " << UIMM << endl;
#endif
}

void OpOris(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	uint32_t eUIMM = UIMM;
	eUIMM <<= 16;
	cout << "ori\t\tr" << AOP << ", r" << SOP << ", " << eUIMM << endl;
}

void OpRfid(void *)
{
	cout << "rfid" << endl;
}

void OpRldcl(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "rldcl";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tSorryForParams";
}

void OpRldcr(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "rldcr";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tSorryForParams";
}

void OpRldic(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "rldic";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tSorryForParams";
}

void OpRldicl(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "rldicl";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tSorryForParams";
}

void OpRldicr(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "rldicr";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tSorryForParams";
}

void OpRldimi(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "rldimi";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tSorryForParams";
}

void OpRlwimi(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "rlwimi";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tSorryForParams";
}

void OpRlwinm(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "rlwinm";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tSorryForParams";
}

void OpRlwnm(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "rlwnm";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tSorryForParams";
}

void OpSc(void *)
{
	cout << "sc" << endl;
}

void OpSlbia(void *)
{
	cout << "slbia" << endl;
}

void OpSlbie(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "slbie\t\tr" << BOP << endl;
}

void OpSlbmfee(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "slbmfee\t\tr" << DOP << ", r" << BOP << endl;
}

void OpSlbmfev(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "slbmfev\t\tr" << DOP << ", r" << BOP << endl;
}

void OpSlbmte(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "slbmte\t\tr" << SOP << ", r" << BOP << endl;
}

void OpSld(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "sld";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", r" << SOP << ", r" << BOP << endl;
}

void OpSlw(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "slw";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", r" << SOP << ", r" << BOP << endl;
}

void OpSrad(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "srad";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", r" << SOP << ", r" << BOP << endl;
}

void OpSradi(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "sradi";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tSorryForOperands";
}

void OpSraw(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "sraw";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", r" << SOP << ", r" << BOP << endl;
}

void OpSrawi(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "srawi";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tSorryForOperands";
}

void OpSrd(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "srd";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", r" << SOP << ", r" << BOP << endl;
}

void OpSrw(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "srw";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", r" << SOP << ", r" << BOP << endl;
}

void OpStb(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stb\t\tr" << SOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpStbu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stbu\t\tr" << SOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpStbux(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stbux\t\tr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpStbx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stbx\t\tr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpStd(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "std\t\tr" << SOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpStdcx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stdcx.\t\tr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpStdu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stdu\t\tr" << SOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpStdux(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stdux\t\tr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpStdx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stdx\t\tr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpStfd(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stfd\t\tfr" << SOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpStfdu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stfdu\t\tfr" << SOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpStfdux(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stfdux\t\tfr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpStfdx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stfdx\t\tfr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpStfiwx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stfiwx\t\tfr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpStfs(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stfs\t\tfr" << SOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpStfsu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stfsu\t\tfr" << SOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpStfsux(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stfsux\t\tfr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpStfsx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stfsx\t\tfr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpSth(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "sth\t\tr" << SOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpSthbrx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "sthbrx\t\tr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpSthu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "sthu\t\tr" << SOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpSthux(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "sthux\t\tr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpSthx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "sthx\t\tr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpStmw(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stmw\t\tr" << SOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpStswi(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stswi\t\tr" << SOP << ", r" << AOP << ", " << BOP << endl;
}

void OpStswx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stswx\t\tr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpStw(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stw\t\tr" << SOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpStwbrx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stwbrx\t\tr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpStwcx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stwcx.\t\tr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpStwu(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stwu\t\tr" << SOP << ", " << SIMM << "(r" << AOP << ")" << endl;
}

void OpStwux(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stwux\t\tr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpStwx(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "stwx\t\tr" << SOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpSubf(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "subf";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpSubfo(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "subfo";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpSubfc(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "subfc";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpSubfco(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "subfco";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpSubfe(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "subfe";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpSubfeo(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "subfeo";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpSubfic(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "subfic\t\tr" << DOP << ", r" << AOP << ", " << SIMM << endl;
}

void OpSubfme(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "subfme";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << endl;
}

void OpSubfmeo(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "subfmeo";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << endl;
}

void OpSubfze(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "subfze";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << endl;
}

void OpSubfzeo(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "subfzeo";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << DOP << ", r" << AOP << endl;
}

void OpSync(void *)
{
	cout << "sync" << endl;
}

void OpTd(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "td\t\t" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpTdi(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "tdi\t\t" << DOP << ", r" << AOP << ", " << SIMM << endl;
}

void OpTlbia(void *)
{
	cout << "tlbia" << endl;
}

void OpTlbie(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "tlbie\t\tr" << BOP << ", " << ((p->Instruction >> 21) & 1) << endl;
}

void OpTlbiel(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "tlbiel\t\tr" << BOP << ", " << ((p->Instruction >> 21) & 1) << endl;
}

void OpTlbsync(void *)
{
	cout << "tlbsync" << endl;
}

void OpTw(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "tw\t\t" << DOP << ", r" << AOP << ", r" << BOP << endl;
}

void OpTwi(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "twi\t\t" << DOP << ", r" << AOP << ", " << SIMM << endl;
}

void OpXor(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "xor";
	if (p->Instruction & 1)
		cout << ".";
	cout << "\t\tr" << AOP << ", r" << SOP << ", r" << BOP << endl;
}

void OpXori(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	cout << "xori\t\tr" << AOP << ", r" << SOP << ", " << UIMM << endl;
}

void OpXoris(void *ps)
{
	ParseStatus *p = (ParseStatus *) ps;
	uint32_t eUIMM = UIMM;
	eUIMM <<= 16;
	cout << "xoris\t\tr" << AOP << ", r" << SOP << ", " << eUIMM << endl;
}

void InitParser()
{
	xp.Register(Mnemonics::PPC_OP_ADD, OpAdd);
	xp.Register(Mnemonics::PPC_OP_ADDO, OpAddo);
	xp.Register(Mnemonics::PPC_OP_ADDC, OpAddc);
	xp.Register(Mnemonics::PPC_OP_ADDCO, OpAddco);
	xp.Register(Mnemonics::PPC_OP_ADDE, OpAdde);
	xp.Register(Mnemonics::PPC_OP_ADDEO, OpAddeo);
	xp.Register(Mnemonics::PPC_OP_ADDI, OpAddi);
	xp.Register(Mnemonics::PPC_OP_ADDIC, OpAddic);
	xp.Register(Mnemonics::PPC_OP_ADDIC_UP, OpAddicUp);
	xp.Register(Mnemonics::PPC_OP_ADDIS, OpAddis);
	xp.Register(Mnemonics::PPC_OP_ADDME, OpAddme);
	xp.Register(Mnemonics::PPC_OP_ADDMEO, OpAddmeo);
	xp.Register(Mnemonics::PPC_OP_ADDZE, OpAddze);
	xp.Register(Mnemonics::PPC_OP_ADDZEO, OpAddzeo);
	xp.Register(Mnemonics::PPC_OP_AND, OpAnd);
	xp.Register(Mnemonics::PPC_OP_ANDC, OpAndc);
	xp.Register(Mnemonics::PPC_OP_ANDI_UP, OpAndiUp);
	xp.Register(Mnemonics::PPC_OP_ANDIS_UP, OpAndisUp);
	xp.Register(Mnemonics::PPC_OP_B, OpB);
	xp.Register(Mnemonics::PPC_OP_BC, OpBc);
	xp.Register(Mnemonics::PPC_OP_BCCTR, OpBcctr);
	xp.Register(Mnemonics::PPC_OP_BCLR, OpBclr);
	xp.Register(Mnemonics::PPC_OP_CMP, OpCmp);
	xp.Register(Mnemonics::PPC_OP_CMPI, OpCmpi);
	xp.Register(Mnemonics::PPC_OP_CMPL, OpCmpl);
	xp.Register(Mnemonics::PPC_OP_CMPLI, OpCmpli);
	xp.Register(Mnemonics::PPC_OP_CNTLZD, OpCntlzd);
	xp.Register(Mnemonics::PPC_OP_CNTLZW, OpCntlzw);
	xp.Register(Mnemonics::PPC_OP_CRAND, OpCrand);
	xp.Register(Mnemonics::PPC_OP_CRANDC, OpCrandc);
	xp.Register(Mnemonics::PPC_OP_CREQV, OpCreqv);
	xp.Register(Mnemonics::PPC_OP_CRNAND, OpCrnand);
	xp.Register(Mnemonics::PPC_OP_CRNOR, OpCrnor);
	xp.Register(Mnemonics::PPC_OP_CROR, OpCror);
	xp.Register(Mnemonics::PPC_OP_CRORC, OpCrorc);
	xp.Register(Mnemonics::PPC_OP_CRXOR, OpCrxor);
	xp.Register(Mnemonics::PPC_OP_DCBF, OpDcbf);
	xp.Register(Mnemonics::PPC_OP_DCBST, OpDcbst);
	xp.Register(Mnemonics::PPC_OP_DCBT, OpDcbt);
	xp.Register(Mnemonics::PPC_OP_DCBTST, OpDcbtst);
	xp.Register(Mnemonics::PPC_OP_DCBZ, OpDcbz);
	xp.Register(Mnemonics::PPC_OP_DIVD, OpDivd);
	xp.Register(Mnemonics::PPC_OP_DIVDO, OpDivdo);
	xp.Register(Mnemonics::PPC_OP_DIVDU, OpDivdu);
	xp.Register(Mnemonics::PPC_OP_DIVDUO, OpDivduo);
	xp.Register(Mnemonics::PPC_OP_DIVW, OpDivw);
	xp.Register(Mnemonics::PPC_OP_DIVWO, OpDivwo);
	xp.Register(Mnemonics::PPC_OP_DIVWU, OpDivwu);
	xp.Register(Mnemonics::PPC_OP_DIVWUO, OpDivwuo);
	xp.Register(Mnemonics::PPC_OP_ECIWX, OpEciwx);
	xp.Register(Mnemonics::PPC_OP_ECOWX, OpEcowx);
	xp.Register(Mnemonics::PPC_OP_EIEIO, OpEieio);
	xp.Register(Mnemonics::PPC_OP_EQV, OpEqv);
	xp.Register(Mnemonics::PPC_OP_EXTSB, OpExtsb);
	xp.Register(Mnemonics::PPC_OP_EXTSH, OpExtsh);
	xp.Register(Mnemonics::PPC_OP_EXTSW, OpExtsw);
	xp.Register(Mnemonics::PPC_OP_FABS, OpFabs);
	xp.Register(Mnemonics::PPC_OP_FADD, OpFadd);
	xp.Register(Mnemonics::PPC_OP_FADDS, OpFadds);
	xp.Register(Mnemonics::PPC_OP_FCFID, OpFcfid);
	xp.Register(Mnemonics::PPC_OP_FCMPO, OpFcmpo);
	xp.Register(Mnemonics::PPC_OP_FCMPU, OpFcmpu);
	xp.Register(Mnemonics::PPC_OP_FCTID, OpFctid);
	xp.Register(Mnemonics::PPC_OP_FCTIDZ, OpFctidz);
	xp.Register(Mnemonics::PPC_OP_FCTIW, OpFctiw);
	xp.Register(Mnemonics::PPC_OP_FCTIWZ, OpFctiwz);
	xp.Register(Mnemonics::PPC_OP_FDIV, OpFdiv);
	xp.Register(Mnemonics::PPC_OP_FDIVS, OpFdivs);
	xp.Register(Mnemonics::PPC_OP_FMADD, OpFmadd);
	xp.Register(Mnemonics::PPC_OP_FMADDS, OpFmadds);
	xp.Register(Mnemonics::PPC_OP_FMR, OpFmr);
	xp.Register(Mnemonics::PPC_OP_FMSUB, OpFmsub);
	xp.Register(Mnemonics::PPC_OP_FMSUBS, OpFmsubs);
	xp.Register(Mnemonics::PPC_OP_FMUL, OpFmul);
	xp.Register(Mnemonics::PPC_OP_FMULS, OpFmuls);
	xp.Register(Mnemonics::PPC_OP_FNABS, OpFnabs);
	xp.Register(Mnemonics::PPC_OP_FNEG, OpFneg);
	xp.Register(Mnemonics::PPC_OP_FNMADD, OpFnmadd);
	xp.Register(Mnemonics::PPC_OP_FNMADDS, OpFnmadds);
	xp.Register(Mnemonics::PPC_OP_FNMSUB, OpFnmsub);
	xp.Register(Mnemonics::PPC_OP_FNMSUBS, OpFnmsubs);
	xp.Register(Mnemonics::PPC_OP_FRES, OpFres);
	xp.Register(Mnemonics::PPC_OP_FRSP, OpFrsp);
	xp.Register(Mnemonics::PPC_OP_FRSQRTE, OpFrsqrte);
	xp.Register(Mnemonics::PPC_OP_FSEL, OpFsel);
	xp.Register(Mnemonics::PPC_OP_FSQRT, OpFsqrt);
	xp.Register(Mnemonics::PPC_OP_FSQRTS, OpFsqrts);
	xp.Register(Mnemonics::PPC_OP_FSUB, OpFsub);
	xp.Register(Mnemonics::PPC_OP_FSUBS, OpFsubs);
	xp.Register(Mnemonics::PPC_OP_ICBI, OpIcbi);
	xp.Register(Mnemonics::PPC_OP_ISYNC, OpIsync);
	xp.Register(Mnemonics::PPC_OP_LBZ, OpLbz);
	xp.Register(Mnemonics::PPC_OP_LBZU, OpLbzu);
	xp.Register(Mnemonics::PPC_OP_LBZUX, OpLbzux);
	xp.Register(Mnemonics::PPC_OP_LBZX, OpLbzx);
	xp.Register(Mnemonics::PPC_OP_LD, OpLd);
	xp.Register(Mnemonics::PPC_OP_LDARX, OpLdarx);
	xp.Register(Mnemonics::PPC_OP_LDU, OpLdu);
	xp.Register(Mnemonics::PPC_OP_LDUX, OpLdux);
	xp.Register(Mnemonics::PPC_OP_LDX, OpLdx);
	xp.Register(Mnemonics::PPC_OP_LFD, OpLfd);
	xp.Register(Mnemonics::PPC_OP_LFDU, OpLfdu);
	xp.Register(Mnemonics::PPC_OP_LFDUX, OpLfdux);
	xp.Register(Mnemonics::PPC_OP_LFDX, OpLfdx);
	xp.Register(Mnemonics::PPC_OP_LFS, OpLfs);
	xp.Register(Mnemonics::PPC_OP_LFSU, OpLfsu);
	xp.Register(Mnemonics::PPC_OP_LFSUX, OpLfsux);
	xp.Register(Mnemonics::PPC_OP_LFSX, OpLfsx);
	xp.Register(Mnemonics::PPC_OP_LHA, OpLha);
	xp.Register(Mnemonics::PPC_OP_LHAU, OpLhau);
	xp.Register(Mnemonics::PPC_OP_LHAUX, OpLhaux);
	xp.Register(Mnemonics::PPC_OP_LHAX, OpLhax);
	xp.Register(Mnemonics::PPC_OP_LHBRX, OpLhbrx);
	xp.Register(Mnemonics::PPC_OP_LHZ, OpLhz);
	xp.Register(Mnemonics::PPC_OP_LHZU, OpLhzu);
	xp.Register(Mnemonics::PPC_OP_LHZUX, OpLhzux);
	xp.Register(Mnemonics::PPC_OP_LHZX, OpLhzx);
	xp.Register(Mnemonics::PPC_OP_LMW, OpLmw);
	xp.Register(Mnemonics::PPC_OP_LSWI, OpLswi);
	xp.Register(Mnemonics::PPC_OP_LSWX, OpLswx);
	xp.Register(Mnemonics::PPC_OP_LWA, OpLwa);
	xp.Register(Mnemonics::PPC_OP_LWARX, OpLwarx);
	xp.Register(Mnemonics::PPC_OP_LWAUX, OpLwaux);
	xp.Register(Mnemonics::PPC_OP_LWAX, OpLwax);
	xp.Register(Mnemonics::PPC_OP_LWBRX, OpLwbrx);
	xp.Register(Mnemonics::PPC_OP_LWZ, OpLwz);
	xp.Register(Mnemonics::PPC_OP_LWZU, OpLwzu);
	xp.Register(Mnemonics::PPC_OP_LWZUX, OpLwzux);
	xp.Register(Mnemonics::PPC_OP_LWZX, OpLwzx);
	xp.Register(Mnemonics::PPC_OP_MCRF, OpMcrf);
	xp.Register(Mnemonics::PPC_OP_MCRFS, OpMcrfs);
	xp.Register(Mnemonics::PPC_OP_MFCR, OpMfcr);
	xp.Register(Mnemonics::PPC_OP_MFFS, OpMffs);
	xp.Register(Mnemonics::PPC_OP_MFMSR, OpMfmsr);
	xp.Register(Mnemonics::PPC_OP_MFSPR, OpMfspr);
	xp.Register(Mnemonics::PPC_OP_MFSR, OpMfsr);
	xp.Register(Mnemonics::PPC_OP_MFSRIN, OpMfsrin);
	xp.Register(Mnemonics::PPC_OP_MFTB, OpMftb);
	xp.Register(Mnemonics::PPC_OP_MTCRF, OpMtcrf);
	xp.Register(Mnemonics::PPC_OP_MTFSB0, OpMtfsb0);
	xp.Register(Mnemonics::PPC_OP_MTFSB1, OpMtfsb1);
	xp.Register(Mnemonics::PPC_OP_MTFSF, OpMtfsf);
	xp.Register(Mnemonics::PPC_OP_MTFSFI, OpMtfsfi);
	xp.Register(Mnemonics::PPC_OP_MTMSR, OpMtmsr);
	xp.Register(Mnemonics::PPC_OP_MTMSRD, OpMtmsrd);
	xp.Register(Mnemonics::PPC_OP_MTSPR, OpMtspr);
	xp.Register(Mnemonics::PPC_OP_MTSR, OpMtsr);
	xp.Register(Mnemonics::PPC_OP_MTSRIN, OpMtsrin);
	xp.Register(Mnemonics::PPC_OP_MULHD, OpMulthd);
	xp.Register(Mnemonics::PPC_OP_MULHDU, OpMulthdu);
	xp.Register(Mnemonics::PPC_OP_MULHW, OpMulhw);
	xp.Register(Mnemonics::PPC_OP_MULHWU, OpMulhwu);
	xp.Register(Mnemonics::PPC_OP_MULLD, OpMulld);
	xp.Register(Mnemonics::PPC_OP_MULLDO, OpMulldo);
	xp.Register(Mnemonics::PPC_OP_MULLI, OpMulli);
	xp.Register(Mnemonics::PPC_OP_MULLW, OpMullw);
	xp.Register(Mnemonics::PPC_OP_MULLWO, OpMullwo);
	xp.Register(Mnemonics::PPC_OP_NAND, OpNand);
	xp.Register(Mnemonics::PPC_OP_NEG, OpNeg);
	xp.Register(Mnemonics::PPC_OP_NEGO, OpNego);
	xp.Register(Mnemonics::PPC_OP_NOR, OpNor);
	xp.Register(Mnemonics::PPC_OP_OR, OpOr);
	xp.Register(Mnemonics::PPC_OP_ORC, OpOrc);
	xp.Register(Mnemonics::PPC_OP_ORI, OpOri);
	xp.Register(Mnemonics::PPC_OP_ORIS, OpOris);
	xp.Register(Mnemonics::PPC_OP_RFID, OpRfid);
	xp.Register(Mnemonics::PPC_OP_RLDCL, OpRldcl);
	xp.Register(Mnemonics::PPC_OP_RLDCR, OpRldcr);
	xp.Register(Mnemonics::PPC_OP_RLDIC, OpRldic);
	xp.Register(Mnemonics::PPC_OP_RLDICL, OpRldicl);
	xp.Register(Mnemonics::PPC_OP_RLDICR, OpRldicr);
	xp.Register(Mnemonics::PPC_OP_RLDIMI, OpRldimi);
	xp.Register(Mnemonics::PPC_OP_RLWIMI, OpRlwimi);
	xp.Register(Mnemonics::PPC_OP_RLWINM, OpRlwinm);
	xp.Register(Mnemonics::PPC_OP_RLWNM, OpRlwnm);
	xp.Register(Mnemonics::PPC_OP_SC, OpSc);
	xp.Register(Mnemonics::PPC_OP_SLBIA, OpSlbia);
	xp.Register(Mnemonics::PPC_OP_SLBIE, OpSlbie);
	xp.Register(Mnemonics::PPC_OP_SLBMFEE, OpSlbmfee);
	xp.Register(Mnemonics::PPC_OP_SLBMFEV, OpSlbmfev);
	xp.Register(Mnemonics::PPC_OP_SLBMTE, OpSlbmte);
	xp.Register(Mnemonics::PPC_OP_SLD, OpSld);
	xp.Register(Mnemonics::PPC_OP_SLW, OpSlw);
	xp.Register(Mnemonics::PPC_OP_SRAD, OpSrad);
	xp.Register(Mnemonics::PPC_OP_SRADI, OpSradi);
	xp.Register(Mnemonics::PPC_OP_SRADI1, OpSradi);		/* Ugly hack. This is actually the same instruction differing just for 1 bit. See PPC spec for details */
	xp.Register(Mnemonics::PPC_OP_SRAW, OpSraw);
	xp.Register(Mnemonics::PPC_OP_SRAWI, OpSrawi);
	xp.Register(Mnemonics::PPC_OP_SRD, OpSrd);
	xp.Register(Mnemonics::PPC_OP_SRW, OpSrw);
	xp.Register(Mnemonics::PPC_OP_STB, OpStb);
	xp.Register(Mnemonics::PPC_OP_STBU, OpStbu);
	xp.Register(Mnemonics::PPC_OP_STBUX, OpStbux);
	xp.Register(Mnemonics::PPC_OP_STBX, OpStbx);
	xp.Register(Mnemonics::PPC_OP_STD, OpStd);
	xp.Register(Mnemonics::PPC_OP_STDCX_UP, OpStdcx);
	xp.Register(Mnemonics::PPC_OP_STDU, OpStdu);
	xp.Register(Mnemonics::PPC_OP_STDUX, OpStdux);
	xp.Register(Mnemonics::PPC_OP_STDX, OpStdx);
	xp.Register(Mnemonics::PPC_OP_STFD, OpStfd);
	xp.Register(Mnemonics::PPC_OP_STFDU, OpStfdu);
	xp.Register(Mnemonics::PPC_OP_STFDUX, OpStfdux);
	xp.Register(Mnemonics::PPC_OP_STFDX, OpStfdx);
	xp.Register(Mnemonics::PPC_OP_STFIWX, OpStfiwx);
	xp.Register(Mnemonics::PPC_OP_STFS, OpStfs);
	xp.Register(Mnemonics::PPC_OP_STFSU, OpStfsu);
	xp.Register(Mnemonics::PPC_OP_STFSUX, OpStfsux);
	xp.Register(Mnemonics::PPC_OP_STFSX, OpStfsx);
	xp.Register(Mnemonics::PPC_OP_STH, OpSth);
	xp.Register(Mnemonics::PPC_OP_STHBRX, OpSthbrx);
	xp.Register(Mnemonics::PPC_OP_STHU, OpSthu);
	xp.Register(Mnemonics::PPC_OP_STHUX, OpSthux);
	xp.Register(Mnemonics::PPC_OP_STHX, OpSthx);
	xp.Register(Mnemonics::PPC_OP_STMW, OpStmw);
	xp.Register(Mnemonics::PPC_OP_STSWI, OpStswi);
	xp.Register(Mnemonics::PPC_OP_STSWX, OpStswx);
	xp.Register(Mnemonics::PPC_OP_STW, OpStw);
	xp.Register(Mnemonics::PPC_OP_STWBRX, OpStwbrx);
	xp.Register(Mnemonics::PPC_OP_STWCX_UP, OpStwcx);
	xp.Register(Mnemonics::PPC_OP_STWU, OpStwu);
	xp.Register(Mnemonics::PPC_OP_STWUX, OpStwux);
	xp.Register(Mnemonics::PPC_OP_STWX, OpStwx);
	xp.Register(Mnemonics::PPC_OP_SUBF, OpSubf);
	xp.Register(Mnemonics::PPC_OP_SUBFO, OpSubfo);
	xp.Register(Mnemonics::PPC_OP_SUBFC, OpSubfc);
	xp.Register(Mnemonics::PPC_OP_SUBFCO, OpSubfco);
	xp.Register(Mnemonics::PPC_OP_SUBFE, OpSubfe);
	xp.Register(Mnemonics::PPC_OP_SUBFEO, OpSubfeo);
	xp.Register(Mnemonics::PPC_OP_SUBFIC, OpSubfic);
	xp.Register(Mnemonics::PPC_OP_SUBFME, OpSubfme);
	xp.Register(Mnemonics::PPC_OP_SUBFMEO, OpSubfmeo);
	xp.Register(Mnemonics::PPC_OP_SUBFZE, OpSubfze);
	xp.Register(Mnemonics::PPC_OP_SUBFZEO, OpSubfzeo);
	xp.Register(Mnemonics::PPC_OP_SYNC, OpSync);
	xp.Register(Mnemonics::PPC_OP_TD, OpTd);
	xp.Register(Mnemonics::PPC_OP_TDI, OpTdi);
	xp.Register(Mnemonics::PPC_OP_TLBIA, OpTlbia);
	xp.Register(Mnemonics::PPC_OP_TLBIE, OpTlbie);
	xp.Register(Mnemonics::PPC_OP_TLBIEL, OpTlbiel);
	xp.Register(Mnemonics::PPC_OP_TLBSYNC, OpTlbsync);
	xp.Register(Mnemonics::PPC_OP_TW, OpTw);
	xp.Register(Mnemonics::PPC_OP_TWI, OpTwi);
	xp.Register(Mnemonics::PPC_OP_XOR, OpXor);
	xp.Register(Mnemonics::PPC_OP_XORI, OpXori);
	xp.Register(Mnemonics::PPC_OP_XORIS, OpXoris);
}

uint32_t instructions[] = {
	0x7d8802a6, 0x48002Bfd, 0x3be1fe10, 0x9421fe10, 0x60000000, 0x7d084378, 0x7d084378, 0x3d209200, 0x3d009200, 0x3960ffff, 0x3940ffff, 0x91697530,
	0x91487534, 0x480005ed, 0x38600001, 0x48000435, 0x4bfffdd9, 0x2c030000, 0x4182000c, 0x48004215, 0x7d084378, 0
};

#include <XexParser.h>
#include <XDVDFSReader.h>

int main()
{
	ifstream f;
	f.open("c:\\users\\alfaomega08\\desktop\\ac4Default.xex", ios::in | ios::binary);

	XexParser p(f);
	p.Read();
	p.ReadBaseFile("c:\\users\\alfaomega08\\desktop\\ac4.exe");

	InitParser();
	int ptr = 0;

	while (instructions[ptr] != 0)
	{
		try
		{
			ps.Instruction = instructions[ptr++];
			xp.Parse(ps.Instruction);
		}
		catch (...)
		{
			cout << "Unknown instruction: 0x" << hex << ps.Instruction << dec << endl;
		}
	}
}
