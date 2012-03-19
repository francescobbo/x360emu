/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#pragma once

#include <Types.h>

union XInstruction
{
	u32 hex;

	struct
	{
		u32 Rc		:	1;
		u32 SUBOP10	:	10;
		u32 RB		:	5;
		u32 RA		:	5;
		u32 RD		:	5;
		u32 OPCD	:	6;
	}; // changed
	struct
	{
		signed SIMM_16	:	16;
		u32 			:	5;
		u32 TO			:	5;
		u32 OPCD_2		:	6;
	};
	struct
	{
		u32 Rc_2	:	1;
		u32			:	10;
		u32 		:	5;
		u32 		:	5;
		u32 RS		:	5;
		u32 OPCD_3	:	6;
	};
	struct
	{
		u64 UIMM	:	16;
		u32 		:	5;
		u32 		:	5;
		u32 OPCD_4	:	6;
	};
	struct
	{
		u32 LK		:	1;
		u32 AA		:	1;
		u32 LI		:	24;
		u32 OPCD_5	:	6;
	};
	struct
	{
		u32 LK_2	:	1;
		u32 AA_2	:	1;
		u32 BD		:	14;
		u32 BI		:	5;
		u32 BO		:	5;
		u32 OPCD_6	:	6;
	};
	struct
	{
		u32 LK_3	:	1;
		u32			:	10;
		u32			:	5;
		u32 BI_2	:	5;
		u32 BO_2	:	5;
		u32 OPCD_7	:	6;
	};
	struct
	{
		u32			:	11;
		u32 RB_2	:	5;
		u32 RA_2	:	5;
		u32 L		:	1;
		u32 		:	1;
		u32 CRFD	:	3;
		u32 OPCD_8	:	6;
	};
	struct
	{
		signed  SIMM_16_2	:	16;
		u32 RA_3	:	5;
		u32 L_2		:	1;
		u32			:	1;
		u32 CRFD_2	:	3;
		u32 OPCD_9	:	6;
	};
	struct
	{
		u32 UIMM_2	:	16;
		u32 RA_4	:	5;
		u32 L_3		:	1;
		u32 dummy2	:	1;
		u32 CRFD_3	:	3;
		u32 OPCD_A	:	6;
	};
	struct
	{
		u32			:	1;
		u32 SUBOP10_2:	10;
		u32 RB_5	:	5;
		u32 RA_5	:	5;
		u32 L_4		:	1;
		u32 dummy3	:	1;
		u32 CRFD_4	:	3;
		u32 OPCD_B	:	6;
	};
	struct
	{
		u32			:	16;
		u32 SR		:	4;
		u32 		:	1;
		u32 RS_2	:	5;
		u32 OPCD_C	:	6;
	};

	// Table 59
	struct
	{
		u32 Rc_4	:	1;
		u32 SUBOP5	:	5;
		u32 RC		:	5;
		u32			:	5;
		u32 RA_6	:	5;
		u32 RD_2	:	5;
		u32 OPCD_D	:	6;
	};

	struct
	{	u32			:	10;
		u32 OE		:	1;
		u32 SPR		:	10;
		u32			:	11;
	};
	struct
	{
		u32			:	10;
		u32 OE_3	:	1;
		u32 SPRU	:	5;
		u32 SPRL	:	5;
		u32			:	11;
	};

	// rlwinmx
	struct
	{
		u32 Rc_3	:	1;
		u32 ME		:	5;
		u32 MB		:	5;
		u32 SH		:	5;
		u32			:	16;
	};

	// crxor
	struct
	{
		u32			:	11;
		u32 CRBB	:	5;
		u32 CRBA	:	5;
		u32 CRBD	:	5;
		u32			:	6;
	};

	// mftb
	struct
	{
		u32			:	11;
		u32 TBR		:	10;
		u32			:	11;
	};

	struct
	{
		u32			:	11;
		u32 TBRU	:	5;
		u32 TBRL	:	5;
		u32			:	11;
	};

	struct
	{
		u32			:	18;
		u32 CRFS	:	3;
		u32 		:	2;
		u32 CRFD_5	:	3;
		u32			:	6;
	};

	// float
	struct
	{
		u32			:	12;
		u32 CRM		:	8;
		u32			:	1;
		u32 FD		:	5;
		u32			:	6;
	};
	struct
	{
		u32			:	6;
		u32 FC		:	5;
		u32 FB		:	5;
		u32 FA		:	5;
		u32 FS		:	5;
		u32			:	6;
	};
	struct
	{
		u32 OFS		:	16;
		u32			:	16;
	};
	struct
	{
		u32			:   17;
		u32 FM		:   8;
		u32			:   7;
	};
	/* MTMSRD */
	struct
	{
		u32			:	16;
		u32  L_MSR	:	1;
		u32			:	15;
	};
	struct
	{
		u32 dummyX	:   11;
		u32 NB		: 5;
	};
};
