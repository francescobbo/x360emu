/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "XexParser.h"

#include <Rsa.h>
#include <Sha1.h>
#include <Lzx.h>
#include <FileStream.h>
#include <MemoryStream.h>

#include <iostream>
#include <math.h>
#include <Windows.h>
#include <winnt.h>

using namespace std;

/* Found this on the web. Hope this is not illegal... */
const u8 RetailKey[16] = { 0x20, 0xB1, 0x85, 0xA5, 0x9D, 0x28, 0xFD, 0xC3, 0x40, 0x58, 0x3F, 0xBB, 0x08, 0x96, 0xBF, 0x91 };

OptionalHeader *OptHdrFactory(OptHeaderKeys key)
{
    switch (key)
    {
    case OptHeaderKeys::ExecutionID:
        return new ExecutionId;
    case OptHeaderKeys::GameRatingsSpecified:
        return new GameRatings;
    case OptHeaderKeys::OriginalPEImageName:
        return new OriginalPEName;
    case OptHeaderKeys::OriginalBaseAddress:
    case OptHeaderKeys::ImageBaseAddress:
        return new BaseFileAddress;
    case OptHeaderKeys::EntryPoint:
        return new BaseFileEntryPoint;
    case OptHeaderKeys::ImageChecksumTimestamp:
        return new BaseFileChecksumAndTimeStamp;
    case OptHeaderKeys::DefaultStackSize:
        return new BaseFileDefaultStackSize;
    case OptHeaderKeys::BaseFileFormat:
        return new BaseFileFormat;
    case OptHeaderKeys::ImportLibraries:
        return new ImportLibraries;
    case OptHeaderKeys::StaticLibraries:
        return new StaticLibraries;
    case OptHeaderKeys::IncludesXbox360Logo:
        return new Xbox360Logo;
    case OptHeaderKeys::TLSInfo:
        return new TLSInfo;
    case OptHeaderKeys::LANKey:
        return new LANKey;
    case OptHeaderKeys::ResourceInfo:
        return new XexResources;
    default:
        return new OptionalHeader(key);
    }
}

XexParser::XexParser(BinaryReaderBE &stream) : reader(stream)
{
    if (!reader.BaseStream.CanRead())
        throw ArgumentException("XexParser: The XEX stream can't be read!");

    Read();
}

void XexParser::Read()
{
    /* Seek to beginning */
    reader.BaseStream.Seek(0, SeekDirection::Begin);

    Header.Read(reader);

    /* Read all the optional headers */
    for (u32 x = 0; x < Header.OptionalHeaderEntries; x++)
    {
        OptHeaderKeys key = (OptHeaderKeys) reader.ReadUInt32();
        u32 data = reader.ReadUInt32();

        /* Get the OptionalHeader subclass that suits the key */
        OptionalHeader *optHeader = OptHdrFactory(key);
        optHeader->Data = data;

        /* Insert the optional header in the map */
        OptHeaders.insert(std::pair<OptHeaderKeys, OptionalHeader *>(key, optHeader));
    }

    reader.BaseStream.Seek(Header.SecurityInfoOffset, SeekDirection::Begin);
    SecurityInfo.Read(reader);
    SectionTable.Read(reader);

    /* Ask each OptionalHeader to read whatever they need */
    for each(std::pair<OptHeaderKeys, OptionalHeader *> c in OptHeaders)
        c.second->Read(reader);

    /* If the BaseFile is encrypted, obtain the decryption key */
    BaseFileFormat *bff = (BaseFileFormat *) GetOptHdr(OptHeaderKeys::BaseFileFormat);
    if (bff != NULL && bff->EncryptionType == bff->Encrypted)
    {
        /*
         * A little explaination is in order...
         * XEX base files are encrypted using a Session Key. The Session Key 
         * is encrypted too, using the Retail Key. The result is stored in the
         * XEX as the File Key.
         *
         * So what? We use the Retail Key, to get a Session Key from the File
         * Key (SecurityInfo.FileKey)
         *
         * Once we got our decrypted Session key, it can be used to decrypt the
         * Base file.
         *
         * However there can be two "Retail Key"s. The second is the Devkey made
         * of all 0s. Actually I don't know how to check wheter a XEX was
         * encrypted using the DevKey or the Retail Key.
         * TODO: detect devkit XEXs and decrypt them correctly.
         */

        u8 FileKey[16];
        memcpy(FileKey, &SecurityInfo.AesKey, 16);

        Rsa rsa;
        rsa.SetupKeyDec(RetailKey, 128);
        rsa.Decrypt(FileKey, SessionKey);
    }
}

void XexHeader::Read(BinaryReaderBE &stream)
{
    /* Ensure this is a XEX2 */
    if ((Magic = stream.ReadUInt32()) != 0x58455832)
        throw "Invalid XEX Header magic";

    ModuleFlags = (ModuleFlag) stream.ReadInt32();
    DataOffset = stream.ReadUInt32();
    Reserved = stream.ReadUInt32();
    SecurityInfoOffset = stream.ReadUInt32();
    OptionalHeaderEntries = stream.ReadUInt32();
}

void XexSecurityInfo::Read(BinaryReaderBE &stream)
{
    HeaderLength = stream.ReadUInt32();
    ImageSize = stream.ReadUInt32();
    stream.ReadBytes(RsaSignature, 256);
    Length2 = stream.ReadUInt32();
    ImageFlags = (ImageFlag) stream.ReadUInt32();
    LoadAddress = stream.ReadUInt32();
    stream.ReadBytes(SectionDigest, 20);
    ImportTableCount = stream.ReadUInt32();
    stream.ReadBytes(ImportTableDigest, 20);
    stream.ReadBytes(Xgd2MediaId, 16);
    stream.ReadBytes(AesKey, 16);
    ExportTable = stream.ReadUInt32();
    stream.ReadBytes(HeaderDigest, 20);
    GameRegions = (GameRegion) stream.ReadInt32();
    AllowedMediaTypes = (AllowedMediaType) stream.ReadInt32();
}

XexSection::XexSection()
{
}

void XexSection::Read(BinaryReaderBE &stream)
{
    int temp = stream.ReadInt32();
    Info = (SectionInfo) (temp & 0x0F);
    Size = temp >> 4;
    stream.ReadBytes(Digest, 20);
}

XexSectionTable::~XexSectionTable()
{
    if (Sections)
        delete[] Sections;
}

void XexSectionTable::Read(BinaryReaderBE &stream)
{
    SectionCount = stream.ReadInt32();
    Sections = new XexSection[SectionCount];

    for (int x = 0; x < SectionCount; x++)
        Sections[x].Read(stream);
}

void XexParser::ExtractBaseFile(const char *path)
{
    u64 fs = reader.BaseStream.Length();
    u8 *buffer = new u8[fs];

    /* Read the BaseFile (that can be both encrypted and/or compressed) */
    reader.BaseStream.Seek(Header.DataOffset, SeekDirection::Begin);
    reader.ReadBytes(buffer, fs - Header.DataOffset);

    /* Decrypt the base file (if encrypted). The encrypted one is overwritten. */
    DecryptBaseFile(buffer);

    /* Decompress it using one of the three methods */
    DecompressBaseFile(buffer, path);

    /* Since the decompression outputs to a file, we can delete the buffer */
    delete[] buffer;
}

void XexParser::DecryptBaseFile(u8 *buffer)
{
    if (!buffer)
        throw ArgumentNullException("XexParser::DecryptBaseFile: Buffer is NULL.");

    /* If the BaseFileFormat header is not found, assume not-encrypted */
    BaseFileFormat *bff = (BaseFileFormat *) GetOptHdr(OptHeaderKeys::BaseFileFormat);
    if (!bff)
        return;

    /* Check that we're really encrypted */
    if (bff->EncryptionType != bff->Encrypted)
        return;

    /* Prepare the key */
    Rsa rsa(Rsa::Cbc);
    rsa.SetupKeyDec(SessionKey, 128);

    /* We're using RSA-CBC, so the Initialization Vector is used. For the first round use all 0s */
    u8 iv[16];
    memset(iv, 0, 16);
    rsa.SetupIv(iv);

    /* Count how many blocks have to be decrypted */
    u64 blocks = (reader.BaseStream.Length() - Header.DataOffset) / 16;
    for (u64 i = 0; i < blocks; i++)
    {
        u8 tmp[16];
        rsa.Decrypt(&buffer[i * 16], tmp);

        /* Copy results back, overwriting the encrypted block */
        memcpy(&buffer[i * 16], tmp, 16);
    }
}

void XexParser::DecompressBaseFile(u8 *buffer, const char *path)
{
    /* If the BaseFileFormat header is not found, assume that the BaseFile is raw copied. (TODO: check this assumption) */
    BaseFileFormat *bff = (BaseFileFormat *) GetOptHdr(OptHeaderKeys::BaseFileFormat);
    if (!bff)
    {
        FileStream f(path, FileMode::Create, FileAccess::Write);
        f.Write(buffer, reader.BaseStream.Length() - Header.DataOffset);
    }

    if (bff->CompressionType == bff->DeltaCompressed)
    {
        /* I don't really know what it is... */
        throw "Sorry, but Delta decompression is still TODO!";
    }
    else if (bff->CompressionType == bff->NotCompressed)
    {
        /* Well this is really compressed, but using a rather dummy algorithm. */
        RawBaseFile *rawBaseFile = &bff->RawFormat;
        FileStream f(path, FileMode::Create, FileAccess::Write);
        
        for (int i = 0; i < rawBaseFile->NBlocks; i++)
        {
            u8 *zeros = new u8[rawBaseFile->Blocks[i].ZeroSize];
            memset(zeros, 0, rawBaseFile->Blocks[i].ZeroSize);

            f.Write(buffer, rawBaseFile->Blocks[i].DataSize);
            f.Write(zeros, rawBaseFile->Blocks[i].ZeroSize);

            buffer += rawBaseFile->Blocks[i].DataSize;
            delete[] zeros;
        }
    }
    else
    {
        u8 *lzxStream = new u8[reader.BaseStream.Length() - Header.DataOffset];
        int lzxPtr = 0;

        /**
         * The compressed file is split up in blocks of @blockSize bytes.
         * Each block is made like this:
         *      0 - 3: size of the next block (big endian)
         *      4 - 24: hash of the next block
         *      following the various pieces
         * Each piece is made like this:
         *      0 - 2: Size of this piece, or zero, if we're done
         *      4 - end: lzx compressed data
         *
         * So first of all, let's remove all those headers, and
         * create a continuos stream of compressed data (lzxStream).
         */

        /* The hash and block size of the first block are kept in the optional header. */
        u8 knownDigest[20];
        memcpy(knownDigest, bff->CompressedFormat.Block.Hash, 20);
        u32 blockSize = bff->CompressedFormat.Block.DataSize;

        Sha1 s;

        while (blockSize)
        {
            u8 *blockBuf = buffer;
            buffer += blockSize;

            /* This is quite slow, and may even be useless... */
            u8 digest[20];
            s.Input(blockBuf, blockSize);
            s.Result((unsigned *) digest);
            s.Reset();

            if (memcmp(knownDigest, digest, 20) != 0)
            {
                delete[] lzxStream;
                throw "Corrupted compressed block!";
            }

            /* Read the next block size & SHA1 hash */
            blockSize = _byteswap_ulong(*((unsigned long *) blockBuf));
            memcpy(knownDigest, blockBuf + 4, 20);

            /* Read the first piece's size */
            blockBuf += 24;
            int slen = _byteswap_ushort(*((short *) blockBuf));

            /* A slen of 0 means "End of Block". Now copy the piece into the lzxStream */
            while (slen != 0)
            {
                memcpy(&lzxStream[lzxPtr], &blockBuf[2], slen);
                lzxPtr += slen;

                /* Advance to the next piece */
                blockBuf = &blockBuf[slen + 2];

                /* Get the new piece's size (or 0) */
                slen = _byteswap_ushort(*((short *) blockBuf));
            }

            /*
             * WARNING: the last piece won't always end EXACTLY at the end of 
             * the block. Blocks might infact contain some padding.
             */
        }
        
        /* Now do the real LZX decompression */
        MemoryStream lzxinput((u8 *) lzxStream, lzxPtr);
        FileStream lzxoutput(path, FileMode::Create, FileAccess::Write);

        Lzx lzxd(&lzxinput, &lzxoutput, 15, (int) lzxPtr, SecurityInfo.ImageSize);
        lzxd.Decompress(SecurityInfo.ImageSize);
        
        delete[] lzxStream;
    }   
}

std::vector<std::string> XexParser::GetResourcesList()
{
    std::vector<std::string> ret;

    XexResources *res = (XexResources *) GetOptHdr(OptHeaderKeys::ResourceInfo);
    if (!res)
        return ret;

    for (int i = 0; i < res->Count; i++)
        ret.push_back(res->Resources[i].Name);
    
    return ret;
}

void XexParser::ExtractResource(std::string resName, std::string baseFile, std::string outputPath)
{
    XexResources *res = (XexResources *) GetOptHdr(OptHeaderKeys::ResourceInfo);
    if (!res)
        throw Exception("There are no resources here!");

    BaseFileAddress *bfa = (BaseFileAddress *) GetOptHdr(OptHeaderKeys::ImageBaseAddress);
    if (!bfa)
        throw Exception("Base file address not present. I can't extract resources!");

    for (int i = 0; i < res->Count; i++)
    {
        if (!res->Resources[i].Name.compare(resName))
        {
            /**
             * The Xbox PE cheats. The PointerToRawData is invalid in all sections.
             * The real PointerToRawData is the same as VirtualAddress.
             * This means that if you want to know what's at 0xYYYYYYYY, you
             * can just subtract the BaseAddress, and you'll have the pointer
             * in the file to that data.
             *
             * So we can cheat too. Instead of searching in all sections the
             * one containing the Resource file we're looking for, just jump
             * to the offset and read.
             */

            u32 Address = res->Resources[i].Address - bfa->BaseAddress;
            u32 Length = res->Resources[i].Size;
            
            /* Since now on, assume that the baseFile is a VALID PE32 executable */

            FileStream in(baseFile, FileMode::Open, FileAccess::Read);
            FileStream out(outputPath, FileMode::Create, FileAccess::Write);
            in.Seek(Address, SeekDirection::Begin);
            
            u8 *tmp = new u8[Length];
            in.Read(tmp, Length);
            out.Write(tmp, Length);

            delete[] tmp;
            return;
        }
    }

    throw Exception("XexParser: The resource specified does not exist!");
}

OptionalHeader *XexParser::GetOptHdr(OptHeaderKeys key)
{
    auto find = OptHeaders.find(key);
    if (find != OptHeaders.end())
        return (*find).second;
    return NULL;
}
