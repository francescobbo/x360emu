/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#pragma once

#include <Windows.h>
#include <winnt.h>
#include <fstream>
#include <XexParser.h>

union OptHdr
{
	IMAGE_OPTIONAL_HEADER32 hdr32;
	IMAGE_OPTIONAL_HEADER64 hdr64;
};

class XPeParser
{
public:
	XPeParser(const char *path, XexParser &xex);
	~XPeParser();

	void Load();

private:
	uint32_t XPeParser::RvaToOffset(uint32_t rva);

	IMAGE_FILE_HEADER ImageHdr;
	OptHdr OptionalHdr;
	IMAGE_SECTION_HEADER *SectHeaders;

	bool PePlus;
	std::fstream file;
	XexParser &xex;
};
