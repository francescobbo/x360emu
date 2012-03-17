#include "../XInterpreter.h"
#include <intrin.h>

void UpdateCr0(u64 val, Xenon::CpuState *xState)
{
	s64 sValue = (s64) val;
	u32 newCr0 = 2 << ((sValue != 0) + (sValue < 0));

	newCr0 |= XER.SO;
	xState->SetCrField(0, newCr0);
}

void UpdateCrX(int reg, u64 val, Xenon::CpuState *xState)
{
	s64 sValue = (s64) val;
	u32 newCr0 = 2 << ((sValue != 0) + (sValue < 0));

	newCr0 |= XER.SO;
	xState->SetCrField(reg, newCr0);
}

u32 CreateMask(int mb, int me)
{
	u32 m1 = 0xFFFFFFFF >> mb;
	u32 m2 = me < 31 ? 0xFFFFFFFF >> (me + 1) : 0;

	u32 mask = m1 ^ m2;

	/* Invert the mask if backwards. */
	return me < mb ? ~mask : mask;
}

inline u32 Rotl32(u32 x, int shift)
{
	shift &= 31;
	if (!shift)
		return x;
	return (x << shift) | (x >> (32 - shift));
}

inline u64 Rotl64(u64 x, unsigned int shift)
{
	shift &= 63;
	if (!shift)
		return x;
	return (x << shift) | (x >> (64 - shift));
}

inline u32 Rotr32(u32 x, int shift)
{
    shift &= 31;
    if (!shift)
		return x;
    return (x >> shift) | (x << (32 - shift));
}

inline u64 Rotr64(u64 x, unsigned int shift)
{
	shift &= 63;
	if (!shift)
		return x;
	return (x >> shift) | (x << (64 - shift));
}

void XInterpreter::OpAdd(Xenon::CpuState *xState)
{
	rGPR[INSTR.RD] = rGPR[INSTR.RA] + rGPR[INSTR.RB];
	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpAddo(Xenon::CpuState *xState)
{
	u64 a = rGPR[INSTR.RA];
	u64 b = rGPR[INSTR.RB];
	rGPR[INSTR.RD] = a + b;

	if ((a >> 63 == b >> 63) && (a >> 63 != rGPR[INSTR.RD] >> 63))
		XER.SO = XER.OV = true;
	else
		XER.OV = false;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpAddc(Xenon::CpuState *xState)
{
	u64 a = rGPR[INSTR.RA];
	u64 b = rGPR[INSTR.RB];
	rGPR[INSTR.RD] = a + b;

	XER.CA = a > ~b;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpAddco(Xenon::CpuState *xState)
{
	u64 a = rGPR[INSTR.RA];
	u64 b = rGPR[INSTR.RB];
	rGPR[INSTR.RD] = a + b;

	XER.CA = a > ~b;

	if ((a >> 63 == b >> 63) && (a >> 63 != rGPR[INSTR.RD] >> 63))
		XER.SO = XER.OV = true;
	else
		XER.OV = false;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpAdde(Xenon::CpuState *xState)
{
	int carry = XER.CA;
	u64 a = rGPR[INSTR.RA];
	u64 b = rGPR[INSTR.RB];
	rGPR[INSTR.RD] = a + b + carry;
	
	XER.CA = (a > ~b) || (carry != 0 && (a + b > ~carry));

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpAddeo(Xenon::CpuState *xState)
{
	int carry = XER.CA;
	u64 a = rGPR[INSTR.RA];
	u64 b = rGPR[INSTR.RB];
	rGPR[INSTR.RD] = a + b + carry;
	
	XER.CA = (a > ~b) || (carry != 0 && (a + b > ~carry));

	if ((a >> 63 == b >> 63) && (a >> 63 != rGPR[INSTR.RD] >> 63))
		XER.SO = XER.OV = true;
	else 
		XER.OV = false;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpAddi(Xenon::CpuState *xState)
{
	if (INSTR.RA)
		rGPR[INSTR.RD] = rGPR[INSTR.RA] + INSTR.SIMM_16;
	else
		rGPR[INSTR.RD] = INSTR.SIMM_16;
}

void XInterpreter::OpAddic(Xenon::CpuState *xState)
{
	u64 a = rGPR[INSTR.RA];
	u64 imm = (u64) (s64) INSTR.SIMM_16;

	rGPR[INSTR.RD] = a + imm;
	XER.CA = a > ~imm;
}

void XInterpreter::OpAddicUp(Xenon::CpuState *xState)
{
	OpAddic(xState);
	UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpAddis(Xenon::CpuState *xState)
{
	if (INSTR.RA)
		rGPR[INSTR.RD] = rGPR[INSTR.RA] + ((s64) INSTR.SIMM_16 << 16);
	else
		rGPR[INSTR.RD] = (s64) INSTR.SIMM_16 << 16;
}

void XInterpreter::OpAddme(Xenon::CpuState *xState)
{
	int carry = XER.CA;
	u64 a = rGPR[INSTR.RA];
	rGPR[INSTR.RD] = a + carry - 1;
	
	XER.CA = a > ~(carry - 1);

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpAddmeo(Xenon::CpuState *xState)
{
	u64 carry = XER.CA;
	u64 a = rGPR[INSTR.RA];
	rGPR[INSTR.RD] = a + carry - 1;
	
	XER.CA = a > ~(carry - 1);

	if ((a >> 63 == (carry - 1) >> 63) && (a >> 63 != rGPR[INSTR.RD] >> 63))
		XER.SO = XER.OV = true;
	else
		XER.OV = false;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpAddze(Xenon::CpuState *xState)
{
	u64 carry = XER.CA;
	u64 a = rGPR[INSTR.RA];
	rGPR[INSTR.RD] = a + carry;
	
	XER.CA = a > ~carry;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpAddzeo(Xenon::CpuState *xState)
{
	u64 carry = XER.CA;
	u64 a = rGPR[INSTR.RA];
	rGPR[INSTR.RD] = a + carry;
	
	XER.CA = a > ~carry;

	if (!(a >> 63) && (rGPR[INSTR.RD] >> 63))
		XER.SO = XER.OV = true;
	else
		XER.OV = false;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpAnd(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = rGPR[INSTR.RS] & rGPR[INSTR.RB];
	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpAndc(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = rGPR[INSTR.RS] & ~rGPR[INSTR.RB];
	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpAndiUp(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = rGPR[INSTR.RS] & INSTR.UIMM;
	UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpAndisUp(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = rGPR[INSTR.RS] & ((u64) INSTR.UIMM << 16);
	UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpCmp(Xenon::CpuState *xState)
{
	s64 a = (s64) rGPR[INSTR.RA];
	s64 b = (s64) rGPR[INSTR.RB];

	if (!INSTR.L)
	{
		a = (s64) (s32) (a & 0xFFFFFFFF);
		b = (s64) (s32) (a & 0xFFFFFFFF);
	}

	UpdateCrX(INSTR.CRFD, a - b, xState);
}

void XInterpreter::OpCmpi(Xenon::CpuState *xState)
{
	s64 a = (s64) rGPR[INSTR.RA];
	if (!INSTR.L)
		a = (s64) (s32) (a & 0xFFFFFFFF);

	UpdateCrX(INSTR.CRFD, a - INSTR.SIMM_16, xState);
}

void XInterpreter::OpCmpl(Xenon::CpuState *xState)
{
    u64 a = rGPR[INSTR.RA];
    u64 b = rGPR[INSTR.RB];

	if (!INSTR.L)
	{
		a &= 0xFFFFFFFF;
		b &= 0xFFFFFFFF;
	}

	UpdateCrX(INSTR.CRFD, a - b, xState);
}

void XInterpreter::OpCmpli(Xenon::CpuState *xState)
{
	u64 a = rGPR[INSTR.RA];
	if (!INSTR.L)
		a &= 0xFFFFFFFF;

	UpdateCrX(INSTR.CRFD, a - INSTR.UIMM, xState);
}

void XInterpreter::OpCntlzd(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = __lzcnt64(rGPR[INSTR.RS]);
	if (INSTR.RC)
		UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpCntlzw(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = __lzcnt((u32) rGPR[INSTR.RS]);
	if (INSTR.RC)
		UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpDivd(Xenon::CpuState *xState)
{
	s64 a = rGPR[INSTR.RA];
	s64 b = rGPR[INSTR.RB];

	if (b == 0 || ((u64) a == 0x8000000000000000 && b == -1))
	{
		if (((u64) a & 0x8000000000000000) && b == 0)
			rGPR[INSTR.RD] = -1;
		else
			rGPR[INSTR.RD] = 0;
	}
	else
		rGPR[INSTR.RD] = (u64)(a / b);

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpDivdo(Xenon::CpuState *xState)
{
	s64 a = rGPR[INSTR.RA];
	s64 b = rGPR[INSTR.RB];

	if (b == 0 || ((u64) a == 0x8000000000000000 && b == -1))
	{
		XER.OV = true;

		if (((u64) a & 0x8000000000000000) && b == 0)
			rGPR[INSTR.RD] = -1;
		else
			rGPR[INSTR.RD] = 0;
	}
	else
		rGPR[INSTR.RD] = (u64)(a / b);

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpDivdu(Xenon::CpuState *xState)
{
	u64 a = rGPR[INSTR.RA];
	u64 b = rGPR[INSTR.RB];

	if (b == 0)
		rGPR[INSTR.RD] = 0;
	else
		rGPR[INSTR.RD] = a / b;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpDivduo(Xenon::CpuState *xState)
{
	u64 a = rGPR[INSTR.RA];
	u64 b = rGPR[INSTR.RB];

	if (b == 0)
	{
		XER.OV = true;
		rGPR[INSTR.RD] = 0;
	}
	else
		rGPR[INSTR.RD] = a / b;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpDivw(Xenon::CpuState *xState)
{
	s32 a = (s32) rGPR[INSTR.RA];
	s32 b = (s32) rGPR[INSTR.RB];

	if (b == 0 || ((u32) a == 0x80000000 && b == -1))
	{
		if (((u32) a & 0x80000000) && b == 0)
			rGPR[INSTR.RD] = -1;
		else
			rGPR[INSTR.RD] = 0;
	}
	else
		rGPR[INSTR.RD] = (u32) (a / b);

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpDivwo(Xenon::CpuState *xState)
{
	s32 a = (s32) rGPR[INSTR.RA];
	s32 b = (s32) rGPR[INSTR.RB];

	if (b == 0 || ((u32) a == 0x80000000 && b == -1))
	{
		XER.OV = true;

		if (((u32) a & 0x80000000) && b == 0)
			rGPR[INSTR.RD] = -1;
		else
			rGPR[INSTR.RD] = 0;
	}
	else
		rGPR[INSTR.RD] = (u32) (a / b);

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpDivwu(Xenon::CpuState *xState)
{
	u32 a = (u32) rGPR[INSTR.RA];
	u32 b = (u32) rGPR[INSTR.RB];

	if (b == 0)
		rGPR[INSTR.RD] = 0;
	else
		rGPR[INSTR.RD] = a / b;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpDivwuo(Xenon::CpuState *xState)
{
	u32 a = (u32) rGPR[INSTR.RA];
	u32 b = (u32) rGPR[INSTR.RB];

	if (b == 0)
	{
		XER.OV = true;
		rGPR[INSTR.RD] = 0;
	}
	else
		rGPR[INSTR.RD] = a / b;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpEqv(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = ~(rGPR[INSTR.RS] ^ rGPR[INSTR.RB]);
	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpExtsb(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = (s64) (s8) rGPR[INSTR.RS];
	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpExtsh(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = (s64) (s16) rGPR[INSTR.RS];
	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpExtsw(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = (s64) (s32) rGPR[INSTR.RS];
	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpMulli(Xenon::CpuState *xState)
{
	rGPR[INSTR.RD] = rGPR[INSTR.RA] * INSTR.SIMM_16;
}

void XInterpreter::OpMullw(Xenon::CpuState *xState)
{
	rGPR[INSTR.RD] = (rGPR[INSTR.RA] * rGPR[INSTR.RB]) & 0xFFFFFFFF;
	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpMullwo(Xenon::CpuState *xState)
{
	u64 res = rGPR[INSTR.RA] * rGPR[INSTR.RB];
	rGPR[INSTR.RD] = res & 0xFFFFFFFF;

	XER.OV = (res & 0xFFFFFFFF00000000) != 0;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpOr(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = rGPR[INSTR.RS] | rGPR[INSTR.RB];
	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpOrc(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = rGPR[INSTR.RS] | (~rGPR[INSTR.RB]);
	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpOri(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = rGPR[INSTR.RS] | INSTR.UIMM;
}

void XInterpreter::OpOris(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = rGPR[INSTR.RS] | (INSTR.UIMM << 16);
}

void XInterpreter::OpRlwimi(Xenon::CpuState *xState)
{
	u32 mask = CreateMask(INSTR.MB, INSTR.ME);
	rGPR[INSTR.RA] = (rGPR[INSTR.RA] & ~mask) | (Rotl32(rGPR[INSTR.RS], INSTR.SH) & mask);
	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpRlwinm(Xenon::CpuState *xState)
{
	u32 mask = CreateMask(INSTR.MB, INSTR.ME);
	rGPR[INSTR.RA] = Rotl32((u32) rGPR[INSTR.RS], INSTR.SH) & mask;
	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpSld(Xenon::CpuState *xState)
{
	int n = rGPR[INSTR.RB] & 0x7F;
	if (n > 64)
		rGPR[INSTR.RA] = 0;
	else
		rGPR[INSTR.RA] = rGPR[INSTR.RS] << n;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpSlw(Xenon::CpuState *xState)
{
	int n = rGPR[INSTR.RB] & 0x3F;
	if (n > 32)
		rGPR[INSTR.RA] = 0;
	else
	{
		/* Shift and truncate to 32 bits */
		u32 res = rGPR[INSTR.RS] << n;
		rGPR[INSTR.RA] = res;
	}

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpSrawi(Xenon::CpuState *xState)
{
	int amount = INSTR.SH;

	if (amount)
	{
		s32 rrs = (s32) rGPR[INSTR.RS];
		rGPR[INSTR.RA] = rrs >> amount;

		if ((rrs < 0) && (rrs << (32 - amount)))
			XER.CA = 1;
		else
			XER.CA = 0;
	}
	else
	{
		XER.CA = 0;
		rGPR[INSTR.RA] = rGPR[INSTR.RS];
	}

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpSubf(Xenon::CpuState *xState)
{
	rGPR[INSTR.RD] = rGPR[INSTR.RB] - rGPR[INSTR.RA];
	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpSubfo(Xenon::CpuState *xState)
{
	/*
	 * To detect overflow, it's better to negate the second operand and use an
	 * actually use an addition. This way we can use the same overflow method
	 * used in addo, addco, ...
	 */

	u64 a = ~rGPR[INSTR.RA];
	u64 b = rGPR[INSTR.RB];
	rGPR[INSTR.RD] = a + b + 1;

	if ((a >> 63 == b >> 63) && (a >> 63 != rGPR[INSTR.RD] >> 63))
		XER.SO = XER.OV = true;
	else
		XER.OV = false;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpSubfc(Xenon::CpuState *xState)
{
	s64 a = rGPR[INSTR.RA];
	s64 b = rGPR[INSTR.RB];
	rGPR[INSTR.RD] = b - a;

	XER.CA = a == 0 || (b > ~(-a));

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpSubfco(Xenon::CpuState *xState)
{
	s64 a = ~rGPR[INSTR.RA];
	s64 b = rGPR[INSTR.RB];
	rGPR[INSTR.RD] = b + a + 1;

	XER.CA = a == 0 || (b > ~(-a));
	
	if ((a >> 63 == b >> 63) && (a >> 63 != rGPR[INSTR.RD] >> 63))
		XER.SO = XER.OV = true;
	else
		XER.OV = false;
	
	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpSubfe(Xenon::CpuState *xState)
{
	u64 a = rGPR[INSTR.RA];
	u64 b = rGPR[INSTR.RB];
	int carry = XER.CA;
	rGPR[INSTR.RD] = (~a) + b + carry;

	/* ~a > ~b <=> a < b */
	XER.CA = (a < b) || (~a + b > ~carry);

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpSubfeo(Xenon::CpuState *xState)
{
	u64 a = ~rGPR[INSTR.RA];
	u64 b = rGPR[INSTR.RB];
	int carry = XER.CA;
	rGPR[INSTR.RD] = a + b + carry;

	XER.CA = (a > ~b) || (a + b > ~carry);

	if ((a >> 63 == b >> 63) && (a >> 63 != rGPR[INSTR.RD] >> 63))
		XER.SO = XER.OV = true;
	else
		XER.OV = false;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpSubfic(Xenon::CpuState *xState)
{
	s64 immediate = INSTR.SIMM_16;
	s64 a = (s64) rGPR[INSTR.RA];
	rGPR[INSTR.RD] = immediate - a;
	XER.CA = !rGPR[INSTR.RA] || (-a > ~immediate);
}

void XInterpreter::OpSubfme(Xenon::CpuState *xState)
{
	u64 a = rGPR[INSTR.RA];
	int carry = XER.CA;
	rGPR[INSTR.RD] = ~a + carry - 1;

	/* ~a > ~(carry - 1) <=> a < carry - 1 */
	XER.CA = a < carry - 1;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpSubfmeo(Xenon::CpuState *xState)
{
	u64 a = ~rGPR[INSTR.RA];
	u64 carry = XER.CA;
	rGPR[INSTR.RD] = a + carry - 1;

	XER.CA = a > ~(carry - 1);

	if ((a >> 63 == (carry - 1) >> 63) && (a >> 63 != rGPR[INSTR.RD] >> 63))
		XER.SO = XER.OV = true;
	else
		XER.OV = false;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpSubfze(Xenon::CpuState *xState)
{
	u64 a = rGPR[INSTR.RA];
	int carry = XER.CA;
	rGPR[INSTR.RD] = ~a + carry;

	/* ~a > ~carry <=> a < carry */
	XER.CA = a < carry;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpSubfzeo(Xenon::CpuState *xState)
{
	u64 a = ~rGPR[INSTR.RA];
	int carry = XER.CA;
	rGPR[INSTR.RD] = a + carry;

	XER.CA = a > ~carry;
	
	if (!(a >> 63) && (rGPR[INSTR.RD] >> 63))
		XER.SO = XER.OV = true;
	else
		XER.OV = false;

	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RD], xState);
}

void XInterpreter::OpXor(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = rGPR[INSTR.RS] ^ rGPR[INSTR.RB];
	if (INSTR.Rc)
		UpdateCr0(rGPR[INSTR.RA], xState);
}

void XInterpreter::OpXori(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = rGPR[INSTR.RS] ^ INSTR.UIMM;
}

void XInterpreter::OpXoris(Xenon::CpuState *xState)
{
	rGPR[INSTR.RA] = rGPR[INSTR.RS] ^ (INSTR.UIMM << 16);
}
