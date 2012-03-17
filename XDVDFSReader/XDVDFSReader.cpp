#include "XDVDFSReader.h"
#include <Exceptions.h>
#include <iostream>
#include <vector>
#include <deque>

using namespace std;

XDVDFSReader::XDVDFSReader(std::string path, bool isPhys) : sr(path, isPhys)
{
    u8 buffer[2048];

    uint64_t possibleBases[] = { 0, 0x0FD90000, 0x02080000 };
    base = 0xFFFFFFFF;
    for (int i = 0; i < 3; i++)
    {
        sr.Read(buffer, possibleBases[i] / 0x800 + 32);
        if (!strncmp((const char *) buffer, "MICROSOFT*XBOX*MEDIA", 20))
        {
            base = possibleBases[i];
            break;
        }
    }

    if (base == 0xFFFFFFFF)
        throw "Invalid XBOX 360 Disk.";

    RootDirectorySector = *((uint32_t *) &buffer[20]);
    RootDirectorySize = *((uint32_t *) &buffer[24]);
}

/*
 * Reads the whole file described by @f in the @buffer.
 * WARNING: Be sure to have allocated at least @f->FileSize bytes for buffer
 */
void XDVDFSReader::ReadFile(u8 *buffer, EntryHeader *f)
{
    if (!buffer || !f)
        throw ArgumentNullException("XDVDFSReader: ReadFile has a NULL argument.");

    if (!f->StartSector)
        throw ArgumentException("XDVDFSReader: Invalid EntryHeader structure.");

    /* Adjust the sector to align to the partition in the ISO file */
    u64 sector = f->StartSector + base / 0x800;
    
    /* Read whole sectors while the size is a multiple of the sector-size */
    for (u32 i = 0; i < f->FileSize / 2048; i++)
        sr.Read(&buffer[i * 2048], sector++);

    /*
     * The last piece of the file might not end on a sector boundary and may be
     * skipped by the previous loop.
     */
    if (f->FileSize & 0x7FF)
        sr.Read(&buffer[f->FileSize & ~0x7FF], sector);
}

/**
 * Traverse the whole file system and list every single file/directory.
 * Useful only for debugging pourpose.
 */
void XDVDFSReader::DumpAllFiles()
{
    EntryHeader root;
    root.StartSector = RootDirectorySector;
    root.FileSize = RootDirectorySize;
    DumpDirectory(&root, true);
}

/**
 * Traverse a directory and list every single file/subdirectory.
 * Useful only for debugging pourpose.
 */
void XDVDFSReader::DumpDirectory(EntryHeader *dir, bool recursive)
{
    if (!dir)
        throw ArgumentNullException("XDVDFSReader: directory is NULL.");

    /* size and/or sector = 0 means that the directory contains no files */
    if (!dir->FileSize || !dir->StartSector)
        return;

    u8 *buffer = new u8[dir->FileSize];
    ReadFile(buffer, dir);
    DumpEntry(buffer, (EntryHeader *) buffer, recursive);

    delete[] buffer;
}

vector<string> path;

string GetPath()
{
    string ret = "\\";
    for (size_t i = 0; i < path.size(); i++)
    {
        ret += path[i];
        ret += "\\";
    }
    return ret;
}

void XDVDFSReader::DumpEntry(u8 *Directory, EntryHeader *entry, bool recursive)
{
    char *fn = new char[entry->FileNameLen + 1];
    strncpy(fn, (const char *) &entry->FileNameLen + 1, entry->FileNameLen);
    fn[entry->FileNameLen] = 0;

    cout << GetPath().c_str() << fn << endl;

    if ((entry->Attributes & 0x10) && recursive && entry->FileSize != 0)
    {
        /*
         * Before entering a directory, add the dir name to the path, then
         * remove it when we're done
         */
        path.push_back(string(fn));
        DumpDirectory(entry, true);
        path.pop_back();
    }

    if (recursive)
    {
        if (entry->LeftOffset)
            DumpEntry(Directory, (EntryHeader *) (Directory + entry->LeftOffset * 4), true);
        if (entry->RightOffset)
            DumpEntry(Directory, (EntryHeader *) (Directory + entry->RightOffset * 4), true);
    }
}

void XDVDFSReader::ExtractAllFiles(const char *path)
{
    EntryHeader root;
    root.StartSector = RootDirectorySector;
    root.FileSize = RootDirectorySize;
    ExtractDirectory(path, "\\", &root, true);
}

void XDVDFSReader::ExtractDirectory(const char *basePath, const char *path, EntryHeader *dir, bool recursive)
{
    if (!dir)
        throw ArgumentNullException("XDVDFSReader: directory is NULL.");

    /* size and/or sector = 0 means that the directory contains no files */
    if (!dir->FileSize || !dir->StartSector)
        return;

    string p = basePath;
    p += path;
    ::CreateDirectory(p.c_str(), NULL);

    u8 *buffer = new u8[dir->FileSize];
    ReadFile(buffer, dir);
    ExtractEntry(basePath, buffer, (EntryHeader *) buffer, recursive);

    delete[] buffer;
}

void XDVDFSReader::ExtractEntry(const char *basePath, u8 *Directory, EntryHeader *entry, bool recursive)
{
    char *fn = new char[entry->FileNameLen + 1];
    strncpy(fn, (const char *) &entry->FileNameLen + 1, entry->FileNameLen);
    fn[entry->FileNameLen] = 0;

    cout << GetPath().c_str() << fn << endl;

    if ((entry->Attributes & 0x10) && recursive && entry->FileSize != 0)
    {
        path.push_back(string(fn));
        ExtractDirectory(basePath, (char *) GetPath().c_str(), entry, true);
        path.pop_back();
    }
    else if (entry->Attributes & 0xA0)
    {
        // Normal file to be extracted
        string p = basePath;
        p += GetPath();
        p += fn;
        
        DoExtract(p.c_str(), entry);
    }
    
    if (recursive)
    {
        if (entry->LeftOffset)
            ExtractEntry(basePath, Directory, (EntryHeader *) (Directory + entry->LeftOffset * 4), true);
        if (entry->RightOffset)
            ExtractEntry(basePath, Directory, (EntryHeader *) (Directory + entry->RightOffset * 4), true);
    }
}

deque<string> StringSplit(std::string original, std::string exploder=".")
{
    deque<string> result;

    string tmp = original;
    size_t num = 0, loc = 0;

    while (tmp.find(exploder) != string::npos)
    {
        loc = tmp.find(exploder);
        result.push_back(tmp.substr(0, loc));
        tmp = tmp.substr(loc + exploder.length());
        num++;
    }

    result.push_back(tmp);
    return result;
}

EntryHeader *XDVDFSReader::Find(string dirName, EntryHeader *parent)
{
    EntryHeader *iterator = ReadDirectory(parent);
    u8 *DirBase = (u8 *) iterator;

    while (iterator)
    {
        char *fn = new char[iterator->FileNameLen + 1];
        strncpy(fn, (char *) iterator + 0xE, iterator->FileNameLen);
        fn[iterator->FileNameLen] = 0;

        int cmp = _stricmp(dirName.c_str(), fn);
        if (!cmp)
            return iterator;
        else if (cmp < 0)
        {
            if (iterator->LeftOffset != 0)
                iterator = (EntryHeader *) (DirBase + iterator->LeftOffset * 4);
            else
                iterator = 0;
        }
        else
        {
            if (iterator->RightOffset != 0)
                iterator = (EntryHeader *) (DirBase + iterator->RightOffset * 4);
            else
                iterator = 0;
        }
    }

    throw "Not found!";
}

EntryHeader *XDVDFSReader::ReadDirectory(EntryHeader *dir)
{
    uint64_t sector = dir->StartSector + base / 0x800;
    
    // Div round up to get number of sectors
    u64 size = (dir->FileSize + 2047) / 2048;

    u8 *buffer = new u8[2048 * size];
    for (int i = 0; i < size; i++)
        sr.Read(&buffer[i * 2048], sector++);

    return (EntryHeader *) buffer;
}

EntryHeader *XDVDFSReader::Find(const char *path)
{
    deque<string> Path = StringSplit(path, "\\");
    if (!Path[0].compare(""))
        Path.pop_front();
    EntryHeader rootDir = { 0, 0, RootDirectorySector, RootDirectorySize, 0, 0 };
    EntryHeader *curDir = &rootDir;

    while (Path.size() != 1)
    {
        curDir = Find(Path[0], curDir);
        if (curDir->Attributes & 0x10)
            throw "Found an entry requested as a directory. But it's a file";
        Path.pop_front();
    }

    return Find(Path[0], curDir);
}

void XDVDFSReader::DoExtract(const char *dest, EntryHeader *entry)
{
    FileStream fs(dest, FileMode::Create, FileAccess::Write);
    
    u8 *tmpBuff = new u8[2048];
    for (uint64_t i = 0; i < entry->FileSize / 2048; i++)
    {
        sr.Read(tmpBuff, entry->StartSector + i + base / 0x800);
        fs.Write(tmpBuff, 2048);
    }

    /*
     * The last piece of the file might not end on a sector boundary and can be
     * skipped by the previous loop. Correct this behaviour
     */
    int remaining = entry->FileSize % 2048;
    sr.Read(tmpBuff, entry->StartSector + (entry->FileSize + 2047) / 2048 + base / 0x800);
    fs.Write(tmpBuff, remaining);
}

void XDVDFSReader::ExtractOne(const char *destPath, const char *origPath)
{
    EntryHeader *fileEntry = Find(origPath);
    DoExtract(destPath, fileEntry);
}   

uint64_t XDVDFSReader::ReadOne(const char *path, u8 **buffer)
{
    EntryHeader *fileEntry = Find(path);

    *buffer = new u8[fileEntry->FileSize];
    u8 *realBuff = *buffer;

    ReadFile(*buffer, fileEntry);

    return fileEntry->FileSize;
}
