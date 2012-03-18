/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#pragma once

#include <fstream>
#include <Stream.h>

#ifdef _FMW_DLL_
#define DLLPREFIX __declspec(dllexport)
#else
#define DLLPREFIX __declspec(dllimport)
#endif

enum FileMode
{
	/* Create a new file. Throws if the file exists */
	CreateNew,
	/* Create a new file, overwriting existing ones */
	Create,
	/* Open an existing file. Throws if the file doesn't exist */
	Open,
	/* Open an existing file, or create a new one if it doesn't */
	OpenOrCreate,
	/* Open an existing file and reduce size to 0. Throws if the file doesn't exist. Write-only */
	Truncate,
	/* Open an existing file and set the pointer to the end. Throws if the file doesn't exist. Write-only */
	Append
};

enum FileAccess
{
	Read = std::ios::in,
	Write = std::ios::out,
	ReadWrite = std::ios::in | std::ios::out
};

class FileStream : public Stream
{
public:
	DLLPREFIX FileStream();
	DLLPREFIX FileStream(std::string path, FileMode mode, FileAccess access);
	DLLPREFIX virtual ~FileStream();
	
	DLLPREFIX void Open(std::string path, FileMode mode, FileAccess access);
	DLLPREFIX void Close();

	DLLPREFIX bool IsOpen();
	DLLPREFIX bool CanRead();
	DLLPREFIX bool CanWrite();

	DLLPREFIX u64 Length();
	DLLPREFIX void Seek(s64 pos, SeekDirection direction);
	DLLPREFIX u64 Position();

	DLLPREFIX u64 Read(u8 *buffer, u64 count);
	DLLPREFIX void Write(const u8 *buffer, u64 count);

private:
	FileMode mode;
	FileAccess access;

	std::fstream stream;
};

#undef DLLPREFIX
