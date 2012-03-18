/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#pragma once

#include <Types.h>
#include <FileStream.h>
#include <Windows.h>
#include <stdint.h>

#ifdef _XDVDFS_DLL_
#define DLLPREFIX __declspec(dllexport)
#else
#define DLLPREFIX __declspec(dllimport)
#endif

class SectorReader
{
public:
	DLLPREFIX SectorReader(std::string path, bool isPhys = false);
	DLLPREFIX ~SectorReader();

	DLLPREFIX void Read(u8 *buffer, u64 sector);

private:
	bool isPhys;

	// isPhys = false
	FileStream file;
	// isPhys = true
	HANDLE disk;
};

#undef DLLPREFIX
