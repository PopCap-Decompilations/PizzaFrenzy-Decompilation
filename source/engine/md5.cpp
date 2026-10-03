// RSA Data Security's MD5 message digest (the old md5.c: the digest is kept in the context), as the engine uses it
// for the .saf archive. Sprout changed only MD5Init, which takes a seed that offsets the initial state.
#include "md5.h"

// Padding: a 1 bit, then zeros
static unsigned char PADDING[64] = {
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// F, G, H and I are the basic MD5 functions: selection, majority, parity
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

// ROTATE_LEFT rotates x left n bits
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

// FF, GG, HH and II are the transformations of rounds 1, 2, 3 and 4 (the rotation is separate from the addition to
// prevent recomputation)
#define FF(a, b, c, d, x, s, ac) \
	{(a) += F((b), (c), (d)) + (x) + (UINT4)(ac); \
	 (a) = ROTATE_LEFT((a), (s)); \
	 (a) += (b); \
	}
#define GG(a, b, c, d, x, s, ac) \
	{(a) += G((b), (c), (d)) + (x) + (UINT4)(ac); \
	 (a) = ROTATE_LEFT((a), (s)); \
	 (a) += (b); \
	}
#define HH(a, b, c, d, x, s, ac) \
	{(a) += H((b), (c), (d)) + (x) + (UINT4)(ac); \
	 (a) = ROTATE_LEFT((a), (s)); \
	 (a) += (b); \
	}
#define II(a, b, c, d, x, s, ac) \
	{(a) += I((b), (c), (d)) + (x) + (UINT4)(ac); \
	 (a) = ROTATE_LEFT((a), (s)); \
	 (a) += (b); \
	}

// 0x49B920: the basic MD5 step, transforms buf based on in
static void MD5Transform(UINT4 buf[4], UINT4 in[16])
{
	UINT4 a = buf[0], b = buf[1], c = buf[2], d = buf[3];

	// Round 1
#define S11 7
#define S12 12
#define S13 17
#define S14 22
	FF(a, b, c, d, in[ 0], S11, 3614090360UL);	// 1
	FF(d, a, b, c, in[ 1], S12, 3905402710UL);	// 2
	FF(c, d, a, b, in[ 2], S13,  606105819UL);	// 3
	FF(b, c, d, a, in[ 3], S14, 3250441966UL);	// 4
	FF(a, b, c, d, in[ 4], S11, 4118548399UL);	// 5
	FF(d, a, b, c, in[ 5], S12, 1200080426UL);	// 6
	FF(c, d, a, b, in[ 6], S13, 2821735955UL);	// 7
	FF(b, c, d, a, in[ 7], S14, 4249261313UL);	// 8
	FF(a, b, c, d, in[ 8], S11, 1770035416UL);	// 9
	FF(d, a, b, c, in[ 9], S12, 2336552879UL);	// 10
	FF(c, d, a, b, in[10], S13, 4294925233UL);	// 11
	FF(b, c, d, a, in[11], S14, 2304563134UL);	// 12
	FF(a, b, c, d, in[12], S11, 1804603682UL);	// 13
	FF(d, a, b, c, in[13], S12, 4254626195UL);	// 14
	FF(c, d, a, b, in[14], S13, 2792965006UL);	// 15
	FF(b, c, d, a, in[15], S14, 1236535329UL);	// 16

	// Round 2
#define S21 5
#define S22 9
#define S23 14
#define S24 20
	GG(a, b, c, d, in[ 1], S21, 4129170786UL);	// 17
	GG(d, a, b, c, in[ 6], S22, 3225465664UL);	// 18
	GG(c, d, a, b, in[11], S23,  643717713UL);	// 19
	GG(b, c, d, a, in[ 0], S24, 3921069994UL);	// 20
	GG(a, b, c, d, in[ 5], S21, 3593408605UL);	// 21
	GG(d, a, b, c, in[10], S22,   38016083UL);	// 22
	GG(c, d, a, b, in[15], S23, 3634488961UL);	// 23
	GG(b, c, d, a, in[ 4], S24, 3889429448UL);	// 24
	GG(a, b, c, d, in[ 9], S21,  568446438UL);	// 25
	GG(d, a, b, c, in[14], S22, 3275163606UL);	// 26
	GG(c, d, a, b, in[ 3], S23, 4107603335UL);	// 27
	GG(b, c, d, a, in[ 8], S24, 1163531501UL);	// 28
	GG(a, b, c, d, in[13], S21, 2850285829UL);	// 29
	GG(d, a, b, c, in[ 2], S22, 4243563512UL);	// 30
	GG(c, d, a, b, in[ 7], S23, 1735328473UL);	// 31
	GG(b, c, d, a, in[12], S24, 2368359562UL);	// 32

	// Round 3
#define S31 4
#define S32 11
#define S33 16
#define S34 23
	HH(a, b, c, d, in[ 5], S31, 4294588738UL);	// 33
	HH(d, a, b, c, in[ 8], S32, 2272392833UL);	// 34
	HH(c, d, a, b, in[11], S33, 1839030562UL);	// 35
	HH(b, c, d, a, in[14], S34, 4259657740UL);	// 36
	HH(a, b, c, d, in[ 1], S31, 2763975236UL);	// 37
	HH(d, a, b, c, in[ 4], S32, 1272893353UL);	// 38
	HH(c, d, a, b, in[ 7], S33, 4139469664UL);	// 39
	HH(b, c, d, a, in[10], S34, 3200236656UL);	// 40
	HH(a, b, c, d, in[13], S31,  681279174UL);	// 41
	HH(d, a, b, c, in[ 0], S32, 3936430074UL);	// 42
	HH(c, d, a, b, in[ 3], S33, 3572445317UL);	// 43
	HH(b, c, d, a, in[ 6], S34,   76029189UL);	// 44
	HH(a, b, c, d, in[ 9], S31, 3654602809UL);	// 45
	HH(d, a, b, c, in[12], S32, 3873151461UL);	// 46
	HH(c, d, a, b, in[15], S33,  530742520UL);	// 47
	HH(b, c, d, a, in[ 2], S34, 3299628645UL);	// 48

	// Round 4
#define S41 6
#define S42 10
#define S43 15
#define S44 21
	II(a, b, c, d, in[ 0], S41, 4096336452UL);	// 49
	II(d, a, b, c, in[ 7], S42, 1126891415UL);	// 50
	II(c, d, a, b, in[14], S43, 2878612391UL);	// 51
	II(b, c, d, a, in[ 5], S44, 4237533241UL);	// 52
	II(a, b, c, d, in[12], S41, 1700485571UL);	// 53
	II(d, a, b, c, in[ 3], S42, 2399980690UL);	// 54
	II(c, d, a, b, in[10], S43, 4293915773UL);	// 55
	II(b, c, d, a, in[ 1], S44, 2240044497UL);	// 56
	II(a, b, c, d, in[ 8], S41, 1873313359UL);	// 57
	II(d, a, b, c, in[15], S42, 4264355552UL);	// 58
	II(c, d, a, b, in[ 6], S43, 2734768916UL);	// 59
	II(b, c, d, a, in[13], S44, 1309151649UL);	// 60
	II(a, b, c, d, in[ 4], S41, 4149444226UL);	// 61
	II(d, a, b, c, in[11], S42, 3174756917UL);	// 62
	II(c, d, a, b, in[ 2], S43,  718787259UL);	// 63
	II(b, c, d, a, in[ 9], S44, 3951481745UL);	// 64

	buf[0] += a;
	buf[1] += b;
	buf[2] += c;
	buf[3] += d;
}

// 0x49C230: the context for a new message; Sprout's seed offsets the magic initial state
void MD5Init(MD5_CTX* context, UINT4 seed)
{
	context->i[0] = context->i[1] = (UINT4)0;

	// load the magic initialization constants
	context->buf[0] = (UINT4)0x67452301 + seed * 11;
	context->buf[1] = (UINT4)0xefcdab89 + seed * 71;
	context->buf[2] = (UINT4)0x98badcfe + seed * 37;
	context->buf[3] = (UINT4)0x10325476 + seed * 97;
}

// 0x49C280
void MD5Update(MD5_CTX* context, const unsigned char* input, unsigned int inputLen)
{
	UINT4 in[16];
	int mdi;
	unsigned int i, ii;

	// compute the number of bytes mod 64
	mdi = (int)((context->i[0] >> 3) & 0x3F);

	// update the number of bits
	if ((context->i[0] + ((UINT4)inputLen << 3)) < context->i[0])
		context->i[1]++;
	context->i[0] += ((UINT4)inputLen << 3);
	context->i[1] += ((UINT4)inputLen >> 29);

	while (inputLen--)
	{
		// add the new character to the buffer, increment mdi
		context->in[mdi++] = *input++;

		// transform if necessary
		if (mdi == 0x40)
		{
			for (i = 0, ii = 0; i < 16; i++, ii += 4)
				in[i] = (((UINT4)context->in[ii + 3]) << 24) |
					(((UINT4)context->in[ii + 2]) << 16) |
					(((UINT4)context->in[ii + 1]) << 8) |
					((UINT4)context->in[ii]);
			MD5Transform(context->buf, in);
			mdi = 0;
		}
	}
}

// 0x49C380: pads the message and leaves the digest in context->digest
void MD5Final(MD5_CTX* context)
{
	UINT4 in[16];
	int mdi;
	unsigned int i, ii;
	unsigned int padLen;

	// save the number of bits
	in[14] = context->i[0];
	in[15] = context->i[1];

	// compute the number of bytes mod 64
	mdi = (int)((context->i[0] >> 3) & 0x3F);

	// pad out to 56 mod 64
	padLen = (mdi < 56) ? (56 - mdi) : (120 - mdi);
	MD5Update(context, PADDING, padLen);

	// append the length in bits and transform
	for (i = 0, ii = 0; i < 14; i++, ii += 4)
		in[i] = (((UINT4)context->in[ii + 3]) << 24) |
			(((UINT4)context->in[ii + 2]) << 16) |
			(((UINT4)context->in[ii + 1]) << 8) |
			((UINT4)context->in[ii]);
	MD5Transform(context->buf, in);

	// store the buffer in the digest
	for (i = 0, ii = 0; i < 4; i++, ii += 4)
	{
		context->digest[ii] = (unsigned char)(context->buf[i] & 0xFF);
		context->digest[ii + 1] = (unsigned char)((context->buf[i] >> 8) & 0xFF);
		context->digest[ii + 2] = (unsigned char)((context->buf[i] >> 16) & 0xFF);
		context->digest[ii + 3] = (unsigned char)((context->buf[i] >> 24) & 0xFF);
	}
}
