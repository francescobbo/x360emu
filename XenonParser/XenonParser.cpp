/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

/*
 * Part of the table format and data was taken from Dolphin.
 * The number of cycles per instruction was taken from Dolphin too.
 * But we can't be sure that the cycle latency of each instruction is the same
 * on the Wii and on the Xbox 360.
 *
 * So TODO: check cycle latency for each instruction
 */

#include <Exceptions.h>
#include "XenonParser.h"

OpcodeInfo InvalidOp = { 0, PPC_OP_INVALID, "InvInstr", OP_UNKNOWN, 0 };

OpcodeInfo BaseTable[] =
{
    { 4,   PPC_OP_INVALID, "", OP_SUBTABLE | (0 << 24), 0 },
    { 19,  PPC_OP_INVALID, "", OP_SUBTABLE | (1 << 24), 0 },
    { 30,  PPC_OP_INVALID, "", OP_SUBTABLE | (2 << 24), 0 },
    { 31,  PPC_OP_INVALID, "", OP_SUBTABLE | (3 << 24), 0 },
    { 58,  PPC_OP_INVALID, "", OP_SUBTABLE | (4 << 24), 0 },
    { 59,  PPC_OP_INVALID, "", OP_SUBTABLE | (5 << 24), 0 },
    { 63,  PPC_OP_INVALID, "", OP_SUBTABLE | (6 << 24), 0 },

    { 1,   PPC_OP_HLE_CALL, "HLECall", OP_ENDBLOCK, 0 },
    { 3,   PPC_OP_TWI, "twi", OP_ENDBLOCK, 1  },
    { 7,   PPC_OP_MULLI, "mulli", 0, 3 },
    { 8,   PPC_OP_SUBFIC, "subfic", 0, 1 },

    { 10,  PPC_OP_CMPLI, "cmpli", 0, 1 },
    { 11,  PPC_OP_CMPI, "cmpi", 0, 1 },
    { 12,  PPC_OP_ADDIC, "addic", 0, 1 },
    { 13,  PPC_OP_ADDIC_UP, "addic.", 0, 1 },
    { 14,  PPC_OP_ADDI, "addi", 0, 1 },
    { 15,  PPC_OP_ADDIS, "addis", 0, 1 },
    { 16,  PPC_OP_BC, "bc", OP_ENDBLOCK, 1 },
    { 18,  PPC_OP_B, "b", OP_ENDBLOCK, 1 },
    { 17,  PPC_OP_SC, "sc", OP_ENDBLOCK, 2 },

    { 20,  PPC_OP_RLWIMI, "rlwimi", 0, 1 },
    { 21,  PPC_OP_RLWINM, "rlwinm", 0, 1 },
    { 23,  PPC_OP_RLWNM, "rlwnm", 0, 1 },
    { 24,  PPC_OP_ORI, "ori", 0, 1 },
    { 25,  PPC_OP_ORIS, "oris", 0, 1 },
    { 26,  PPC_OP_XORI, "xori", 0, 1 },
    { 27,  PPC_OP_XORIS, "xoris", 0, 1 },
    { 28,  PPC_OP_ANDI_UP, "andi_rc", 0, 1 },
    { 29,  PPC_OP_ANDIS_UP, "andis_rc", 0, 1 },

    { 32,  PPC_OP_LWZ, "lwz", 0, 1 },
    { 33,  PPC_OP_LWZU, "lwzu", 0, 1 },
    { 34,  PPC_OP_LBZ, "lbz", 0, 1 },
    { 35,  PPC_OP_LBZU, "lbzu", 0, 1 },
    { 36,  PPC_OP_STW, "stw", 0, 1 },
    { 37,  PPC_OP_STWU, "stwu", 0, 1 },
    { 38,  PPC_OP_STB, "stb", 0, 1 },
    { 39,  PPC_OP_STBU, "stbu", 0, 1 },

    { 40,  PPC_OP_LHZ, "lhz", 0, 1 },
    { 41,  PPC_OP_LHZU, "lhzu", 0, 1 },
    { 42,  PPC_OP_LHA, "lha", 0, 1 },
    { 43,  PPC_OP_LHAU, "lhau", 0, 1 },
    { 44,  PPC_OP_STH, "sth", 0, 1 },
    { 45,  PPC_OP_STHU, "sthu", 0, 1 },
    { 46,  PPC_OP_LMW, "lmw", 0, 11 },
    { 47,  PPC_OP_STMW, "stmw", 0, 11 },
    { 48,  PPC_OP_LFS, "lfs", 0, 1 },
    { 49,  PPC_OP_LFSU, "lfsu", 0, 1 },

    { 50,  PPC_OP_LFD, "lfd", 0, 1 },
    { 51,  PPC_OP_LFDU, "lfdu", 0, 1 },
    { 52,  PPC_OP_STFS, "stfs", 0, 1 },
    { 53,  PPC_OP_STFSU, "stfsu", 0, 1 },
    { 54,  PPC_OP_STFD, "stfd", 0, 1 },
    { 55,  PPC_OP_STFDU, "stfdu", 0, 1 },

    { 62,  PPC_OP_STD, "std", 0, 1 },

    /* Unused opcodes */
    { 0,   PPC_OP_INVALID, "InvInstr", OP_UNKNOWN, 0 },
    { 2,   PPC_OP_INVALID, "InvInstr", OP_UNKNOWN, 0 },
    { 5,   PPC_OP_INVALID, "InvInstr", OP_UNKNOWN, 0 },
    { 6,   PPC_OP_INVALID, "InvInstr", OP_UNKNOWN, 0 },
    { 9,   PPC_OP_INVALID, "InvInstr", OP_UNKNOWN, 0 },
    { 22,  PPC_OP_INVALID, "InvInstr", OP_UNKNOWN, 0 },
    { 56,  PPC_OP_INVALID, "InvInstr", OP_UNKNOWN, 0 },
    { 57,  PPC_OP_INVALID, "InvInstr", OP_UNKNOWN, 0 },
    { 60,  PPC_OP_INVALID, "InvInstr", OP_UNKNOWN, 0 },
    { 61,  PPC_OP_INVALID, "InvInstr", OP_UNKNOWN, 0 },
};

/* VMX128 specific table */
OpcodeInfo Table4[] = 
{
    { 0,   0, "", OP_UNKNOWN, 0 }
};

OpcodeInfo Table19[] =
{
    { 0,   PPC_OP_MCRF, "mcrf", 0, 1 },
    { 16,  PPC_OP_BCLR, "bclr", OP_ENDBLOCK, 1 },
    { 18,  PPC_OP_RFID, "rfid", OP_ENDBLOCK, 1 },
    { 33,  PPC_OP_CRNOR, "crnor", 0, 1 },
    { 129, PPC_OP_CRANDC, "crandc", 0, 1 },
    { 150, PPC_OP_ISYNC, "isync", 0, 1 },
    { 193, PPC_OP_CRXOR, "crxor", 0, 1 },
    { 225, PPC_OP_CRNAND, "crnand", 0, 1 },
    { 257, PPC_OP_CRAND, "crand", 0, 1 },
    { 289, PPC_OP_CREQV, "creqv", 0, 1 },
    { 417, PPC_OP_CRORC, "crorc", 0, 1 },
    { 449, PPC_OP_CROR, "cror", 0, 1 },
    { 528, PPC_OP_BCCTR, "bcctr", OP_ENDBLOCK, 1 },
};

OpcodeInfo Table30[] =
{
    /* The last bit has a special meaning in the first 4 pairs. Each pair maps to the same OpCode */
    { 0,   PPC_OP_RLDICL, "rldicl", 0, 2 },
    { 1,   PPC_OP_RLDICL, "rldicl", 0, 2 },
    { 2,   PPC_OP_RLDICR, "rldicr", 0, 2 },
    { 3,   PPC_OP_RLDICR, "rldicr", 0, 2 },
    { 4,   PPC_OP_RLDIC, "rldic", 0, 2 },
    { 5,   PPC_OP_RLDIC, "rldic", 0, 2 },
    { 6,   PPC_OP_RLDIMI, "rldimi", 0, 2 },
    { 7,   PPC_OP_RLDIMI, "rldimi", 0, 2 },

    { 8,   PPC_OP_RLDCL, "rldcl", 0, 2 },
    { 9,   PPC_OP_RLDCR, "rldcr", 0, 2 }
};

OpcodeInfo Table31[] =
{
    { 0,   PPC_OP_CMP, "cmp", 0, 1 },
    { 4,   PPC_OP_TW, "tw", OP_ENDBLOCK, 2 },
    { 9,   PPC_OP_MULHDU, "mulhdu", 0, 5 },
    { 19,  PPC_OP_MFCR, "mfcr", 0, 1 },
    { 20,  PPC_OP_LWARX, "lwarx", 0, 1 },
    { 21,  PPC_OP_LDX, "ldx", 0, 1 },
    { 23,  PPC_OP_LWZX, "lwzx", 0, 1 },
    { 24,  PPC_OP_SLW, "slw", 0, 1 },
    { 26,  PPC_OP_CNTLZW, "cntlzwx", 0, 1 },
    { 27,  PPC_OP_SLD, "sld", 0, 1 },
    { 28,  PPC_OP_AND, "and", 0, 1 },
    { 32,  PPC_OP_CMPL, "cmpl", 0, 1 },
    { 53,  PPC_OP_LDUX, "ldux", 0, 1 },
    { 54,  PPC_OP_DCBST, "dcbst", 0, 5 },
    { 55,  PPC_OP_LWZUX, "lwzux", 0, 1 },
    { 58,  PPC_OP_CNTLZD, "cntlzd", 0, 1 },
    { 60,  PPC_OP_ANDC, "andc", 0, 1 },
    { 68,  PPC_OP_TD, "td", OP_ENDBLOCK, 2 },
    { 73,  PPC_OP_MULHD, "mulhd", 0, 5 },
    { 83,  PPC_OP_MFMSR, "mfmsr", 0, 1 },
    { 84,  PPC_OP_LDARX, "ldarx", 0, 1 },
    { 86,  PPC_OP_DCBF, "dcbf", 0, 5 },
    { 87,  PPC_OP_LBZX, "lbzx", 0, 1 },

    { 119, PPC_OP_LBZUX, "lbzux", 0, 1 },
    { 124, PPC_OP_NOR, "nor", 0, 1 },
    { 144, PPC_OP_MTCRF, "mtcrf", 0, 1 },
    { 146, PPC_OP_MTMSR, "mtmsr", 0, 1 },
    { 149, PPC_OP_STDX, "stdx", 0, 1 },
    { 150, PPC_OP_STWCX_UP, "stwcx.", 0, 1 },
    { 151, PPC_OP_STWX, "stwx", 0, 1 },
    { 178, PPC_OP_MTMSRD, "mtmsrd", 0, 1 },
    { 181, PPC_OP_STDUX, "stdux", 0, 1 },
    { 183, PPC_OP_STWUX, "stwux", 0, 1 },

    { 210, PPC_OP_MTSR, "mtsr", 0, 1 },
    { 214, PPC_OP_STDCX_UP, "stdcx.", 0, 1 },
    { 215, PPC_OP_STBX, "stbx", 0, 1 },
    { 242, PPC_OP_MTSRIN, "mtsrin", 0, 1 },
    { 246, PPC_OP_DCBTST, "dcbtst", 0, 2 },
    { 247, PPC_OP_STBUX, "stbux", 0, 1 },
    { 274, PPC_OP_TLBIEL, "tlbiel", 0, 1 },
    { 278, PPC_OP_DCBT, "dcbt", 0, 2 },
    { 279, PPC_OP_LHZX, "lhzx", 0, 1 },
    { 284, PPC_OP_EQV, "eqv", 0, 1 },

    { 306, PPC_OP_TLBIE, "tlbie", 0, 1 },
    { 310, PPC_OP_ECIWX, "eciwx", 0, 1 },
    { 311, PPC_OP_LHZUX, "lhzux", 0, 1 },
    { 316, PPC_OP_XOR, "xor", 0, 1 },
    { 339, PPC_OP_MFSPR, "mfspr", 0, 1 },
    { 341, PPC_OP_LWAX, "lwax", 0, 1 },
    { 343, PPC_OP_LHAX, "lhax", 0, 1 },
    { 370, PPC_OP_TLBIA, "tlbia", 0, 1 },
    { 371, PPC_OP_MFTB, "mftb", 0, 1 },
    { 373, PPC_OP_LWAUX, "lwaux", 0, 1 },
    { 375, PPC_OP_LHAUX, "lhaux", 0, 1 },

    { 402, PPC_OP_SLBMTE, "slbmte", 0, 1 },
    { 407, PPC_OP_STHX, "sthx", 0, 1 },
    { 412, PPC_OP_ORC, "orc", 0, 1 },
    { 434, PPC_OP_SLBIE, "slbie", 0, 1 },
    { 438, PPC_OP_ECOWX, "ecowx", 0, 1 },
    { 439, PPC_OP_STHUX, "sthux", 0, 1 },
    { 444, PPC_OP_OR, "or", 0, 1 },
    { 467, PPC_OP_MTSPR, "mtspr", 0, 3 },
    { 470, PPC_OP_DCBI, "dcbi", 0, 5 }, // Is this present on the Xenon?
    { 476, PPC_OP_NAND, "nand", 0, 1 },
    { 498, PPC_OP_SLBIA, "slbia", 0, 1 },

    { 512, PPC_OP_MCRXR, "mcrxr", 0, 1 },
    { 533, PPC_OP_LSWX, "lswx", 0, 1 },
    { 534, PPC_OP_LWBRX, "lwbrx", 0, 1 },
    { 535, PPC_OP_LFSX, "lfsx", 0, 1 },
    { 536, PPC_OP_SRW, "srw", 0, 1 },
    { 539, PPC_OP_SRD, "srd", 0, 1 },
    { 566, PPC_OP_TLBSYNC, "tlbsync", 0, 1 },
    { 567, PPC_OP_LFSUX, "lfsux", 0, 1 },
    { 595, PPC_OP_MFSR, "mfsr", 0, 3 },
    { 597, PPC_OP_LSWI, "lswi", 0, 1 },
    { 598, PPC_OP_SYNC, "sync", 0, 3 },
    { 599, PPC_OP_LFDX, "lfdx", 0, 1 },

    { 631, PPC_OP_LFDUX, "lfdux", 0, 1 },
    { 659, PPC_OP_MFSRIN, "mfsrin", 0, 3 },
    { 661, PPC_OP_STSWX, "stswx", 0, 1 },
    { 662, PPC_OP_STWBRX, "stwbrx", 0, 1 },
    { 663, PPC_OP_STFSX, "stfsx", 0, 1 },
    { 695, PPC_OP_STFSUX, "stfsux", 0, 1 },

    { 725, PPC_OP_STSWI, "stswi", 0, 1 },
    { 727, PPC_OP_STFDX, "stfdx", 0, 1 },
    { 758, PPC_OP_DCBA, "dcba", 0, 5 }, // Is this present on the Xenon?
    { 759, PPC_OP_STFDUX, "stfdux", 0, 1 },
    { 790, PPC_OP_LHBRX, "lhbrx", 0, 1 },
    { 792, PPC_OP_SRAW, "sraw", 0, 1 },
    { 794, PPC_OP_SRAD, "srad", 0, 1 },

    { 824, PPC_OP_SRAWI, "srawi", 0, 1 },
    /* The next two maps to the same opcode. The last bit has a special meaning */
    { 826, PPC_OP_SRADI, "sradi", 0, 1 },
    { 827, PPC_OP_SRADI, "sradi", 0, 1 },
    { 851, PPC_OP_SLBMFEV, "slbmfev", 0, 1 },
    { 854, PPC_OP_EIEIO, "eieio", 0, 1 },

    { 915, PPC_OP_SLBMFEE, "slbmfee", 0, 1 },
    { 918, PPC_OP_STHBRX, "sthbrx", 0, 1 },
    { 922, PPC_OP_EXTSH, "extsh", 0, 1 },
    { 954, PPC_OP_EXTSB, "extsb", 0, 1 },
    { 982, PPC_OP_ICBI, "icbi", 0, 4 },
    { 983, PPC_OP_STFIWX, "stfiwx", 0, 1 },
    { 986, PPC_OP_EXTSW, "extsw", 0, 1 },

    { 1014, PPC_OP_DCBZ, "dcbz", 0, 5 },
};

OpcodeInfo Table31_2[] = 
{   
    { 8, PPC_OP_SUBFC, "subfc", 0, 1 },
    { 10, PPC_OP_ADDC, "addc", 0, 1 },
    { 11, PPC_OP_MULHWU, "mulhwu", 0, 5 },
    { 40, PPC_OP_SUBF, "subf", 0, 1 },
    { 75, PPC_OP_MULHW, "mulhw", 0, 5 },

    { 104, PPC_OP_NEG, "negx", 0, 1 },
    { 136, PPC_OP_SUBFE, "subfe", 0, 1 },
    { 138, PPC_OP_ADDE, "adde", 0, 1 },
    
    { 200, PPC_OP_SUBFZE, "subfze", 0, 1 },
    { 202, PPC_OP_ADDZE, "addze", 0, 1 },
    { 232, PPC_OP_SUBFME, "subfme", 0, 1 },
    { 233, PPC_OP_MULLD, "mulld", 0, 5 },
    { 234, PPC_OP_ADDME, "addme", 0, 1 },
    { 235, PPC_OP_MULLW, "mullw", 0, 5 },
    { 266, PPC_OP_ADD, "add", 0, 1 },
    
    { 457, PPC_OP_DIVDU, "divdu", 0, 40 },
    { 459, PPC_OP_DIVWU, "divwu", 0, 40 },
    { 489, PPC_OP_DIVD, "divd", 0, 40 },
    { 491, PPC_OP_DIVW, "divw", 0, 40 },
};

OpcodeInfo Table58[] =
{
    { 0, PPC_OP_LD, "ld", 0, 1 },
    { 1, PPC_OP_LDU, "ldu", 0, 1 },
    { 2, PPC_OP_LWA, "lwa", 0, 1 }
};

OpcodeInfo Table59[] =
{
    { 18, PPC_OP_FDIVS, "fdivs", 0, 17 },
    { 20, PPC_OP_FSUBS, "fsubs", 0, 1 },
    { 21, PPC_OP_FADDS, "fadds", 0, 1 },
    { 22, PPC_OP_FSQRTS, "fsqrts", 0, 1 },
    { 24, PPC_OP_FRES, "fres", 0, 1 },
    { 25, PPC_OP_FMULS, "fmuls", 0, 1 },
    { 28, PPC_OP_FMSUBS, "fmsubs", 0, 1 },
    { 29, PPC_OP_FMADDS, "fmadds", 0, 1 },
    { 30, PPC_OP_FNMSUBS, "fnmsubs", 0, 1 },
    { 31, PPC_OP_FNMADDS, "fnmadds", 0, 1 },
};

OpcodeInfo Table63[] =
{
    { 0,   PPC_OP_FCMPU, "fcmpu", 0, 1 },
    { 12,  PPC_OP_FRSP, "frsp", 0, 1 },
    { 14,  PPC_OP_FCTIW, "fctiw", 0, 1 },
    { 15,  PPC_OP_FCTIWZ, "fctiwz", 0, 1 },
    { 32,  PPC_OP_FCMPO, "fcmpo", 0, 1 },
    { 38,  PPC_OP_MTFSB1, "mtfsb1", 0, 3 },
    { 40,  PPC_OP_FNEG, "fneg", 0, 1 },
    { 64,  PPC_OP_MCRFS, "mcrfs", 0, 1 },
    { 70,  PPC_OP_MTFSB0, "mtfsb0", 0, 3 },
    { 72,  PPC_OP_FMR, "fmr", 0, 1 },
    { 134, PPC_OP_MTFSFI, "mtfsfi", 0, 3 },
    { 136, PPC_OP_FNABS, "fnabs", 0, 1 },
    { 264, PPC_OP_FABS, "fabs", 0, 1 },
    { 583, PPC_OP_MFFS, "mffs", 0, 1 },
    { 711, PPC_OP_MTFSF, "mtfsf", 0, 3 },
    { 814, PPC_OP_FCTID, "fctid", 0, 1 },
    { 815, PPC_OP_FCTIDZ, "fctidz", 0, 1 },
    { 846, PPC_OP_FCFID, "fcfid", 0, 1 },
};

OpcodeInfo Table63_2[] = 
{
    { 18, PPC_OP_FDIV, "fdiv", 0, 32 },
    { 20, PPC_OP_FSUB, "fsub", 0, 2 },
    { 21, PPC_OP_FADD, "fadd", 0, 2 },
    { 22, PPC_OP_FSQRT, "fsqrt", 0, 2 },
    { 23, PPC_OP_FSEL, "fsel", 0, 2 },
    { 25, PPC_OP_FMUL, "fmul", 0, 2 },
    { 26, PPC_OP_FRSQRTE, "frsqrte", 0, 2 },
    { 28, PPC_OP_FMSUB, "fmsub", 0, 2 },
    { 29, PPC_OP_FMADD, "fmadd", 0, 2 },
    { 30, PPC_OP_FNMSUB, "fnmsub", 0, 2 },
    { 31, PPC_OP_FNMADD, "fnmadd", 0, 2 },
};

OpcodeInfo *RealTableBase[64];
OpcodeInfo *RealTable4[1024];
OpcodeInfo *RealTable19[1024];
OpcodeInfo *RealTable30[16];
OpcodeInfo *RealTable31[1024];
OpcodeInfo *RealTable58[4];
OpcodeInfo *RealTable59[32];
OpcodeInfo *RealTable63[1024];

OpcodeInfo **SubTables[] =
{
    RealTable4, RealTable19, RealTable30, RealTable31, RealTable58, RealTable59, RealTable63,
};

void SetupXenonTables()
{
    for (int i = 0; i < 64; i++)
        RealTableBase[BaseTable[i].OpCode] = &BaseTable[i];

    /* Set all the entries to OP_INVALID */
    for (int i = 0; i < 1024; i++)
        RealTable4[i] = &InvalidOp;
    for (int i = 0; i < 1024; i++)
        RealTable19[i] = &InvalidOp;
    for (int i = 0; i < 16; i++)
        RealTable30[i] = &InvalidOp;
    for (int i = 0; i < 1024; i++)
        RealTable31[i] = &InvalidOp;
    for (int i = 0; i < 4; i++)
        RealTable58[i] = &InvalidOp;
    for (int i = 0; i < 32; i++)
        RealTable59[i] = &InvalidOp;
    for (int i = 0; i < 1024; i++)
        RealTable63[i] = &InvalidOp;

    for (int i = 0; i < (int)(sizeof(Table19) / sizeof(OpcodeInfo)); i++)
    {
        int op = Table19[i].OpCode;
        RealTable19[op] = &Table19[i];
    }

    for (int i = 0; i < (int)(sizeof(Table30) / sizeof(OpcodeInfo)); i++)
    {
        int op = Table30[i].OpCode;
        RealTable30[op] = &Table30[i];
    }

    for (int i = 0; i < (int)(sizeof(Table31) / sizeof(OpcodeInfo)); i++)
    {
        int op = Table31[i].OpCode;
        RealTable31[op] = &Table31[i];
    }

    /* The Table31_2 contains OpCodes that may or not have the OV bit */
    for (int i = 0; i < 1; i++)
    {
        int fill = i << 9;
        for (int j = 0; j < (int)(sizeof(Table31_2) / sizeof(OpcodeInfo)); j++)
        {
            int op = fill + Table31_2[j].OpCode;
            RealTable31[op] = &Table31_2[j];
        }
    }

    for (int i = 0; i < (int)(sizeof(Table58) / sizeof(OpcodeInfo)); i++)
    {
        int op = Table58[i].OpCode;
        RealTable58[op] = &Table58[i];
    }

    for (int i = 0; i < (int)(sizeof(Table59) / sizeof(OpcodeInfo)); i++)
    {
        int op = Table59[i].OpCode;
        RealTable59[op] = &Table59[i];
    }

    for (int i = 0; i < (int)(sizeof(Table63) / sizeof(OpcodeInfo)); i++)
    {
        int op = Table63[i].OpCode;
        RealTable63[op] = &Table63[i];
    }

    for (int i = 0; i < 32; i++)
    {
        int fill = i << 5;
        for (int j = 0; j < (int)(sizeof(Table63_2) / sizeof(OpcodeInfo)); j++)
        {
            int op = fill + Table63_2[j].OpCode;
            RealTable63[op] = &Table63_2[j];
        }
    }
}

OpcodeInfo *GetInfo(u32 Instruction)
{
    u32 base = Instruction >> 26;
    if (!(RealTableBase[base]->Flags & OP_SUBTABLE))
        return RealTableBase[base];

    int subId = (RealTableBase[base]->Flags >> 24) & 7;
    OpcodeInfo **SubTable = SubTables[subId];

    if (base == 31 || base == 19 || base == 63 || base == 4)
        return SubTable[(Instruction >> 1) & 0x3FF];
    else if (base == 58)
        return SubTable[Instruction & 0x3];
    else if (base == 30)
        return SubTable[Instruction & 0xF];
    else if (base == 59)
        return SubTable[(Instruction >> 1) & 0x1F];

    throw Exception("XenonParser: How the f*** did you get here??");
}
