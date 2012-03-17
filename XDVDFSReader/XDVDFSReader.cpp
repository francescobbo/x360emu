#include "XDVDFSReader.h"
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

vector<string> path;

void XDVDFSReader::DumpAllFiles()
{
	DumpDirectory(RootDirectorySector, RootDirectorySize, true);
}

void XDVDFSReader::DumpDirectory(uint64_t sector, uint64_t size, bool recursive)
{
	if (!size)
		return;

	sector += base / 0x800;
	
	// Div round up to get number of sectors
	size = (size + 2047) / 2048;

	u8 *buffer = new u8[2048 * size];

	for (int i = 0; i < size; i++)
		sr.Read(&buffer[i * 2048], sector++);

	DumpEntry(buffer, buffer, recursive);

	delete[] buffer;
}

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

void XDVDFSReader::DumpEntry(u8 *DirBase, u8 *buffer, bool recursive)
{
	EntryHeader *hdr = (EntryHeader *) buffer;

	char *fn = new char[hdr->FileNameLen + 1];
	fn[hdr->FileNameLen] = 0;
	strncpy(fn, (const char *) &buffer[0xE], hdr->FileNameLen);
	
	cout << GetPath().c_str() << fn << endl;

	if ((hdr->Attributes & 0x10) && recursive && hdr->FileSize != 0)
	{
		path.push_back(string(fn));
		DumpDirectory(hdr->StartSector, hdr->FileSize, true);
		path.pop_back();
	}

	if (recursive)
	{
		if (hdr->LeftOffset)
			DumpEntry(DirBase, DirBase + hdr->LeftOffset * 4, true);
		if (hdr->RightOffset)
			DumpEntry(DirBase, DirBase + hdr->RightOffset * 4, true);
	}
}

void XDVDFSReader::ExtractAllFiles(char *path)
{
	ExtractDirectory(path, "\\", RootDirectorySector, RootDirectorySize, true);
}

void XDVDFSReader::ExtractDirectory(char *basePath, char *path, uint64_t sector, uint64_t size, bool recursive)
{
	if (!size)
		return;

	string p = basePath;
	p += path;
	::CreateDirectory(p.c_str(), NULL);

	sector += base / 0x800;
	
	// Div round up to get number of sectors
	size = (size + 2047) / 2048;

	u8 *buffer = new u8[2048 * size];

	for (int i = 0; i < size; i++)
		sr.Read(&buffer[i * 2048], sector++);

	ExtractEntry(basePath, buffer, buffer, recursive);

	delete[] buffer;
}

void XDVDFSReader::ExtractEntry(char *basePath, u8 *DirBase, u8 *buffer, bool recursive)
{
	EntryHeader *hdr = (EntryHeader *) buffer;

	char *fn = new char[hdr->FileNameLen + 1];
	fn[hdr->FileNameLen] = 0;
	strncpy(fn, (const char *) &buffer[0xE], hdr->FileNameLen);
	
	cout << GetPath().c_str() << fn << endl;

	if ((hdr->Attributes & 0x10) && recursive && hdr->FileSize != 0)
	{
		path.push_back(string(fn));
		ExtractDirectory(basePath, (char *) GetPath().c_str(), hdr->StartSector, hdr->FileSize, true);
		path.pop_back();
	}
	else if (hdr->Attributes & 0xA0)
	{
		// Normal file to be extracted
		string p = basePath;
		p += GetPath();
		p += fn;
		
		DoExtract(p.c_str(), hdr);
	}
	
	if (recursive)
	{
		if (hdr->LeftOffset)
			ExtractEntry(basePath, DirBase, DirBase + hdr->LeftOffset * 4, true);
		if (hdr->RightOffset)
			ExtractEntry(basePath, DirBase, DirBase + hdr->RightOffset * 4, true);
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

EntryHeader *XDVDFSReader::FindSubdir(string dirName, EntryHeader *parent)
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
		{
			if (iterator->Attributes & 0x10)
				return iterator;
			throw "Found a file with the good name. But it's not a directory as requested!";
		}
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

	throw "Directory not found!";
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

EntryHeader *XDVDFSReader::FindFile(string dirName, EntryHeader *parent)
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
		{
			if (iterator->Attributes & 0xA0)
				return iterator;
			throw "Found a file with the good name. But it's not a \"normal\" file...";
		}
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

	throw "File not found!";
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
		curDir = FindSubdir(Path[0], curDir);
		Path.pop_front();
	}

	return FindFile(Path[0], curDir);
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

void XDVDFSReader::ExtractOne(char *destPath, char *origPath)
{
	EntryHeader *fileEntry = Find(origPath);
	DoExtract(destPath, fileEntry);
}	

uint64_t XDVDFSReader::ReadOne(char *path, char **buffer)
{
	EntryHeader *fileEntry = Find(path);

	*buffer = new char[fileEntry->FileSize];
	char *realBuff = *buffer;

	for (uint64_t i = 0; i < fileEntry->FileSize / 2048; i++)
		sr.Read((u8 *) &realBuff[i * 2048], fileEntry->StartSector + i + base / 0x800);

	/*
	 * Since the above will read only 2048 bytes blocks, it will skip the last
	 * one, if present, that is smaller than that. So read the remaining bytes.
	 */
	char tmpBuff[2048];
	size_t remaining = fileEntry->FileSize % 2048;
	sr.Read((u8 *) tmpBuff, fileEntry->StartSector + (fileEntry->FileSize + 2047) / 2048 + base / 0x800);
	memcpy(&realBuff[(fileEntry->FileSize / 0x800) * 0x800], tmpBuff, remaining);

	return fileEntry->FileSize;
}
