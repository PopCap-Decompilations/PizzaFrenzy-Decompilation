// RSA Data Security's MD5 (the old md5.c that keeps the digest in the context), with Sprout's keyed MD5Init.
#pragma once

// md5.h's 32-bit unsigned type
typedef unsigned long UINT4;

// md5.h's context (0x68 bytes)
struct MD5_CTX
{
	UINT4 i[2];									// +0x00 number of bits handled, mod 2^64
	UINT4 buf[4];								// +0x08 the state
	unsigned char in[64];						// +0x18 input buffer
	unsigned char digest[16];					// +0x58 the digest, after MD5Final
};

// Sprout's change: the initial state is {0x67452301 + 11*seed, 0xEFCDAB89 + 71*seed, 0x98BADCFE + 37*seed,
// 0x10325476 + 97*seed}; the archive code always passes 0x53414646
void MD5Init(MD5_CTX* context, UINT4 seed);
void MD5Update(MD5_CTX* context, const unsigned char* input, unsigned int inputLen);
// pads the message and stores the digest in context->digest (no output parameter)
void MD5Final(MD5_CTX* context);
