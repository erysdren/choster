// ========================================================
//
// FILE:			/lib_cxx/b64.c
//
// AUTHORS:			Jaycie Ewald
//
// PROJECT:			libcohost
//
// LICENSE:			ACSL v1.4
//
// DESCRIPTION:		Base64 functions
//
// LAST EDITED:		November 26th, 2022
//
// ========================================================

// Standard headers
#include "b64.h"

const char Base64_Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int Base64_Invs[] = {
	62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58,
	59, 60, 61, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
	21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
	29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
	43, 44, 45, 46, 47, 48, 49, 50, 51
};

size_t Base64_EncodedSize(size_t inlen)
{
	size_t ret;

	ret = inlen;

	if (inlen % 3 != 0)
		ret += 3 - (inlen % 3);

	ret /= 3;
	ret *= 4;

	return ret;
}

size_t Base64_DecodedSize(const char *in)
{
	size_t len;
	size_t ret;
	size_t i;

	if (in == NULL)
		return 0;

	len = strlen(in);
	ret = len / 4 * 3;

	for (i=len; i-->0; )
	{
		if (in[i] == '=')
			ret--;
		else
			break;
	}

	return ret;
}

void Base64_GenerateDecodeTable(void)
{
	int inv[80];
	size_t i;

	memset(inv, -1, sizeof(inv));

	for (i=0; i<sizeof(Base64_Chars)-1; i++)
	{
		inv[Base64_Chars[i]-43] = i;
	}
}

int Base64_IsValidChar(char c)
{
	if (c >= '0' && c <= '9')
		return 1;

	if (c >= 'A' && c <= 'Z')
		return 1;

	if (c >= 'a' && c <= 'z')
		return 1;

	if (c == '+' || c == '/' || c == '=')
		return 1;

	return 0;
}

char *Base64_Encode(const unsigned char *in, size_t len)
{
	char *out;
	size_t elen;
	size_t i;
	size_t j;
	size_t v;

	if (in == NULL || len == 0)
		return NULL;

	elen = Base64_EncodedSize(len);
	out = (char *)malloc(elen + 1);
	out[elen] = '\0';

	for (i = 0, j = 0; i < len; i += 3, j += 4)
	{
		v = in[i];
		v = i+1 < len ? v << 8 | in[i+1] : v << 8;
		v = i+2 < len ? v << 8 | in[i+2] : v << 8;

		out[j] = Base64_Chars[(v >> 18) & 0x3F];
		out[j+1] = Base64_Chars[(v >> 12) & 0x3F];

		if (i+1 < len)
			out[j+2] = Base64_Chars[(v >> 6) & 0x3F];
		else
			out[j+2] = '=';

		if (i+2 < len)
			out[j+3] = Base64_Chars[v & 0x3F];
		else
			out[j+3] = '=';
	}

	return out;
}

int Base64_Decode(const char *in, unsigned char *out, size_t outlen)
{
	size_t len;
	size_t i;
	size_t j;
	int v;

	if (in == NULL || out == NULL)
		return 0;

	len = strlen(in);

	if (outlen < Base64_DecodedSize(in) || len % 4 != 0)
		return 0;

	for ( i= 0; i < len; i++)
	{
		if (!Base64_IsValidChar(in[i]))
			return 0;
	}

	for (i = 0, j = 0; i < len; i += 4, j += 3)
	{
		v = Base64_Invs[in[i]-43];
		v = (v << 6) | Base64_Invs[in[i+1]-43];
		v = in[i+2]=='=' ? v << 6 : (v << 6) | Base64_Invs[in[i+2]-43];
		v = in[i+3]=='=' ? v << 6 : (v << 6) | Base64_Invs[in[i+3]-43];

		out[j] = (v >> 16) & 0xFF;

		if (in[i+2] != '=')
			out[j+1] = (v >> 8) & 0xFF;

		if (in[i+3] != '=')
			out[j+2] = v & 0xFF;
	}

	return 1;
}
