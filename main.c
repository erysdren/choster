// ========================================================
//
// FILE:			/main.c
//
// AUTHORS:			Jaycie Ewald
//
// PROJECT:			libcohost
//
// LICENSE:			ACSL v1.4
//
// DESCRIPTION:		Cohost terminal app
//
// LAST EDITED:		November 20th, 2022
//
// ========================================================

#include "cohost.h"

int main(int argc, char *argv[])
{
	// Variables
	CURL *curl;
	CURLcode response_code;
	struct curl_slist *cookies = NULL;
	struct curl_slist *cookie_next = NULL;
	char *salt;
	char *clienthash;
	int i;
	char *url_get;

	// print a cute logo
	printf("  _____  ____    __ __  ____    ____ ______     ____    ___   _____\n");
	printf(" / ___/ / __ \\  / // / / __ \\  / __//_  __/    / __ \\  / _ \\ / ___/\n");
	printf("/ /__  / /_/ / / _  / / /_/ / _\\ \\   / /    _ / /_/ / / , _// (_ / \n");
	printf("\\___/  \\____/ /_//_/  \\____/ /___/  /_/    (_)\\____/ /_/|_| \\___/  \n\n");

	// check number of args
	if (argc != 3)
	{
		printf("> invalid number of arguments supplied!\n");
		printf("> usage: cohost <email> <password>\n");
		printf("> exiting...\n");
		return EXIT_FAILURE;
	}
	else
	{
		printf("> password and username found!\n");
	}

	char *email = argv[1];
	char *password = argv[2];

	printf("> initializing curl\n");
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	if (curl)
	{
		// allocate response string struct
		response_t response;
		response_t response_header;
		response_allocate(&response);
		response_allocate(&response_header);

		// send GET request
		printf("> sending GET request\n");
		url_get = calloc(512, sizeof(char));
		snprintf(url_get, 512, "https://cohost.org/api/v1/login/salt?email=%s", email);
		curl_easy_setopt(curl, CURLOPT_URL, url_get);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_catch);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, response_catch);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response_header);
		response_code = curl_easy_perform(curl);

		// error check
		printf("> checking for errors...\n");
		if (response_code != CURLE_OK)
		{
			fprintf(stderr, "> curl_easy_perform() failed: %s\n", curl_easy_strerror(response_code));
			return EXIT_FAILURE;
		}
		printf("> GET request acknowledged!\n");

		// process salt hash
		printf("> processing salt hash\n");
		salt = salt_process(&response);

		// generate client hash
		printf("> generating client hash\n");
		clienthash = hash_generate(password, salt);

		// send POST request
		printf("> sending POST request\n");
		response_code = login_post_request(curl, email, clienthash);

		// error check
		printf("> checking for errors...\n");
		if (response_code != CURLE_OK)
		{
			fprintf(stderr, "> curl_easy_perform() failed: %s\n", curl_easy_strerror(response_code));
			return EXIT_FAILURE;
		}
		printf("> POST request acknowledged!\n");

		printf("\n...\n\n");

		#if 0

		// print raw response data
		printf("raw POST response:\n");
		printf("%s\n", response.string);

		// setup cookies
		printf("printing cookie info:\n");
		response_code = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);

		// error check
		if (response_code != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_getinfo() failed: %s\n", curl_easy_strerror(response_code));
			return EXIT_FAILURE;
		}

		// print cookies
		cookie_next = cookies;
		i = 1;

		while(cookie_next)
		{
			printf("[%d]: %s\n", i, cookie_next->data);
			cookie_next = cookie_next->next;
			i++;
		}

		if(i == 1)
		{
			printf("(none)\n");
		}

		#endif

		// cleanup everything and wind down
		printf("> cleaning up\n");
		curl_slist_free_all(cookies);
		free(response.string);
		free(salt);
		free(clienthash);
		free(url_get);
		curl_easy_cleanup(curl);
	}
	else
	{
		// oopsie woopsie
		printf("> couldn't init curl\n\n");
		curl_global_cleanup();
		printf("> try again later!\n\n");
		return EXIT_FAILURE;
	}

	// shutdown program
	printf("> shutting down curl\n\n");
	curl_global_cleanup();
	printf("> cya later!\n\n");
	return EXIT_SUCCESS;
}
