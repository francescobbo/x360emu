/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#pragma once

#include <Exceptions.h>
#include <Stream.h>

#ifdef _FMW_DLL_
#define DLLPREFIX __declspec(dllexport)
#else
#define DLLPREFIX __declspec(dllimport)
#endif

class MemoryStream : public Stream
{
public:
	DLLPREFIX MemoryStream(u8 *buffer, u64 size);
	
	DLLPREFIX u64 Length();
	DLLPREFIX void Seek(s64 pos, SeekDirection direction);
	DLLPREFIX u64 Position();

	DLLPREFIX u64 Read(u8 *buffer, u64 count);
	DLLPREFIX void Write(const u8 *buffer, u64 count);

	DLLPREFIX bool IsOpen();
	DLLPREFIX bool CanRead();
	DLLPREFIX bool CanWrite();

private:
	u8 *buffer;
	u64 size, ptr;
};

#undef DLLPREFIX
