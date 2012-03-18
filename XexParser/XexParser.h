/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#pragma once

#include <BinaryReaderBE.h>
#include <map>
#include <vector>
#include <stdint.h>

#include "Enums.h"
#include "OptionalHeaders.h"

#ifdef _XEXP_DLL_
#define DLLPREFIX __declspec(dllexport)
#else
#define DLLPREFIX __declspec(dllimport)
#endif

struct XexHeader
{
	u32 Magic;
	ModuleFlag ModuleFlags;
	u32 DataOffset;
	u32 Reserved;
	u32 SecurityInfoOffset;
	u32 OptionalHeaderEntries;

	void Read(BinaryReaderBE &);
};

struct XexSecurityInfo
{
	u32 HeaderLength;
	u32 ImageSize;
	u8 RsaSignature[256];
	u32 Length2;
	ImageFlag ImageFlags;
	u32 LoadAddress;
	u8 SectionDigest[20];
	u32 ImportTableCount;
	u8 ImportTableDigest[20];
	u8 Xgd2MediaId[16];
	u8 AesKey[16];
	u32 ExportTable;
	u8 HeaderDigest[20];
	GameRegion GameRegions;
	AllowedMediaType AllowedMediaTypes;

	void Read(BinaryReaderBE &);
};

struct XexSection
{
	XexSection();

	SectionInfo Info;
	int Size;
	u8 Digest[20];
 
	void Read(BinaryReaderBE &stream);
};

struct XexSectionTable
{
	int SectionCount;
	XexSection *Sections;
	
	XexSectionTable() : Sections(0)
	{
	}

	DLLPREFIX ~XexSectionTable();
	void Read(BinaryReaderBE &stream);
};

class XexParser
{
public:
	DLLPREFIX XexParser(BinaryReaderBE &stream);
	DLLPREFIX void ExtractBaseFile(const char *path);
	
	DLLPREFIX std::vector<std::string> XexParser::GetResourcesList();
	DLLPREFIX void ExtractResource(std::string resName, std::string baseFile, std::string outputPath);

	DLLPREFIX OptionalHeader *GetOptHdr(OptHeaderKeys key);

private:
	/* Parse all the needed structures */
	void Read();

	BinaryReaderBE reader;

	XexHeader Header;
	XexSecurityInfo SecurityInfo;
	XexSectionTable SectionTable;
	std::map<OptHeaderKeys, OptionalHeader *> OptHeaders;

	/* The AES key used to encrypt the base file */
	u8 SessionKey[16];

	void DecryptBaseFile(u8 *buffer);
	void DecompressBaseFile(u8 *buffer, const char *path);
};

#undef DLLPREFIX
