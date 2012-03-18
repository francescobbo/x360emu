/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include <Exceptions.h>
#include <MemoryStream.h>
#include <string.h>

MemoryStream::MemoryStream(u8 *buffer, u64 size) : buffer(buffer), size(size), ptr(0)
{
	if (!buffer)
		throw ArgumentNullException();
}

u64 MemoryStream::Read(u8 *buf, u64 count)
{
	if (!buf)
		throw ArgumentNullException();

	if (ptr >= size)
	{
		memset(buffer, 0, count);
		return 0;
	}
	else if (ptr + count > size)
	{
		memcpy(buf, &buffer[ptr], size - ptr);
		memset(&buf[size - ptr], 0, count - (size - ptr));
		ptr = size;
		return size - ptr;
	}
	else
	{
		memcpy(buf, &buffer[ptr], count);
		ptr += count;
		return count;
	}
}

void MemoryStream::Write(const u8 *buf, u64 count)
{
	if (!buf)
		throw ArgumentNullException();

	/* We can't expand the buffer. So if a Write request is made out of bounds, throw */
	if (ptr + count > size)
		throw NotSupportedException();

	memcpy(&buffer[ptr], buf, count);
	ptr += count;
}

u64 MemoryStream::Length()
{
	return size;
}

bool MemoryStream::IsOpen()
{
	return true;
}

bool MemoryStream::CanRead()
{
	return true;
}

bool MemoryStream::CanWrite()
{
	return true;
}

void MemoryStream::Seek(s64 pos, SeekDirection direction)
{
	switch (direction)
	{
	case SeekDirection::Begin:
		if (pos >= (s64) size || pos < 0)
			throw ArgumentException();
		ptr = pos;
		break;
	case SeekDirection::Current:
		if (ptr + pos >= size || ptr + pos < 0)
			throw ArgumentException();
		ptr += pos;
		break;
	case SeekDirection::End:
		if (pos >= (s64) size || pos < 0)
			throw ArgumentException();
		ptr = size - pos;
		break;
	default:
		throw ArgumentException();
	}
}

u64 MemoryStream::Position()
{
	return ptr;
}
