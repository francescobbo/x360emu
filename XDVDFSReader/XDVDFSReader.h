#include <Windows.h>
#include <Types.h>
#include "SectorReader.h"

#ifdef _XDVDFS_DLL_
#define DLLPREFIX __declspec(dllexport)
#else
#define DLLPREFIX __declspec(dllimport)
#endif

#pragma pack(1)
struct EntryHeader
{
	u16 LeftOffset;
	u16 RightOffset;
	u32 StartSector;
	u32 FileSize;
	u8 Attributes;
	u8 FileNameLen;
};
#pragma pack()

class XDVDFSReader
{
public:
	DLLPREFIX XDVDFSReader(std::string path, bool isPhys);
	DLLPREFIX void DumpAllFiles();
	DLLPREFIX void DumpDirectory(EntryHeader *dir, bool recursive);
	DLLPREFIX void DumpEntry(u8 *Directory, EntryHeader *entry, bool recursive);

	DLLPREFIX void ExtractAllFiles(const char *path);
	DLLPREFIX void ExtractDirectory(const char *base, const char *path, EntryHeader *entry, bool recursive = false);
	DLLPREFIX void ExtractEntry(const char *base, u8 *Directory, EntryHeader *entry, bool recursive = false);

	DLLPREFIX void ExtractOne(const char *destPath, const char *origPath);
	DLLPREFIX u64 ReadOne(const char *path, u8 **buffer);

private:
	SectorReader sr;
	u64 base;
	u32 RootDirectorySector;
	u32 RootDirectorySize;

	EntryHeader *Find(const char *path);

	void ReadFile(u8 *buffer, EntryHeader *f);

	EntryHeader *Find(std::string dirName, EntryHeader *parent);
	EntryHeader *ReadDirectory(EntryHeader *dir);

	void DoExtract(const char *dest, EntryHeader *entry);
};

#undef DLLPREFIX
