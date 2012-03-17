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
	DLLPREFIX void DumpDirectory(u64 sector, u64 size, bool recursive = false);
	DLLPREFIX void DumpEntry(u8 *DirBase, u8 *buffer, bool recursive = false);

	DLLPREFIX void ExtractAllFiles(char *path);
	DLLPREFIX void ExtractDirectory(char *base, char *path, u64 sector, u64 size, bool recursive = false);
	DLLPREFIX void ExtractEntry(char *base, u8 *dirBase, u8 *buffer, bool recursive = false);

	DLLPREFIX void ExtractOne(char *destPath, char *origPath);
	DLLPREFIX u64 ReadOne(char *path, char **buffer);

private:
	SectorReader sr;
	u64 base;
	u32 RootDirectorySector;
	u32 RootDirectorySize;

	EntryHeader *Find(const char *path);

	EntryHeader *FindSubdir(std::string dirName, EntryHeader *parent);
	EntryHeader *FindFile(std::string dirName, EntryHeader *parent);
	EntryHeader *ReadDirectory(EntryHeader *dir);

	void DoExtract(const char *dest, EntryHeader *entry);
};

#undef DLLPREFIX
