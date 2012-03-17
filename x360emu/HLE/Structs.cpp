#include "HLE.h"

XBOX_HARDWARE_INFO XboxHardwareInfo =
{
	/* Flags */
	0x2000817,
	/* CpuCount */
	6,
	/* PCIBridgeRevisionID */
	0,
	/* Reserved */
	{ 0, 0, 0, 0, 0, 0, },
	/* BootLoaderMagic */
	0,
	/* BootLoaderFlags */
	0
};

/*
 * Just the same structure repeated twice. Do this change at run-time? I don't know...
 * Anyway, here we're impersonating xbxkrnl.exe 2.0.14448
 */
KERNEL_VERSION XboxKrnlVersion = 
{
	2, 0, 14448, 0x80 /* 0x00 */, 0
};

KERNEL_VERSION XboxKrnlBaseVersion = 
{
	2, 0, 14448, 0x80 /* 0x00 */, 0
};

uint32_t VdGpuClockInMHz = 500;