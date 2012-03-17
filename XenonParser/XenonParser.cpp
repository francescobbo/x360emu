#include <Types.h>
#include "XenonParser.h"
#include <stddef.h>

void NoHndlr(void *)
{
	throw "Handler for this opcode was not installed.";
}

typedef void (*handler)(void *param);

InternalOp IsNormalOp[64] =
{
	&XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp,
	&XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp,
	&XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::Op19, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp,
	&XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::Op30, &XenonParser::Op31,
	&XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp,
	&XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp,
	&XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::NormalOp,
	&XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::Op58, &XenonParser::Op59, &XenonParser::NormalOp, &XenonParser::NormalOp, &XenonParser::Op62, &XenonParser::Op63
};

XenonParser::XenonParser() : param((void *) 0)
{
	NormalOps = Sub19 = Sub30 = Sub31 = Sub58 = Sub59 = Sub62 = Sub63 = NULL;
}

XenonParser::XenonParser(void *param) : param(param), IsOwner(true)
{
	InitTables();
}

XenonParser::XenonParser(const XenonParser &copy, void *param) : param(param), IsOwner(false)
{
	CopyTables(copy);
}

XenonParser::~XenonParser()
{
	if (IsOwner)
	{
		delete[] Sub19;
		delete[] Sub30;
		delete[] Sub31;
		delete[] Sub58;
		delete[] Sub59;
		delete[] Sub62;
		delete[] Sub63;
	}
}

void XenonParser::InitTables()
{
	NormalOps = new handler[64];
	Sub19 = new handler[529];
	Sub30 = new handler[10];
	Sub31 = new handler[1015];
	Sub58 = new handler[3];
	Sub59 = new handler[35];
	Sub62 = new handler[2];
	Sub63 = new handler[847];

	for (int i = 0; i < 64; i++)
		NormalOps[i] = NoHndlr;

	for (int i = 0; i < 529; i++)
		Sub19[i] = NoHndlr;

	for (int i = 0; i < 10; i++)
		Sub30[i] = NoHndlr;

	for (int i = 0; i < 1015; i++)
		Sub31[i] = NoHndlr;

	for (int i = 0; i < 3; i++)
		Sub58[i] = NoHndlr;

	for (int i = 0; i < 35; i++)
		Sub59[i] = NoHndlr;

	for (int i = 0; i < 2; i++)
		Sub62[i] = NoHndlr;

	for (int i = 0; i < 847; i++)
		Sub63[i] = NoHndlr;
}

void XenonParser::CopyTables(const XenonParser &copy)
{
	NormalOps = copy.NormalOps;
	Sub19 = copy.Sub19;
	Sub30 = copy.Sub30;
	Sub31 = copy.Sub31;
	Sub58 = copy.Sub58;
	Sub59 = copy.Sub59;
	Sub62 = copy.Sub62;
	Sub63 = copy.Sub63;
}

void XenonParser::SetParam(void *p)
{
	param = p;
}

void XenonParser::Register(Mnemonics Instr, handler Handler)
{
	if (!NormalOps)
		InitTables();

	switch (Instr & 0x3F)
	{
	case 19:
		Sub19[(Instr >> 8) & 0x3FF] = Handler;
		break;
	case 30:
		Sub30[(Instr >> 8) & 0x1F] = Handler;
		break;
	case 31:
		Sub31[(Instr >> 8) & 0x3FF] = Handler;
		break;
	case 58:
		Sub58[(Instr >> 8) & 3] = Handler;
		break;
	case 59:
		Sub59[(Instr >> 8) & 0x3FF] = Handler;
		break;
	case 62:
		Sub62[(Instr >> 8) & 3] = Handler;
		break;
	case 63:
		Sub63[(Instr >> 8) & 0x3FF] = Handler;
		break;
	default:
		NormalOps[Instr & 0x3F] = Handler;
		break;
	}
}

void XenonParser::Parse(u32 instruction)
{
	curInstr = instruction;
	(this->*(IsNormalOp[instruction >> 26]))(param);
}

void XenonParser::NormalOp(void *p)
{
	NormalOps[curInstr >> 26](p);
}

void XenonParser::Op19(void *p)
{
	Sub19[(curInstr >> 1) & 0x3FF](p);
}

void XenonParser::Op30(void *p)
{
	u32 secOp = (curInstr >> 1) & 0x1F;
	if (!(secOp & 0x10))
		secOp >>= 1;

	Sub30[secOp](p);
}

void XenonParser::Op31(void *p)
{
	Sub31[(curInstr >> 1) & 0x3FF](p);
}

void XenonParser::Op58(void *p)
{
	Sub58[curInstr & 3](p);
}

void XenonParser::Op59(void *p)
{
	u32 secOp = (curInstr >> 1) & 0x3F;

	/*
	 * The bit n. 4 of the secondary opcode (the n. 5 of the instruction) tells us that the secondary opcode is only 5 bits wide, and not 10.
	 * The remaining bits are used to encode an additional register C
	 */ 
	if (((secOp >> 4) & 1) == 1)
		secOp &= 0x1F;

	Sub59[secOp](p);
}

void XenonParser::Op62(void *p)
{
	Sub62[curInstr & 1](p);
}

void XenonParser::Op63(void *p)
{
	u32 secOp = (curInstr >> 1) & 0x3FF;

	/*
	 * The bit n. 4 of the secondary opcode (the n. 5 of the instruction) tells us that the secondary opcode is only 5 bits wide, and not 10.
	 * The remaining bits are used to encode an additional register C
	 */ 
	if (((secOp >> 4) & 1) == 1)
		secOp &= 0x1F;

	Sub63[secOp](p);
}
