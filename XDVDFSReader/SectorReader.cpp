/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include <Exceptions.h>
#include <Windows.h>
#include <winioctl.h>
#include <ntddcdrm.h>

#include "SectorReader.h"

using namespace std;

SectorReader::SectorReader(std::string path, bool isPhys) : isPhys(isPhys)
{
	if (!isPhys)
		file.Open(path, FileMode::Open, FileAccess::Read);
	else
	{
		disk = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (disk == INVALID_HANDLE_VALUE)
			throw IOException("XDVDFS: The Device wasn't opened properly.");
	}
}

SectorReader::~SectorReader()
{
	if (isPhys)
		CloseHandle(disk);
	else
		file.Close();
}

void SectorReader::Read(u8 *buffer, u64 sector)
{
	if (!buffer)
		throw ArgumentNullException("SectorReader::Read: Buffer is NULL.");

	if (!isPhys)
	{
		file.Seek(sector * 2048, SeekDirection::Begin);
		file.Read(buffer, 2048);
		return;
	}

	PVOID tmpBuff = VirtualAlloc(NULL, 2048, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	PREVENT_MEDIA_REMOVAL pmrLockCDROM;
	DWORD dwNotUsed;

	/* Lock the DVD drive to prevent accidental removal while reading from it */
	pmrLockCDROM.PreventMediaRemoval = TRUE;
	DeviceIoControl(disk, IOCTL_CDROM_MEDIA_REMOVAL, &pmrLockCDROM, sizeof(pmrLockCDROM), NULL, 0, &dwNotUsed, NULL);

	/* Seek to the sector */
	int64_t pos = sector * 2048;
	int64_t posHi = pos >> 32;
	if (SetFilePointer(disk, pos & 0xFFFFFFFF, (PLONG) &posHi, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		throw "Unable to seek to the sector. Maybe this CD/DVD is too small.";

	/* Read and check errors */
	BOOL readRes = ReadFile(disk, tmpBuff, 2048, &dwNotUsed, NULL);
	if (!!readRes)
		memcpy(buffer, tmpBuff, 2048);

	/* Cleanup Memory (before throwing exceptions) */
	VirtualFree(tmpBuff, 0, MEM_RELEASE);
		
	/* Unlock the DVD drive */
	pmrLockCDROM.PreventMediaRemoval = FALSE;
	DeviceIoControl(disk, IOCTL_CDROM_MEDIA_REMOVAL, &pmrLockCDROM, sizeof(pmrLockCDROM), NULL, 0, &dwNotUsed, NULL);

	/* Report eventual failure */
	if (!readRes)
		throw "Unable to read physical sector from DVD drive";
}
