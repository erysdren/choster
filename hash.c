// ========================================================
//
// FILE:			/hash.c
//
// AUTHORS:			Jaycie Ewald
//
// PROJECT:			libcohost
//
// LICENSE:			ACSL v1.4
//
// DESCRIPTION:		Hash handling
//
// LAST EDITED:		November 20th, 2022
//
// ========================================================

#include "cohost.h"

char *hash_generate(char *password, char *salt)
{
	// variables
	char *hash;

	// allocate hash buffer
	hash = calloc(128, sizeof(char));

	// use nettle to generate it
	nettle_pbkdf2_hmac_sha384(strlen(password), password, 200000, strlen(salt), salt, 128, hash);

	// encode client hash
	return b64_encode((unsigned char *)hash, strlen(hash));
}
