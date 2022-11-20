// ========================================================
//
// FILE:			/salt.c
//
// AUTHORS:			Jaycie Ewald
//
// PROJECT:			libcohost
//
// LICENSE:			ACSL v1.4
//
// DESCRIPTION:		Salt handling
//
// LAST EDITED:		November 20th, 2022
//
// ========================================================

#include "cohost.h"

char *salt_process(response_t *response)
{
	// variables
	cJSON *json;
	char *salt;

	// parse json
	json = cJSON_ParseWithLength(response->string, response->len_string);
	json = cJSON_GetObjectItemCaseSensitive(json, "salt");

	// modify salt hash appropriately
	salt = calloc(strlen(json->valuestring) + 3, sizeof(char));
	strncpy(salt, json->valuestring, strlen(json->valuestring) + 1);
	salt = replace_char(salt, '-', 'A');
	salt = replace_char(salt, '_', 'A');
	strcat(salt, "==");

	return salt_decode(salt);
}

char *salt_decode(char *salt)
{
	// Variables
	int decoded_len;
	char *decoded;

	decoded_len = b64_decoded_size(salt) + 1;
	decoded = calloc(decoded_len, sizeof(char));

	if (!b64_decode(salt, (unsigned char *)decoded, decoded_len))
	{
		printf("failed to decode base64 in salt_decode()\n");
		exit(EXIT_FAILURE);
	}

	return decoded;
}
