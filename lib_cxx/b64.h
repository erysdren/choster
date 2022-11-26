// ========================================================
//
// FILE:			/lib_cxx/b64.h
//
// AUTHORS:			Jaycie Ewald
//
// PROJECT:			libcohost
//
// LICENSE:			ACSL v1.4
//
// DESCRIPTION:		Base64 function prototypes
//
// LAST EDITED:		November 26th, 2022
//
// ========================================================

#ifdef __cplusplus
extern "C" {
#endif

// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const char Base64_Chars[];
extern int Base64_Invs[];

size_t Base64_EncodedSize(size_t inlen);
size_t Base64_DecodedSize(const char *in);
void Base64_GenerateDecodeTable(void);
int Base64_IsValidChar(char c);
char *Base64_Encode(const unsigned char *in, size_t len);
int Base64_Decode(const char *in, unsigned char *out, size_t outlen);

#ifdef __cplusplus
}
#endif
