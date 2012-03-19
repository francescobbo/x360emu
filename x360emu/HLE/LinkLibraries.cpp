/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include <XexParser.h>
#include "../Memory.h"
#include "HLE.h"

#include <map>
#include <iostream>
#include <stdlib.h>

using namespace std;

struct VarExport
{
	uint32_t Id;
	uint64_t Ptr;
	size_t Length;
};

VarExport XboxKrnlVarExports[25] = 
{
	/* ExConsoleGameRegion */
	{ 0x000C, NULL, 2 },
	/* KeDebugMonitorData */
	{ 0x0059, NULL, 4 },
	/* KeTimeStampBundle */
	{ 0x00AD, NULL, 8 },
	/* KiBugCheckData */
	{ 0x00B5, NULL, 4 },
	/* Nls844UnicodeCaseTable */
	{ 0x00CB, NULL, 2655 },
	/* XboxHardwareInfo */
	{ 0x0156, (uint64_t) &XboxHardwareInfo, sizeof(XBOX_HARDWARE_INFO) },
	/* XboxKrnlBaseVersion */
	{ 0x0157, (uint64_t) &XboxKrnlBaseVersion, sizeof(KERNEL_VERSION) },
	/* XboxKrnlVersion */
	{ 0x0158, (uint64_t) &XboxKrnlVersion, sizeof(KERNEL_VERSION) },
	/* XexExecutableModuleHandle */
	{ 0x0193, NULL, 4 },
	/* ExLoadedCommandLine */
	{ 0x01AE, NULL, 512 },
	/* ExLoadedImageName */
	{ 0x01AF, NULL, 256 },
	/* VdGlobalDevice */
	{ 0x01BE, NULL, 4 },
	/* VdGlobalXamDevice */
	{ 0x01BF, NULL, 4 },
	/* VdGpuClockInMHz */
	{ 0x01C0, (uint64_t) &VdGpuClockInMHz, 4 },
	/* VdHSIOCalibrationLock */
	{ 0x01C1, NULL, 28 },
	/* HalFsbInterruptCount */
	{ 0x025C, NULL, 4 },
	/* KeCertMonitorData */
	{ 0x0266, NULL, 4 },
	/* VdHSIOTrainCount */
	{ 0x026B, NULL, 4 },
	/* VdHSIOTrainingStatus */
	{ 0x026D, NULL, 4 },
	/* RgcBindInfo */
	{ 0x026E, NULL, 4 },
	/* HalFsbResetCount */
	{ 0x02AB, NULL, 4 },
	/* UsbdBootEnumerationDoneEvent */
	{ 0x02DB, NULL, 12 },
	/* StfsDeviceErrorEvent */
	{ 0x02DC, NULL, 16 },
	/* UsbdDriverLoadRequiredEvent */
	{ 0x02F1, NULL, 16 },
	/* XboxKrnlVersion4Digit */
	{ 0x0345, NULL, 8 }
};

VarExport XamXexVarExports[2] = 
{
	/* g_XuiAutomation */
	{ 0x03DB, NULL, 4 },
	/* XamSwapDiscPatchAddress */
	{ 0x0A29, NULL, 4 }
};

map<uint32_t, uint64_t> XboxKrnlExports;
map<uint32_t, uint64_t> XamXexExports;

uintptr_t StructsPointer = 0;

void BuildExportsTrees()
{
	XboxKrnlExports[0x52] = (uint64_t) HLE::KeBugCheck;
	XboxKrnlExports[0x53] = (uint64_t) HLE::KeBugCheckEx;
	XboxKrnlExports[0x83] = (uint64_t) HLE::KeQueryPerformanceFrequency;
	XboxKrnlExports[0xCC] = (uint64_t) HLE::NtAllocateVirtualMemory;
	XboxKrnlExports[0xDC] = (uint64_t) HLE::NtFreeVirtualMemory;
	XboxKrnlExports[0x125] = (uint64_t) HLE::RtlEnterCriticalSection;
	XboxKrnlExports[0x12E] = (uint64_t) HLE::RtlInitializeCriticalSection;
	XboxKrnlExports[0x12F] = (uint64_t) HLE::RtlInitializeCriticalSectionAndSpinCount;
	XboxKrnlExports[0x130] = (uint64_t) HLE::RtlLeaveCriticalSection;
}

bool SetupVar(string libName, uint32_t exportId, uint32_t loc)
{
	exportId &= 0xFFFF;

	if (!libName.compare("xboxkrnl.exe"))
	{
		for (int i = 0; i < 25; i++)
		{
			if (XboxKrnlVarExports[i].Id == exportId)
			{
				if (XboxKrnlVarExports[i].Ptr == NULL)
				{
					cout << "WARNING: variable export detected but not implemented yet!";
					Memory::Write32(loc, 0);
					StructsPointer += XboxKrnlVarExports[i].Length + 3;
					StructsPointer &= ~3;
					return true;
				}

				cout << "Found Variable import with ID: " << exportId << " from lib: " << libName.c_str();
				memcpy(Memory::GetPointer(StructsPointer), (const void *) XboxKrnlVarExports[i].Ptr, XboxKrnlVarExports[i].Length);
				Memory::Write32(loc, StructsPointer + 0x80040000);
				StructsPointer += XboxKrnlVarExports[i].Length + 3;
				StructsPointer &= ~3;
				return true;
			}
		}
	}
	else if (!libName.compare("xam.xex"))
	{
		for (int i = 0; i < 2; i++)
		{
			if (XamXexVarExports[i].Id == exportId)
			{
				if (XamXexVarExports[i].Ptr == NULL)
				{
					cout << "WARNING: variable export detected but not implemented yet!" << endl;
					return true;
				}

				cout << "Found Variable import with ID: " << exportId << " from lib: " << libName.c_str() << endl;

				memcpy(Memory::GetPointer(StructsPointer), (const void *) XamXexVarExports[i].Ptr, XamXexVarExports[i].Length);
				Memory::Write32(loc, StructsPointer + 0x80040000);
				StructsPointer += XamXexVarExports[i].Length;
				return true;
			}
		}
	}

	return false;
}

void SetupImports(string libName, ImportLibraries::ImportLib &lib, map<uint32_t, uint64_t> &exports)
{
	BuildExportsTrees();

	for (int i = 0; i < lib.Count; i++)
	{
		uint32_t exportId = Memory::Read32(lib.ImportTable[i]);

		/* Try first with variables and structures */
		if (SetupVar(libName, exportId, lib.ImportTable[i]))
			continue;

		if (exports.find(exportId & 0xFFFF) == exports.end())
			cout << "WARNING: Export #" << exportId << " (from " << libName.c_str() << ") not implemented yet!" << endl;

		uint32_t jumpStubAddr = lib.ImportTable[++i];

		/* Use a the custom opcode 1 to signal "HLE Call" */
		Memory::Write32(jumpStubAddr, 0x04000000 | exportId);
		/* BLR */
		Memory::Write32(jumpStubAddr + 4, 0x4E800020);
	}
}

std::map<uint32_t, map<uint32_t, uint64_t> *> NumToLib;

void LinkLibraries(XexParser &xex)
{
	ImportLibraries *imp = (ImportLibraries *) xex.GetOptHdr(OptHeaderKeys::ImportLibraries);
	if (!imp)
		return;

	for (int i = 0; i < imp->LibraryCount; i++)
	{
		if (!imp->LibNames[i].compare("xboxkrnl.exe"))
		{
			NumToLib[imp->Libs[i].NameIndex] = &XboxKrnlExports;
			SetupImports("xboxkrnl.exe", imp->Libs[i], XboxKrnlExports);
		}
		else if (!imp->LibNames[i].compare("xam.xex"))
		{
			NumToLib[imp->Libs[i].NameIndex] = &XamXexExports;
			SetupImports("xam.xex", imp->Libs[i], XamXexExports);
		}
		else
			cout << "WARNING: import library " << imp->LibNames[i].c_str() << " not supported yet!" << endl;
	}
}

namespace HLE
{
	void Exec(Xenon::CpuState *xState, uint32_t funcId)
	{
		int libN = (funcId >> 16) & 0xFF;
		funcId &= 0xFFFF;

		map<uint32_t, uint64_t> *m = NumToLib[libN];
		if (m->find(funcId) != m->end())
		{
			void (*fnc)(Xenon::CpuState *xState);
			fnc = (void (*)(Xenon::CpuState *xState)) m->at(funcId);
			fnc(xState);
		}
		else
			cout << "WARNING: unknown library call: " << funcId << " on lib #" << libN << endl;
	}
}

namespace HLE
{
	extern CRITICAL_SECTION cs;

	void Init()
	{
		::InitializeCriticalSection(&cs);
	}
}
