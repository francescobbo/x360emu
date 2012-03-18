/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include <Exceptions.h>
#include <FileStream.h>
#include <Windows.h>

static bool FileExists(const char *path)
{
    DWORD dwAttrib = GetFileAttributes(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

FileStream::FileStream()
{
}

FileStream::FileStream(std::string path, FileMode mode, FileAccess access) : mode(mode), access(access)
{
    Open(path, mode, access);
}

void FileStream::Open(std::string path, FileMode mode, FileAccess access)
{
    this->mode = mode;
    this->access = access;

    if (mode == FileMode::Append || mode == FileMode::Open || mode == FileMode::Truncate)
    {
        if (!FileExists(path.c_str()))
            throw FileNotFoundException();
    }

    if (mode == FileMode::CreateNew)
    {
        if (FileExists(path.c_str()))
            throw IOException();
    }

    if ((mode == FileMode::Append || mode == FileMode::Truncate) && access == FileAccess::Read)
    {
        throw NotSupportedException();
    }

    /* Create the file if it doesn't exist */
    if (mode == FileMode::CreateNew || mode == FileMode::Create || mode == FileMode::OpenOrCreate)
    {
        if (!FileExists(path.c_str()))
        {
            std::fstream tmp(path, std::ios::out);
            /* Let's do a dummy write */
            tmp.write(path.c_str(), 0);
            tmp.close();
        }
    }

    int openFlags = (int) access | std::ios::binary;
    if (mode == FileMode::Append)
        openFlags |= std::ios::ate;
    else if (mode == FileMode::Truncate)
        openFlags |= std::ios::trunc;

    stream.open(path, openFlags);
    if (!stream.is_open() || !stream.good())
        throw IOException();
}

FileStream::~FileStream()
{
    Close();
}

void FileStream::Close()
{
    if (stream.is_open())
        stream.close();
}

u64 FileStream::Read(u8 *buffer, u64 count)
{
    if (!(access & std::ios::in))
        throw NotSupportedException();

    if (!stream.is_open())
        throw IOException();

    if (stream.fail())
        throw IOException();

    if (stream.eof())
        return 0;

    stream.read((char *) buffer, count);
    if (stream.fail())
        throw IOException();

    return stream.gcount();
}

void FileStream::Write(const u8 *buffer, u64 count)
{
    if (!(access & std::ios::out))
        throw NotSupportedException();

    if (!stream.is_open())
        throw IOException();

    if (stream.fail())
        throw IOException();

    stream.write((const char *) buffer, count);
    if (stream.fail())
        throw IOException();
}

u64 FileStream::Length()
{
    /* Save old position */
    std::fstream::pos_type pos = stream.tellg();

    /* Seek to the end */
    stream.seekg(0, std::ios::end);

    /* Get new position */
    u64 ret = stream.tellg();

    /* Restore old position */
    stream.seekg(pos, std::ios::beg);

    return ret;
}

bool FileStream::IsOpen()
{
    return stream.is_open();
}

bool FileStream::CanRead()
{
    return IsOpen() && (access & std::ios::in);
}

bool FileStream::CanWrite()
{
    return IsOpen() && (access & std::ios::out);
}

void FileStream::Seek(s64 pos, SeekDirection direction)
{
    if (!stream.is_open())
        throw IOException();

    switch (direction)
    {
    case SeekDirection::Begin:
        stream.seekg(pos, std::ios::beg);
        break;
    case SeekDirection::Current:
        stream.seekg(pos, std::ios::cur);
        break;
    case SeekDirection::End:
        stream.seekg(pos, std::ios::end);
        break;
    default:
        throw ArgumentException();
    }
}

u64 FileStream::Position()
{
    if (!stream.is_open())
        throw IOException();

    return stream.tellg();
}
