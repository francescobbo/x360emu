/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#pragma once

#include <Types.h>

enum OpcodeFlags
{
	OP_UNKNOWN = 0x1,

	/*
	 * If using the JIT or the recompiler, signal the necessity of executing
	 * what has been JITed because this instruction is a conditional branch
	 * (or similar)
	 */
	OP_ENDBLOCK = 0x2,

	/* Signal that this is not a proper opcode, but that it links to a subtable */
	OP_SUBTABLE = 0x4,
};

struct OpcodeInfo
{
	int OpCode;
	int Id;
	const char *Name;
	int Flags;
	int Cycles;
};

enum Mnemonics
{
	/* This OpCode was unused on the Xenon. We can use it to jump to HLE functions */
	PPC_OP_HLE_CALL = 1,
	PPC_OP_TDI,
	PPC_OP_TWI,
	PPC_OP_MULLI,
	PPC_OP_SUBFIC,
	PPC_OP_CMPLI,
	PPC_OP_CMPI,
	PPC_OP_ADDIC,
	PPC_OP_ADDIC_UP,
	PPC_OP_ADDI,
	PPC_OP_ADDIS,
	PPC_OP_BC,
	PPC_OP_SC,
	PPC_OP_B,
	PPC_OP_RLWIMI,
	PPC_OP_RLWINM,
	PPC_OP_RLWNM,
	PPC_OP_ORI,
	PPC_OP_ORIS,
	PPC_OP_XORI,
	PPC_OP_XORIS,
	PPC_OP_ANDI_UP,
	PPC_OP_ANDIS_UP,
	PPC_OP_LWZ,
	PPC_OP_LWZU,
	PPC_OP_LBZ,
	PPC_OP_LBZU,
	PPC_OP_STW,
	PPC_OP_STWU,
	PPC_OP_STB,
	PPC_OP_STBU,
	PPC_OP_LHZ,
	PPC_OP_LHZU,
	PPC_OP_LHA,
	PPC_OP_LHAU,
	PPC_OP_STH,
	PPC_OP_STHU,
	PPC_OP_LMW,
	PPC_OP_STMW,
	PPC_OP_LFS,
	PPC_OP_LFSU,
	PPC_OP_LFD,
	PPC_OP_LFDU,
	PPC_OP_STFS,
	PPC_OP_STFSU,
	PPC_OP_STFD,
	PPC_OP_STFDU,
	PPC_OP_STD,

	/* Opcode 19 sub operations */
	PPC_OP_BCCTR,
	PPC_OP_BCLR,
	PPC_OP_CRAND,
	PPC_OP_CRANDC,
	PPC_OP_CREQV,
	PPC_OP_CRNAND,
	PPC_OP_CRNOR,
	PPC_OP_CROR,
	PPC_OP_CRORC,
	PPC_OP_CRXOR,
	PPC_OP_ISYNC,
	PPC_OP_MCRF,
	PPC_OP_RFID,

	/* Opcode 30 sub operations */
	PPC_OP_RLDCL,
	PPC_OP_RLDCR,
	PPC_OP_RLDIC,
	PPC_OP_RLDICL,
	PPC_OP_RLDICR,
	PPC_OP_RLDIMI,

	/* Opcode 31 sub operations */
	PPC_OP_ADD,
	PPC_OP_ADDC,
	PPC_OP_ADDE,
	PPC_OP_ADDME,
	PPC_OP_ADDZE,
	PPC_OP_AND,
	PPC_OP_ANDC,
	PPC_OP_CMP,
	PPC_OP_CMPL,
	PPC_OP_CNTLZD,
	PPC_OP_CNTLZW,
	PPC_OP_DCBA,
	PPC_OP_DCBF,
	PPC_OP_DCBI,
	PPC_OP_DCBST,
	PPC_OP_DCBT,
	PPC_OP_DCBTST,
	PPC_OP_DCBZ,
	PPC_OP_DIVD,
	PPC_OP_DIVDU,
	PPC_OP_DIVW,
	PPC_OP_DIVWU,
	PPC_OP_ECIWX,
	PPC_OP_ECOWX,
	PPC_OP_EIEIO,
	PPC_OP_EQV,
	PPC_OP_EXTSB,
	PPC_OP_EXTSH,
	PPC_OP_EXTSW,
	PPC_OP_ICBI,
	PPC_OP_LBZUX,
	PPC_OP_LBZX,
	PPC_OP_LDARX,
	PPC_OP_LDUX,
	PPC_OP_LDX,
	PPC_OP_LFDUX,
	PPC_OP_LFDX,
	PPC_OP_LFSUX,
	PPC_OP_LFSX,
	PPC_OP_LHAUX,
	PPC_OP_LHAX,
	PPC_OP_LHBRX,
	PPC_OP_LHZUX,
	PPC_OP_LHZX,
	PPC_OP_LSWI,
	PPC_OP_LSWX,
	PPC_OP_LWARX,
	PPC_OP_LWAUX,
	PPC_OP_LWAX,
	PPC_OP_LWBRX,
	PPC_OP_LWZUX,
	PPC_OP_LWZX,
	PPC_OP_MCRXR,
	PPC_OP_MFCR,
	PPC_OP_MFMSR,
	PPC_OP_MFSPR,
	PPC_OP_MFSR,
	PPC_OP_MFSRIN,
	PPC_OP_MFTB,
	PPC_OP_MTCRF,
	PPC_OP_MTMSR,
	PPC_OP_MTMSRD,
	PPC_OP_MTSPR,
	PPC_OP_MTSR,
	PPC_OP_MTSRIN,
	PPC_OP_MULHD,
	PPC_OP_MULHDU,
	PPC_OP_MULHW,
	PPC_OP_MULHWU,
	PPC_OP_MULLD,
	PPC_OP_MULLW,
	PPC_OP_NAND,
	PPC_OP_NEG,
	PPC_OP_NOR,
	PPC_OP_OR,
	PPC_OP_ORC,
	PPC_OP_SLBIA,
	PPC_OP_SLBIE,
	PPC_OP_SLBMFEE,
	PPC_OP_SLBMFEV,
	PPC_OP_SLBMTE,
	PPC_OP_SLD,
	PPC_OP_SLW,
	PPC_OP_SRAD,
	PPC_OP_SRADI,
	PPC_OP_SRADI1,
	PPC_OP_SRAW,
	PPC_OP_SRAWI,
	PPC_OP_SRD,
	PPC_OP_SRW,
	PPC_OP_STBUX,
	PPC_OP_STBX,
	PPC_OP_STDCX_UP,
	PPC_OP_STDUX,
	PPC_OP_STDX,
	PPC_OP_STFDUX,
	PPC_OP_STFDX,
	PPC_OP_STFIWX,
	PPC_OP_STFSUX,
	PPC_OP_STFSX,
	PPC_OP_STHBRX,
	PPC_OP_STHUX,
	PPC_OP_STHX,
	PPC_OP_STSWI,
	PPC_OP_STSWX,
	PPC_OP_STWBRX,
	PPC_OP_STWCX_UP,
	PPC_OP_STWUX,
	PPC_OP_STWX,
	PPC_OP_SUBF,
	PPC_OP_SUBFC,
	PPC_OP_SUBFE,
	PPC_OP_SUBFME,
	PPC_OP_SUBFZE,
	PPC_OP_SYNC,
	PPC_OP_TD,
	PPC_OP_TLBIA,
	PPC_OP_TLBIE,
	PPC_OP_TLBIEL,
	PPC_OP_TLBSYNC,
	PPC_OP_TW,
	PPC_OP_XOR,

	/* Opcode 58 sub operations */
	PPC_OP_LD,
	PPC_OP_LDU,
	PPC_OP_LWA,

	/* Opcode 59 sub operations */
	PPC_OP_FADDS,
	PPC_OP_FDIVS,
	PPC_OP_FMADDS,
	PPC_OP_FMSUBS,
	PPC_OP_FMULS,
	PPC_OP_FNMADDS,
	PPC_OP_FNMSUBS,
	PPC_OP_FRES,
	PPC_OP_FSQRTS,
	PPC_OP_FSUBS,
	/* 202 is used up */

	/* Opcode 63 sub operations */
	PPC_OP_FABS,
	PPC_OP_FADD,
	PPC_OP_FCFID,
	PPC_OP_FCMPO,
	PPC_OP_FCMPU,
	PPC_OP_FCTID,
	PPC_OP_FCTIDZ,
	PPC_OP_FCTIW,
	PPC_OP_FCTIWZ,
	PPC_OP_FDIV,
	PPC_OP_FMADD,
	PPC_OP_FMR,
	PPC_OP_FMSUB,
	PPC_OP_FMUL,
	PPC_OP_FNABS,
	PPC_OP_FNEG,
	PPC_OP_FNMADD,
	PPC_OP_FNMSUB,
	PPC_OP_FRSP,
	PPC_OP_FRSQRTE,
	PPC_OP_FSEL,
	PPC_OP_FSQRT,
	PPC_OP_FSUB,
	PPC_OP_MCRFS,
	PPC_OP_MFFS,
	PPC_OP_MTFSB0,
	PPC_OP_MTFSB1,
	PPC_OP_MTFSF,
	PPC_OP_MTFSFI,

	/* Update this if you add new instructions */
	PPC_OP_INVALID,
	PPC_OP_LAST
};

#ifdef INDLL
#define DLLPREFIX __declspec(dllexport)
#else
#define DLLPREFIX __declspec(dllimport)
#endif

DLLPREFIX void SetupXenonTables();
DLLPREFIX OpcodeInfo *GetInfo(u32 Instruction);
