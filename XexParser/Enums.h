/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#pragma once

#include <stdint.h>

enum class OptHeaderKeys
{
	ResourceInfo = 0x000002FF,
	BaseFileFormat = 0x000003FF,
	DeltaPatchDescriptor = 0x000005FF,
	BaseReference = 0x00000405,

	OriginalBaseAddress = 0x00010001,
	EntryPoint = 0x00010100,
	ImageBaseAddress = 0x00010201,
	ImportLibraries = 0x000103FF,
	ImageChecksumTimestamp = 0x00018002,
	EnabledForCallcap = 0x00018102,
	EnabledForFastcap = 0x00018200,
	OriginalPEImageName = 0x000183FF,

	StaticLibraries = 0x000200FF,
	TLSInfo = 0x00020104,
	DefaultStackSize = 0x00020200,
	DefaultFilesystemCacheSize = 0x00020301,
	DefaultHeapSize = 0x00020401,
	PageHeapSizeAndflags = 0x00028002,

	SystemFlags = 0x00030000,

	ExecutionID = 0x00040006,
	TitleWorkspaceSize = 0x00040201,
	GameRatingsSpecified = 0x00040310,
	LANKey = 0x00040404,
	IncludesXbox360Logo = 0x000405FF,
	MultidiscMediaIDs = 0x000406FF,
	AlternateTitleIDs = 0x000407FF,
	AdditionalTitleMemory = 0x00040801,

	BoundingPath = 0x000080FF,
	DeviceId = 0x00008105,

	IncludesExportsByName = 0x00E10402
};

enum class ImageFlag
{
	RevocationCheckRequired = 0x01,
 
	ManufacturingUtility = 0x02,
	ManufacturingSupportTool = 0x04,
	ManufacturingAwareModule = ManufacturingUtility | ManufacturingSupportTool,
 	Xgd2MediaOnly = 0x08,
	CardeaKey = 0x100,
	XeikaKey = 0x200,
	TitleUserMode = 0x400,
	SystemUserMode = 0x800,
	Orange0 = 0x1000,
	Orange1 = 0x2000,
	Orange2 = 0x4000,
	IptvSignupApplication = 0x10000,
	IptvTitleApplication = 0x20000,
	KeyVaultPrivilegesRequired = 0x04000000,
	OnlineActivationRequired = 0x08000000,
	PageSize4Kb = 0x10000000,	// Either 4KB or 64KB
	NoGameRegion = 0x20000000,
	RevocationCheckOptional = 0x40000000
};

enum class ModuleFlag
{
	TitleModule = 0x00000001,
	ExportsToTitle = 0x00000002,
	SystemDebugger = 0x00000004,
	DllModule = 0x00000008,
	ModulePatch = 0x00000010,
	PatchFull = 0x00000020,
	PatchDelta = 0x00000040,
	UserMode = 0x00000080
};

enum class GameRegion : uint32_t
{
	NorthAmerica = 0xFF,
	Japan = 0x100,
	China = 0x200,
	RestOfAsia = 0xFC00,
	AustraliaNewZealand = 0x10000,
	RestOfEurope = 0xFE0000,
	RestOfWord = 0xFF000000,
	AllRegions = 0xFFFFFFFF
};

enum class AllowedMediaType
{
	HardDisk = 0x00000001,
	DvdX2 = 0x00000002,
	DvdCd = 0x00000004,
	Dvd5 = 0x00000008,
	Dvd9 = 0x00000010,
	SystemFlash = 0x00000020,
	MemoryUnit = 0x00000080,
	MassStorageDevice = 0x00000100,
	SmbFilesystem = 0x00000200,
	DirectFromRam = 0x00000400,
	SecureVirtualOpticalDevice = 0x00001000,
	WirelessNStorageDevice = 0x00002000,
	InsecurePackage = 0x01000000,
	SaveGamePackage = 0x02000000,
	LocallySignedPackage = 0x04000000,
	LiveSignedPackage = 0x08000000,
	XboxPlatformPackage = 0x10000000
};

enum class SectionInfo
{
	Code = 0x00000001,
	Data = 0x00000002,
	ReadOnly = 0x00000003
};
