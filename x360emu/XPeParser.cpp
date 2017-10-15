/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "XPeParser.h"
#include "Types.h"
#include "Memory.h"
#include "Xenon/Xenon.h"
#include <fstream>
#include <stdint.h>
#include <iostream>

using namespace std;

XPeParser::XPeParser(const char *path, XexParser &xex) : xex(xex)
{
	PePlus = false;

	file.open(path, ios::in | ios::binary);
	if (!file.good())
		throw "PE file is unreadable!";

	/* Check the main signature */
	uint16_t dosSig;
	file.read((char *) &dosSig, 2);
	if (dosSig != IMAGE_DOS_SIGNATURE)
		throw "Invalid DOS Signature for the executable!";

	/* Seek to the PE header */
	file.seekg(0x3C, ios::beg);
	uint32_t peOffset;
	file.read((char *) &peOffset, 4);

	/* Check the PE signature */
	file.seekg(peOffset, ios::beg);
	uint32_t peSign;
	file.read((char *) &peSign, 4);
	if (peSign != IMAGE_NT_SIGNATURE)
		throw "Invalid PE Signature for the executable!";

	/* Read the rest of PE header, then check some fields */
	file.read((char *) &ImageHdr, sizeof(IMAGE_FILE_HEADER));

	/* Specific machine value to indicate Xbox 360 */
	if (ImageHdr.Machine != 0x1F2)
		throw "The PE is valid but it's not an Xbox 360 executable.";

	/*
	 * The strange thing is that imHdr.Characteristics is set to 0x102
	 * (usually), indicating a 32bit architecture, while the Xenon is 64-bit.
	 * Microsoft likes to mess with their own specifications...
	 *
	 * Now read the optional header in an union and detect wheter it's PE32 or
	 * PE32+, then seek back and forth again to syncronize the end of structure.
	 */

	uint64_t oldPos = file.tellg();

	file.read((char *) &OptionalHdr, ImageHdr.SizeOfOptionalHeader);

	if (OptionalHdr.hdr32.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC && OptionalHdr.hdr32.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		throw "The optional header has an invalid signature!";

	if (OptionalHdr.hdr32.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		PePlus = true;

	/* Read the section headers */
	SectHeaders = new IMAGE_SECTION_HEADER[ImageHdr.NumberOfSections];
	file.read((char *) SectHeaders, sizeof(IMAGE_SECTION_HEADER) * ImageHdr.NumberOfSections);
}

XPeParser::~XPeParser()
{
	if (SectHeaders)
		delete[] SectHeaders;
}

uint32_t XPeParser::RvaToOffset(uint32_t rva)
{
	for (int i = 0; i < ImageHdr.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER &hdr = SectHeaders[i];
		if (hdr.VirtualAddress <= rva && (hdr.VirtualAddress + hdr.Misc.VirtualSize) > rva)
			return rva - hdr.VirtualAddress + hdr.PointerToRawData;
	}

	throw "Invalid RVA";
}

void XPeParser::Load()
{
	uint64_t ImageBase = PePlus ? OptionalHdr.hdr64.ImageBase : OptionalHdr.hdr32.ImageBase;

	BaseFileAddress *bfa = (BaseFileAddress *) xex.GetOptHdr(OptHeaderKeys::ImageBaseAddress);
	if (bfa)
		ImageBase = bfa->BaseAddress;

	for (int i = 0; i < ImageHdr.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER &hdr = SectHeaders[i];

		if (hdr.VirtualAddress)
		{
			u8 *pointer = Memory::GetPointer((u32) (ImageBase + hdr.VirtualAddress));

			if (hdr.SizeOfRawData)
			{
				/*
				 * For some strange bug in the Xbox 360 linker (or maybe this is
				 * wanted), the PointerToRawData is screwed up. The pointer to data
				 * in the file is the same as it's VirtualAddress
				 */

				file.seekg(hdr.VirtualAddress);

				/* VirtualSize might be less than SizeOfRawData, in that case read only VirtualSize bytes */
				if (hdr.Misc.VirtualSize <= hdr.SizeOfRawData)
					file.read((char *) pointer, hdr.Misc.VirtualSize);
				else
				{
					file.read((char *) pointer, hdr.SizeOfRawData);

					/* Pad to 0 remaining bytes */
					memset(pointer + hdr.SizeOfRawData, 0, hdr.Misc.VirtualSize - hdr.SizeOfRawData);
				}
			}
			else
			{
				/* Some kind of .bss section (uninitialized data) */
				memset(pointer, 0, hdr.Misc.VirtualSize);
			}
		}
	}

	uint32_t EP = PePlus ? OptionalHdr.hdr64.AddressOfEntryPoint : OptionalHdr.hdr32.AddressOfEntryPoint;

	BaseFileEntryPoint *bfep = (BaseFileEntryPoint *) xex.GetOptHdr(OptHeaderKeys::EntryPoint);
	if (bfep)
		EP = bfep->EntryPoint;

	Xenon::xState[0].pc = EP;
}
