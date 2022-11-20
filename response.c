// ========================================================
//
// FILE:			/response.c
//
// AUTHORS:			Jaycie Ewald
//
// PROJECT:			libcohost
//
// LICENSE:			ACSL v1.4
//
// DESCRIPTION:		Response structure handling
//
// LAST EDITED:		November 20th, 2022
//
// ========================================================

#include "cohost.h"

// https://stackoverflow.com/questions/2329571/c-libcurl-get-output-into-a-string
size_t response_catch(void *pointer, size_t size, size_t nmemb, response_t *response)
{
	size_t new_length = response->len_string + size * nmemb;

	response->string = realloc(response->string, new_length + 1);

	if (response->string == NULL)
	{
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}

	memcpy(response->string + response->len_string, pointer, size * nmemb);

	response->string[new_length] = '\0';
	response->len_string = new_length;

	return size * nmemb;
}


// https://stackoverflow.com/questions/2329571/c-libcurl-get-output-into-a-string
void response_allocate(response_t *response)
{
	response->len_string = 0;
	response->string = malloc(response->len_string + 1);

	if (response->string == NULL)
	{
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}

	response->string[0] = '\0';
}
