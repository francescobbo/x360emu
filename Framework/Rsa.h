/*
 * Adapted from rijndael-alg-fst.c 3.0 made available in the public domain from:
 * Vincent Rijmen <vincent.rijmen@esat.kuleuven.ac.be>
 * Antoon Bosselaers <antoon.bosselaers@esat.kuleuven.ac.be>
 * Paulo Barreto <paulo.barreto@terra.com.br>
 *
 * You can find the original at: http://www.cs.ucdavis.edu/~rogaway/ocb/ocb-ref/
 */

#pragma once

#include <Types.h>

#ifdef _FMW_DLL_
#define DLLPREFIX __declspec(dllexport)
#else
#define DLLPREFIX __declspec(dllimport)
#endif

#define MAXKC	(256 / 32)
#define MAXKB	(256 / 8)
#define MAXNR	14

class Rsa
{
public:
	enum Kind
	{
		Normal, Cbc
	};

	DLLPREFIX Rsa(Kind k = Normal);

	DLLPREFIX void SetupKeyDec(const u8 key[], int keyBits);
	DLLPREFIX void SetupIv(const u8 iv[]);
	DLLPREFIX void Decrypt(const u8 in[16], u8 out[16]);

private:
	void SetupKeyEnc(const u8 cipherKey[], int keyBits);

	int Nr;
	u32 Key[4 * (MAXNR + 1)];
	u8 Iv[16];

	bool Ready;
	Kind kind;
};

#undef DLLPREFIX
