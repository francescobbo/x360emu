/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#pragma once

#include "Enums.h"
#include <BinaryReaderBE.h>
#include <stdint.h>

struct XexVersion
{
	int Major;
	int Minor;
	int Build;
	int Qfe;
 
	XexVersion()
	{
	}

	XexVersion(int version)
	{
		Major = version >> 28;
		Minor = (version >> 24) & 0x0F;
		Build = (version >> 8) & 0xFFFF;
		Qfe = version & 0xFF;
	}
};

class OptionalHeader
{
public:
	OptHeaderKeys ImageKey;
	uint32_t Data;

	OptionalHeader(OptHeaderKeys key);
	virtual void Read(BinaryReaderBE &) {}
};

class ExecutionId : public OptionalHeader
{
public:
	int MediaId;
	XexVersion Version;
	XexVersion BaseVersion;
	int TitleId;
	uint8_t Platform;
	uint8_t ExecutableType;
	uint8_t DiscNumber;
	uint8_t NumberOfDiscs;
	int SavegameId;

	ExecutionId() : OptionalHeader(OptHeaderKeys::ExecutionID)
	{
	}

	void Read(BinaryReaderBE &stream);
};

class GameRatings : public OptionalHeader
{
public:
	// North America
	enum class ESRB
	{
		Everyone = 0x00,
		Everyone10AndOlder = 0x02,
		Teen = 0x04,
		Mature = 0x06,
		RatingPending = 0x08,
		Unrated = 0xFF
	};
 
	// Pan European
	enum class PEGI
	{
		Ages3AndUp = 0x00,
		Ages4AndUp = 0x01,
		Ages5AndUp = 0x02,
		Ages6AndUp = 0x03,
		Ages7AndUp = 0x04,
		Ages8AndUp = 0x05,
		Ages9AndUp = 0x06,
		Ages10AndUp = 0x07,
		Ages11AndUp = 0x08,
		Ages12AndUp = 0x09,
		Ages13AndUp = 0x0A,
		Ages14AndUp = 0x0B,
		Ages15AndUp = 0x0C,
		Ages16AndUp = 0x0D,
		Ages18AndUp = 0x0E,
		Unrated = 0xFF
	};
 
	// Japan
	enum class CERO
	{
		AllAges = 0x00,
		Ages12AndUp = 0x02,
		Ages15AndUp = 0x04,
		Ages17AndUp = 0x06,
		Ages18AndUp = 0x08,
	};
 
	// Germany
	enum class USK
	{
		AllAges = 0x00,
		Ages6AndUp = 0x02,
		Ages12AndUp = 0x04,
		Ages16AndUp = 0x06,
		Ages18AndUp = 0x08,
		Unrated = 0xFF
	};
 
	// Australia & New Zealand
	enum class OFLC
	{
		AllAges = 0x00,
		Ages8AndUp = 0x02,
		Mature = 0x04,
		MatureAccompanied = 0x06,
		Unrated = 0xFF
	};
 
	// Korea
	enum class KMRB
	{
		Unrated = 0xFF
	};
 
	// Brazil
	enum class DJCTQ
	{
		Unrated = 0xFF
	};
 
	enum class FPB
	{
		AllAges = 0x00,
		ParentalGuidance = 0x06,
		Ages10AndUp = 0x07,
		Ages13AndUp = 0x0A,
		Ages16AndUp = 0x0D,
		Ages18AndUp = 0x0E,
		Unrated = 0xFF
	};
 
	uint8_t Ratings[64];

	GameRatings() : OptionalHeader(OptHeaderKeys::GameRatingsSpecified)
	{
	}

	void Read(BinaryReaderBE &stream);
};

class OriginalPEName : public OptionalHeader
{
public:
	std::string Name;
 
	OriginalPEName() : OptionalHeader(OptHeaderKeys::OriginalPEImageName)
	{
	}

	~OriginalPEName();

	void Read(BinaryReaderBE &stream);
};

class BaseFileAddress : public OptionalHeader
{
public:
	uint32_t BaseAddress;
 
	BaseFileAddress() : OptionalHeader(OptHeaderKeys::OriginalBaseAddress)
	{
	}

	void Read(BinaryReaderBE &stream);
};

class BaseFileEntryPoint : public OptionalHeader
{
public:
	uint32_t EntryPoint;
 
	BaseFileEntryPoint() : OptionalHeader(OptHeaderKeys::EntryPoint)
	{
	}

	void Read(BinaryReaderBE &stream);
};

class BaseFileChecksumAndTimeStamp : public OptionalHeader
{
public:
	uint32_t Checksum;
	uint32_t Timestamp;
 
	BaseFileChecksumAndTimeStamp() : OptionalHeader(OptHeaderKeys::ImageChecksumTimestamp)
	{
	}

	void Read(BinaryReaderBE &stream);
};

class BaseFileDefaultStackSize : public OptionalHeader
{
public:
	uint32_t DefaultStackSize;
 
	BaseFileDefaultStackSize() : OptionalHeader(OptHeaderKeys::DefaultStackSize)
	{
	}

	void Read(BinaryReaderBE &stream);
};

class RawBaseFile
{
public:
	struct RawBaseFileBlock
	{
		int DataSize;
		int ZeroSize;

		void Read(BinaryReaderBE &er)
		{
			DataSize = er.ReadInt32();
			ZeroSize = er.ReadInt32();
		}
	};

	RawBaseFileBlock *Blocks;
 
	void Read(BinaryReaderBE &stream, int size)
	{
		Blocks = new RawBaseFileBlock[size / 8];
		for (int x = 0; x < size / 8; x++)
			Blocks[x].Read(stream);
	}
};
 
class CompressedBaseFile
{
public:
	struct CompBaseFileBlock
	{
		int DataSize;
		uint8_t Hash[20];
 
		void Read(BinaryReaderBE &stream)
		{
			DataSize = stream.ReadInt32();
			stream.ReadBytes(Hash, 20);
		}
	};

	int CompressionWindow;
	CompBaseFileBlock Block;

	void Read(BinaryReaderBE &stream)
	{
		CompressionWindow = stream.ReadInt32();
		Block.Read(stream);
	}
};

class BaseFileFormat : public OptionalHeader
{
public:
	enum EncryptionTypes
	{
		NotEncrypted = 0,
		Encrypted = 1
	};
	
	enum CompressionTypes
	{
		NotCompressed = 1,
		Compressed = 2,
		DeltaCompressed = 3
	};

	int InfoSize;
	EncryptionTypes EncryptionType;
	CompressionTypes CompressionType;

	RawBaseFile RawFormat;
	CompressedBaseFile CompressedFormat;

	BaseFileFormat() : OptionalHeader(OptHeaderKeys::BaseFileFormat)
	{
	}

	void Read(BinaryReaderBE &stream);
};

class ImportLibraries : public OptionalHeader
{
public:
    struct ImportLib
    {
        int Size;
        uint8_t NextImportDigest[20];
        int ID;
        XexVersion Version;
        XexVersion VersionMin;
        short NameIndex;
        uint16_t Count;
        int *ImportTable;
 
        void Read(BinaryReaderBE &stream);
    };
 
    int SectionSize;
    int HeaderSize;
    int LibraryCount;
    std::string *LibNames;
    ImportLib *Libs;
 
	ImportLibraries() : OptionalHeader(OptHeaderKeys::ImportLibraries)
	{
	}

	void Read(BinaryReaderBE &stream);
};

class StaticLibraries : public OptionalHeader
{
public:
	struct Library
	{
		enum ApprovalTypes
		{
			Unapproved = 0,
			PossibleApproved,
			Approved,
			Expired
		};
 
		std::string Name;
		short VersionMajor;
		short VersionMinor;
		short VersionBuild;
		ApprovalTypes ApprovalType;
		uint8_t VersionQfe;
 
		void Read(BinaryReaderBE &stream);
	};
 
	int sectionSize;
	Library *Libraries;
 
	StaticLibraries() : OptionalHeader(OptHeaderKeys::StaticLibraries)
	{
	}

	void Read(BinaryReaderBE &stream);
};

class Xbox360Logo : public OptionalHeader
{
public:
	int SectionSize;
	int ImageLength;
	uint8_t *ImageData;

	Xbox360Logo() : OptionalHeader(OptHeaderKeys::IncludesXbox360Logo)
	{
	}

	void Read(BinaryReaderBE &stream);
};

class TLSInfo : public OptionalHeader
{
public:
	int NumberOfSlots;
	int DataSize;
	int RawDataAddress;
	int RawDataSize;

	TLSInfo() : OptionalHeader(OptHeaderKeys::TLSInfo)
	{
	}

	void Read(BinaryReaderBE &stream);
};

class LANKey : public OptionalHeader
{
public:
	uint8_t Key[16];
 
	LANKey() : OptionalHeader(OptHeaderKeys::LANKey)
	{
	}

	void Read(BinaryReaderBE &stream);
};

class XexResources : public OptionalHeader
{
public:
	struct Resource
	{
		std::string Name;
		int Address;
		int Size;

		void Read(BinaryReaderBE &stream);
	};

	XexResources() : OptionalHeader(OptHeaderKeys::ResourceInfo), Resources(0)
	{
	}

	~XexResources();

	Resource *Resources;
	int Count;

	void Read(BinaryReaderBE &stream);
};
