/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "OptionalHeaders.h"

OptionalHeader::OptionalHeader(OptHeaderKeys key)
{
	ImageKey = key;
}

void ExecutionId::Read(BinaryReaderBE &stream)
{
	stream.BaseStream.Seek(Data, SeekDirection::Begin);
	MediaId = stream.ReadInt32();
	Version = stream.ReadInt32();
	BaseVersion = stream.ReadInt32();
	TitleId = stream.ReadInt32();
	Platform = stream.ReadByte();
	ExecutableType = stream.ReadByte();
	DiscNumber = stream.ReadByte();
	NumberOfDiscs = stream.ReadByte();
	SavegameId = stream.ReadInt32();
}

void GameRatings::Read(BinaryReaderBE &stream)
{
	stream.BaseStream.Seek(Data, SeekDirection::Begin);
	stream.ReadBytes(Ratings, 64);
}

void OriginalPEName::Read(BinaryReaderBE &stream)
{
	stream.BaseStream.Seek(Data, SeekDirection::Begin);
	int size = stream.ReadInt32();
	stream.ReadStringN(Name, size - 4);
}

void BaseFileAddress::Read(BinaryReaderBE &)
{
	BaseAddress = Data;
}

void BaseFileEntryPoint::Read(BinaryReaderBE &)
{
	EntryPoint = Data;
}

void BaseFileChecksumAndTimeStamp::Read(BinaryReaderBE &stream)
{
	stream.BaseStream.Seek(Data, SeekDirection::Begin);
	Checksum = stream.ReadUInt32();
	Timestamp = stream.ReadUInt32();
}

void BaseFileDefaultStackSize::Read(BinaryReaderBE &)
{
	DefaultStackSize = Data;
}

void BaseFileFormat::Read(BinaryReaderBE &stream)
{
	stream.BaseStream.Seek(Data, SeekDirection::Begin);
	InfoSize = stream.ReadInt32();
	EncryptionType = (EncryptionTypes) stream.ReadInt16();
	CompressionType = (CompressionTypes) stream.ReadInt16();

	if (CompressionType == CompressionTypes::NotCompressed)
		RawFormat.Read(stream, InfoSize - 8);
	else
		CompressedFormat.Read(stream);
}

void ImportLibraries::ImportLib::Read(BinaryReaderBE &stream)
{
    Size = stream.ReadInt32();
    stream.ReadBytes(NextImportDigest, 20);
    ID = stream.ReadInt32();
    Version = stream.ReadInt32();
    VersionMin = stream.ReadInt32();
    NameIndex = stream.ReadInt16();
    Count = stream.ReadUInt16();
    ImportTable = new int[Count];
    for (int x = 0; x < Count; x++)
        ImportTable[x] = stream.ReadInt32();
}

void ImportLibraries::Read(BinaryReaderBE &stream)
{
    stream.BaseStream.Seek(Data, SeekDirection::Begin);
    SectionSize = stream.ReadInt32();
    HeaderSize = stream.ReadInt32();
    LibraryCount = stream.ReadInt32();
    LibNames = new std::string[LibraryCount];
        
    for (int x = 0; x < LibraryCount; x++)
    {
		stream.ReadString(LibNames[x]);
		stream.BaseStream.Seek((4 - (stream.BaseStream.Position() % 4)) & 3, SeekDirection::Current);
    }
 
    Libs = new ImportLib[LibraryCount];
    for (int x = 0; x < LibraryCount; x++)
        Libs[x].Read(stream);
}

void StaticLibraries::Library::Read(BinaryReaderBE &stream)
{
	stream.ReadStringN(Name, 8);
	VersionMajor = stream.ReadInt16();
	VersionMinor = stream.ReadInt16();
	VersionBuild = stream.ReadInt16();
	ApprovalType = (ApprovalTypes)(stream.ReadByte() >> 5);
	VersionQfe = stream.ReadByte();
}

void StaticLibraries::Read(BinaryReaderBE &stream)
{
	stream.BaseStream.Seek(Data, SeekDirection::Begin);
	sectionSize = stream.ReadInt32();
	int lcount = (sectionSize - 4) / 0x10;
	Libraries = new Library[lcount];
	for (int x = 0; x < lcount; x++)
		Libraries[x].Read(stream);
}

void Xbox360Logo::Read(BinaryReaderBE &stream)
{
	stream.BaseStream.Seek(Data, SeekDirection::Begin);
	SectionSize = stream.ReadInt32();
	ImageLength = stream.ReadInt32();
	ImageData = new uint8_t[ImageLength];
	stream.ReadBytes(ImageData, ImageLength);
}

void TLSInfo::Read(BinaryReaderBE &stream)
{
	stream.BaseStream.Seek(Data, SeekDirection::Begin);
	NumberOfSlots = stream.ReadInt32();
	RawDataAddress = stream.ReadInt32();
	DataSize = stream.ReadInt32();
	RawDataSize = stream.ReadInt32();
}

void LANKey::Read(BinaryReaderBE &stream)
{
	stream.BaseStream.Seek(Data, SeekDirection::Begin);
	stream.ReadBytes(Key, 16);
}

XexResources::~XexResources()
{
	if (Resources)
		delete[] Resources;
}

void XexResources::Resource::Read(BinaryReaderBE &stream)
{
	stream.ReadStringN(Name, 8);
	Address = stream.ReadInt32();
	Size = stream.ReadInt32();
}

void XexResources::Read(BinaryReaderBE &stream)
{
	stream.BaseStream.Seek(Data, SeekDirection::Begin);
	int length = stream.ReadInt32();
	Count = (length - 4) / 16;
	Resources = new Resource[Count];
	for (int i = 0; i < Count; i++)
		Resources[i].Read(stream);
}
