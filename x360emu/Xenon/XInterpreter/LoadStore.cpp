/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "../XInterpreter.h"
#include "../../Memory.h"
#include <stdlib.h>

#define RESERVE_STRICT

#define DOUBLE_SIGN 0x8000000000000000ULL
#define DOUBLE_EXP 0x7FF0000000000000ULL
#define DOUBLE_FRAC 0x000FFFFFFFFFFFFFULL
#define DOUBLE_ZERO 0x0000000000000000ULL

u32 ToSingle(u64 x)
{
#if 0
    /*
     * C++ uses internal methods to cast from double to single, which might
     * even be a lot faster, but I don't know wheter the compiler respects the 
     * PowerPC specification about this kind of conversion
     */
    double tmp = *((double *) &x);
    float f = (float) tmp;
    return *((u32 *) &f);
#else
    u32 exp = (x >> 52) & 0x7FF;
    if (exp > 896 || (x & ~DOUBLE_SIGN) == 0)
        return ((x >> 32) & 0xC0000000) | ((x >> 29) & 0x3FFFFFFF);
    else if (exp >= 874)
    {
        u32 t = (u32)(0x80000000 | ((x & DOUBLE_FRAC) >> 21));
        t = t >> (905 - exp);
        t |= (x >> 32) & 0x80000000;
        return t;
    }
    else
    {
        // This is said to be undefined.
        // The code is based on hardware tests.
        return ((x >> 32) & 0xc0000000) | ((x >> 29) & 0x3fffffff);
    }
#endif
}

void XInterpreter::OpLbz(Xenon::CpuState *xState)
{
    rGPR[INSTR.RD] = Memory::Read8(GetEA(xState));
}

void XInterpreter::OpLbzu(Xenon::CpuState *xState)
{
    rGPR[INSTR.RA] = GetEAu(xState);
    rGPR[INSTR.RD] = Memory::Read8((u32) rGPR[INSTR.RA]);
}

void XInterpreter::OpLbzux(Xenon::CpuState *xState)
{
    rGPR[INSTR.RA] = GetEAXu(xState);
    rGPR[INSTR.RD] = Memory::Read8((u32) rGPR[INSTR.RA]);
}

void XInterpreter::OpLbzx(Xenon::CpuState *xState)
{
    rGPR[INSTR.RD] = Memory::Read8(GetEAX(xState));
}

void XInterpreter::OpLd(Xenon::CpuState *xState)
{
    rGPR[INSTR.RD] = Memory::Read64(GetEA(xState));
}

void XInterpreter::OpLdarx(Xenon::CpuState *xState)
{
    u32 Address = GetEAX(xState);
    rGPR[INSTR.RD] = Memory::Read64(Address);
    xState->ReserveW = true;
    xState->ReserveAddressW = Address;
}

void XInterpreter::OpLdu(Xenon::CpuState *xState)
{
    rGPR[INSTR.RA] = GetEAu(xState);
    rGPR[INSTR.RD] = Memory::Read64((u32) rGPR[INSTR.RA]);
}

void XInterpreter::OpLdux(Xenon::CpuState *xState)
{
    rGPR[INSTR.RA] = GetEAXu(xState);
    rGPR[INSTR.RD] = Memory::Read64((u32) rGPR[INSTR.RA]);
}

void XInterpreter::OpLdx(Xenon::CpuState *xState)
{
    rGPR[INSTR.RD] = Memory::Read64(GetEAX(xState));
}

void XInterpreter::OpLfd(Xenon::CpuState *xState)
{
    rFP[INSTR.FD] = Memory::Read64(GetEA(xState));
}

void XInterpreter::OpLfdu(Xenon::CpuState *xState)
{
    rGPR[INSTR.RA] = GetEAu(xState);
    rFP[INSTR.FD] = Memory::Read64(rGPR[INSTR.RA]);
}

void XInterpreter::OpLfdux(Xenon::CpuState *xState)
{
    rGPR[INSTR.RA] = GetEAXu(xState);
    rFP[INSTR.FD] = Memory::Read64(rGPR[INSTR.RA]);
}

void XInterpreter::OpLfdx(Xenon::CpuState *xState)
{
    rFP[INSTR.FD] = Memory::Read64(GetEAX(xState));
}

void XInterpreter::OpLfs(Xenon::CpuState *xState)
{
    u32 tmp = Memory::Read32(GetEA(xState));
    float val = *(float *) &tmp;
    rFPD[INSTR.FD] = val;
}

void XInterpreter::OpLfsu(Xenon::CpuState *xState)
{
    u32 Address = GetEAu(xState);
    u32 tmp = Memory::Read32(Address);
    rFP[INSTR.FD] = *(float *) &tmp;
    rGPR[INSTR.RA] = Address;
}

void XInterpreter::OpLfsux(Xenon::CpuState *xState)
{
    u32 Address = GetEAXu(xState);
    u32 tmp = Memory::Read32(Address);
    rFP[INSTR.FD] = *(float *) &tmp;
    rGPR[INSTR.RA] = Address;
}

void XInterpreter::OpLfsx(Xenon::CpuState *xState)
{
    u32 tmp = Memory::Read32(GetEAX(xState));
    rFP[INSTR.FD] = *(float *) &tmp;
}

void XInterpreter::OpLha(Xenon::CpuState *xState)
{
    rGPR[INSTR.RD] = (s64) (s16) Memory::Read16(GetEA(xState));
}

void XInterpreter::OpLhau(Xenon::CpuState *xState)
{
    rGPR[INSTR.RA] = GetEAu(xState);
    rGPR[INSTR.RD] = (s64) (s16) Memory::Read16((u32) rGPR[INSTR.RA]);
}

void XInterpreter::OpLhaux(Xenon::CpuState *xState)
{
    rGPR[INSTR.RA] = GetEAXu(xState);
    rGPR[INSTR.RD] = (s64) (s16) Memory::Read16((u32) rGPR[INSTR.RA]);
}

void XInterpreter::OpLhax(Xenon::CpuState *xState)
{
    rGPR[INSTR.RD] = (s64) (s16) Memory::Read16(GetEAX(xState));
}

void XInterpreter::OpLhbrx(Xenon::CpuState *xState)
{
    rGPR[INSTR.RD] = _byteswap_ushort(Memory::Read16(GetEAX(xState)));
}

void XInterpreter::OpLhz(Xenon::CpuState *xState)
{
    rGPR[INSTR.RD] = Memory::Read16(GetEA(xState));
}

void XInterpreter::OpLhzu(Xenon::CpuState *xState)
{
    rGPR[INSTR.RA] = GetEAu(xState);
    rGPR[INSTR.RD] = Memory::Read16((u32) rGPR[INSTR.RA]);
}

void XInterpreter::OpLhzux(Xenon::CpuState *xState)
{
    rGPR[INSTR.RA] = GetEAXu(xState);
    rGPR[INSTR.RD] = Memory::Read16((u32) rGPR[INSTR.RA]);
}

void XInterpreter::OpLhzx(Xenon::CpuState *xState)
{
    rGPR[INSTR.RD] = Memory::Read16(GetEAX(xState));
}

void XInterpreter::OpLmw(Xenon::CpuState *xState)
{
    u32 Address = GetEA(xState);
    for (int iReg = INSTR.RD; iReg <= 31; iReg++, Address += 4)
        rGPR[iReg] = Memory::Read32(Address);
}

void XInterpreter::OpLswi(Xenon::CpuState *xState)
{
    u32 EA = (u32) (INSTR.RA ? rGPR[INSTR.RA] : 0);
    u32 n = INSTR.NB ? INSTR.NB : 32;

    int r = INSTR.RD - 1;
    int i = 0;

    while (n-- > 0)
    {
        if (i == 0)
        {
            r++;
            r &= 31;
            rGPR[r] = 0;
        }

        u32 TempValue = Memory::Read8(EA++) << (24 - i);
        rGPR[r] |= TempValue;
        
        i += 8;
        i &= 31;
    }
}

void XInterpreter::OpLswx(Xenon::CpuState *xState)
{
    u32 EA = GetEAX(xState);
    u32 n = XER.BYTE_COUNT;
    
    int r = INSTR.RD;
    int i = 0;

    if (n > 0)
    {
        rGPR[r] = 0;
        while (n-- > 0)
        {
            u32 TempValue = Memory::Read8(EA++) << (24 - i);
            rGPR[r] |= TempValue;

            i += 8;
            if (i == 32)
            {
                i = 0;
                r = (r + 1) & 31;
                rGPR[r] = 0;
            }
        }
    }
}

void XInterpreter::OpLwa(Xenon::CpuState *xState)
{
    rGPR[INSTR.RD] = (s64) (s32) Memory::Read32(GetEA(xState));
}

void XInterpreter::OpLwarx(Xenon::CpuState *xState)
{
    u32 Address = GetEAX(xState);
    rGPR[INSTR.RD] = Memory::Read32(Address);
    xState->ReserveW = true;
    xState->ReserveAddressW = Address;
}

void XInterpreter::OpLwaux(Xenon::CpuState *xState)
{
    rGPR[INSTR.RA] = GetEAXu(xState);
    rGPR[INSTR.RD] = (s64) (s32) Memory::Read32((u32) rGPR[INSTR.RA]);
}

void XInterpreter::OpLwax(Xenon::CpuState *xState)
{
    rGPR[INSTR.RD] = (s64) (s32) Memory::Read32(GetEAX(xState));
}

void XInterpreter::OpLwbrx(Xenon::CpuState *xState)
{
    rGPR[INSTR.RD] = _byteswap_ulong(Memory::Read32(GetEAX(xState)));
}

void XInterpreter::OpLwz(Xenon::CpuState *xState)
{
    rGPR[INSTR.RD] = Memory::Read32(GetEA(xState));
}

void XInterpreter::OpLwzu(Xenon::CpuState *xState)
{
    rGPR[INSTR.RA] = GetEAu(xState);
    rGPR[INSTR.RD] = Memory::Read32((u32) rGPR[INSTR.RA]);
}

void XInterpreter::OpLwzux(Xenon::CpuState *xState)
{
    rGPR[INSTR.RA] = GetEAXu(xState);
    rGPR[INSTR.RD] = Memory::Read32((u32) rGPR[INSTR.RA]);
}

void XInterpreter::OpLwzx(Xenon::CpuState *xState)
{
    rGPR[INSTR.RD] = Memory::Read32(GetEAX(xState));
}

void XInterpreter::OpStb(Xenon::CpuState *xState)
{
    Memory::Write8(GetEA(xState), (u8) rGPR[INSTR.RS]);
}

void XInterpreter::OpStbu(Xenon::CpuState *xState)
{
    u32 Address = GetEAu(xState);
    Memory::Write8(Address, (u8) rGPR[INSTR.RS]);
    rGPR[INSTR.RA] = Address;
}

void XInterpreter::OpStbux(Xenon::CpuState *xState)
{
    u32 Address = GetEAXu(xState);
    Memory::Write8(Address, (u8) rGPR[INSTR.RS]);
    rGPR[INSTR.RA] = Address;
}

void XInterpreter::OpStbx(Xenon::CpuState *xState)
{
    Memory::Write8(GetEAX(xState), (u8) rGPR[INSTR.RS]);
}

void XInterpreter::OpStd(Xenon::CpuState *xState)
{
    /* Check whether this is STD or STDU */
    if (xState->CurrentInstruction.hex & 1)
    {
        /* GetEAu should be used but we got a bit set in the SIMM_16 for this opCode, that has to be removed! */
        u32 Address = (u32) (rGPR[INSTR.RS] + (((s64) INSTR.SIMM_16) & ~1));
        Memory::Write64(Address, rGPR[INSTR.RS]);
        rGPR[INSTR.RA] = Address;
        return;
    }

    /* STD */
    Memory::Write64(GetEA(xState), rGPR[INSTR.RS]);
}

void XInterpreter::OpStdcx(Xenon::CpuState *xState)
{
    if (!xState->ReserveD)
    {
        xState->SetCrField(0, XER.SO);
        return;
    }

    u32 Address = GetEAX(xState);

#ifdef RESERVE_STRICT
    if (Address == xState->ReserveAddressD)
#endif
    {
        Memory::Write64(Address, rGPR[INSTR.RS]);
        xState->ReserveD = false;
        xState->SetCrField(0, 2 | XER.SO);
    }
#ifdef RESERVE_STRICT
    else
    {
        // Debug::Print("STDCX used but the reserved address do not match");
        xState->SetCrField(0, XER.SO);
    }
#endif
}

void XInterpreter::OpStdux(Xenon::CpuState *xState)
{
    u32 Address = GetEAXu(xState);
    Memory::Write64(Address, rGPR[INSTR.RS]);
    rGPR[INSTR.RA] = Address;
}

void XInterpreter::OpStdx(Xenon::CpuState *xState)
{
    Memory::Write64(GetEAX(xState), rGPR[INSTR.RS]);
}

void XInterpreter::OpStfd(Xenon::CpuState *xState)
{
    Memory::Write64(GetEA(xState), rFP[INSTR.FS]);
}

void XInterpreter::OpStfdu(Xenon::CpuState *xState)
{
    u32 Address = GetEAXu(xState);
    Memory::Write64(Address, rFP[INSTR.FS]);
    rGPR[INSTR.RA] = Address;
}

void XInterpreter::OpStfdux(Xenon::CpuState *xState)
{
    u32 Address = GetEAX(xState);
    Memory::Write64(Address, rFP[INSTR.FS]);
    rGPR[INSTR.RA] = Address;
}

void XInterpreter::OpStfdx(Xenon::CpuState *xState)
{
    Memory::Write64(GetEAX(xState), rFP[INSTR.FS]);
}

void XInterpreter::OpStfiwx(Xenon::CpuState *xState)
{
    Memory::Write32(GetEAX(xState), rFP[INSTR.FS] & 0xFFFFFFFF);
}

void XInterpreter::OpStfs(Xenon::CpuState *xState)
{
    Memory::Write32(GetEA(xState), ToSingle(rFP[INSTR.FS]));
}

void XInterpreter::OpStfsu(Xenon::CpuState *xState)
{
    u32 Address = GetEAu(xState);
    Memory::Write32(Address, ToSingle(rFP[INSTR.FS]));
    rGPR[INSTR.RA] = Address;
}

void XInterpreter::OpStfsux(Xenon::CpuState *xState)
{
    u32 Address = GetEAXu(xState);
    Memory::Write32(Address, ToSingle(rFP[INSTR.FS]));
    rGPR[INSTR.RA] = Address;
}

void XInterpreter::OpStfsx(Xenon::CpuState *xState)
{
    Memory::Write32(GetEAXu(xState), ToSingle(rFP[INSTR.FS]));
}

void XInterpreter::OpSth(Xenon::CpuState *xState)
{
    Memory::Write16(GetEA(xState), (u16) rGPR[INSTR.RS]);
}

void XInterpreter::OpSthbrx(Xenon::CpuState *xState)
{
    Memory::Write16(GetEAX(xState), _byteswap_ushort((u16) rGPR[INSTR.RS]));
}

void XInterpreter::OpSthu(Xenon::CpuState *xState)
{
    u32 Address = GetEAu(xState);
    Memory::Write16(Address, (u16) rGPR[INSTR.RS]);
    rGPR[INSTR.RA] = Address;
}

void XInterpreter::OpSthux(Xenon::CpuState *xState)
{
    u32 Address = GetEAXu(xState);
    Memory::Write16(Address, (u16) rGPR[INSTR.RS]);
    rGPR[INSTR.RA] = Address;
}

void XInterpreter::OpSthx(Xenon::CpuState *xState)
{
    Memory::Write16(GetEAX(xState), (u16) rGPR[INSTR.RS]);
}

void XInterpreter::OpStmw(Xenon::CpuState *xState)
{
    u32 Address = GetEA(xState);
    for (int iReg = INSTR.RS; iReg <= 31; iReg++, Address += 4)
        Memory::Write32(Address, (u32) rGPR[iReg]);
}

/* Well this is a pain. See if it's really used, and discard if no game actually needs it... */
void XInterpreter::OpStswi(Xenon::CpuState *xState)
{
    u32 EA = (u32) (INSTR.RA ? rGPR[INSTR.RA] : 0);
    u32 n = INSTR.NB ? INSTR.NB : 32;

    int r = INSTR.RS - 1;
    int i = 0;

    while (n-- > 0)
    {
        if (i == 0)
        {
            r++;
            r &= 31;
        }

        Memory::Write8(EA++, (rGPR[r] >> (24 - i)) & 0xFF);
        i += 8;
        i &= 31;
    }
}

void XInterpreter::OpStswx(Xenon::CpuState *xState)
{
    u32 EA = GetEAX(xState);
    u32 n = XER.BYTE_COUNT;
    int r = INSTR.RS;
    int i = 0;

    while (n-- > 0)
    {
        Memory::Write8(EA++, (rGPR[r] >> (24 - i)) & 0xFF);

        i += 8;
        if (i == 32)
        {
            i = 0;
            r++;
        }
    }
}

void XInterpreter::OpStw(Xenon::CpuState *xState)
{
    Memory::Write32(GetEA(xState), (u32) rGPR[INSTR.RS]);
}

void XInterpreter::OpStwbrx(Xenon::CpuState *xState)
{
    Memory::Write32(GetEAX(xState), _byteswap_ulong((u32) rGPR[INSTR.RS]));
}

void XInterpreter::OpStwcx(Xenon::CpuState *xState)
{
    if (!xState->ReserveW)
    {
        xState->SetCrField(0, XER.SO);
        return;
    }

    u32 Address = GetEAX(xState);

#ifdef RESERVE_STRICT
    if (Address == xState->ReserveAddressW)
#endif
    {
        Memory::Write32(Address, (u32) rGPR[INSTR.RS]);
        xState->ReserveW = false;
        xState->SetCrField(0, 2 | XER.SO);
    }
#ifdef RESERVE_STRICT
    else
    {
        // Debug::Print("STDCX used but the reserved address do not match");
        xState->SetCrField(0, XER.SO);
    }
#endif
}

void XInterpreter::OpStwu(Xenon::CpuState *xState)
{
    u32 Address = GetEAu(xState);
    Memory::Write32(Address, (u32) rGPR[INSTR.RS]);
    rGPR[INSTR.RA] = Address;
}

void XInterpreter::OpStwux(Xenon::CpuState *xState)
{
    u32 Address = GetEAXu(xState);
    Memory::Write32(Address, (u32) rGPR[INSTR.RS]);
    rGPR[INSTR.RA] = Address;
}

void XInterpreter::OpStwx(Xenon::CpuState *xState)
{
    Memory::Write32(GetEAX(xState), (u32) rGPR[INSTR.RS]);
}
