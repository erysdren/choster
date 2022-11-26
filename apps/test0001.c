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
// LAST EDITED:		November 23rd, 2022
//
// ========================================================

#include "cohost.h"

#ifndef __DJGPP__
#include <getopt.h>
#endif

#define PRINTF_INDENT " >  "

enum cohost_app_modes
{
	LOGIN_WITH_EMAIL_AND_PASSWORD,
	LOGIN_WITH_CREDENTIALS_FILE,
};

void print_logo(void)
{
	printf(PRINTF_INDENT "  _____  ____    __ __  ____    ____ ______     ____    ___   _____\n");
	printf(PRINTF_INDENT " / ___/ / __ \\  / // / / __ \\  / __//_  __/    / __ \\  / _ \\ / ___/\n");
	printf(PRINTF_INDENT "/ /__  / /_/ / / _  / / /_/ / _\\ \\   / /    _ / /_/ / / , _// (_ / \n");
	printf(PRINTF_INDENT "\\___/  \\____/ /_//_/  \\____/ /___/  /_/    (_)\\____/ /_/|_| \\___/  \n");
	printf(PRINTF_INDENT "\n");
}

void print_starthelp(void)
{
	printf(PRINTF_INDENT "\n");
	printf(PRINTF_INDENT "Cohost terminal interface v0.1\n");
	printf(PRINTF_INDENT "Usage:\n");
	printf(PRINTF_INDENT "-e <email>	Email\n");
	printf(PRINTF_INDENT "-p <pass>	Password\n");
	printf(PRINTF_INDENT "-c <file>	Load credentials from file\n");
	printf(PRINTF_INDENT "-s <file>	Save credentials to file\n");
	printf(PRINTF_INDENT "\n");
}

int main(int argc, char *argv[])
{
	// Variables
	cohost_session_t *session;
	CURL *curl;
	char *email;
	char *password;
	char *credentials_load;
	char *credentials_save;
	char command[64];
	int c;
	int mode;

	// If user provided no args, print help text
	if (argc < 2)
	{
		print_starthelp();
		return EXIT_SUCCESS;
	}

	#ifndef __DJGPP__

	// Process args
	while ((c = getopt(argc, argv, "e:p:c:s:")) != -1)
	{
		switch (c)
		{
			case 'e':
				email = optarg;
				mode = LOGIN_WITH_EMAIL_AND_PASSWORD;
				break;

			case 'p':
				password = optarg;
				mode = LOGIN_WITH_EMAIL_AND_PASSWORD;
				break;

			case 'c':
				credentials_load = optarg;
				mode = LOGIN_WITH_CREDENTIALS_FILE;
				break;

			case 's':
				credentials_save = optarg;
				break;

			case '?':
				print_starthelp();
				return EXIT_SUCCESS;
				break;

			default:
				break;
		}
	}

	#else

	email = calloc(strlen(argv[1]) + 1, sizeof(char));
	password = calloc(strlen(argv[2]) + 1, sizeof(char));

	strcpy(email, argv[1]);
	strcpy(password, argv[2]);

	#endif

	// Make sure everything's correct
	if ((email == NULL && password != NULL) || (email != NULL && password == NULL))
	{
		printf(PRINTF_INDENT "ERROR: You must supply both an email and a password to login!\n");
		print_starthelp();
		return EXIT_FAILURE;
	}

	// Print a cute logo
	print_logo();

	// Initialize CURL
	printf(PRINTF_INDENT "Initializing CURL\n");
	Cohost_Initialize();
	printf(PRINTF_INDENT "CURL initialized successfully!\n");

	// Login with provided args
	// Never hurts to be paranoid.
	printf(PRINTF_INDENT "Logging in to Cohost...\n");
	if (mode == LOGIN_WITH_EMAIL_AND_PASSWORD && email != NULL && password != NULL)
	{
		session = Cohost_Login(email, password, credentials_save);
	}
	else if (mode == LOGIN_WITH_CREDENTIALS_FILE && credentials_load != NULL)
	{
		session = Cohost_LoginWithCookie(credentials_load, credentials_save);
	}
	else
	{
		printf(PRINTF_INDENT "Invalid login mode! Cannot proceed!\n");
		return EXIT_FAILURE;
	}

	// Error check
	if (session == NULL)
	{
		printf(PRINTF_INDENT "ERROR: Failed to login to Cohost!\n");
		return EXIT_FAILURE;
	}

	// Print a bunch of stuff to see if it worked
	printf(PRINTF_INDENT "Successfully logged into Cohost!\n");
	printf(PRINTF_INDENT "\n");

	printf(PRINTF_INDENT "Entering Cohost command terminal\n");
	printf(PRINTF_INDENT "Enter the heilp command to get started!\n");
	printf(PRINTF_INDENT "\n");

	// debugging
	//Cohost_GetNotifications(0, 20, session);

	// Text command parser
	while (printf(PRINTF_INDENT) && fgets(command, sizeof(command), stdin) && strncmp(command, "exit", 4) != 0)
	{
		if (!strncmp(command, "help", 4))
		{
			printf(PRINTF_INDENT "\n");
			printf(PRINTF_INDENT "help		- Display this message\n");
			printf(PRINTF_INDENT "exit		- Exit the program\n");
			printf(PRINTF_INDENT "userinfo		- Display current user information\n");
			printf(PRINTF_INDENT "\n");
		}
		else if (!strncmp(command, "notifs", 6))
		{
			Cohost_GetNotifications(0, 20, session);
		}
		else if (!strncmp(command, "userinfo", 8))
		{
			printf(PRINTF_INDENT "\n");
			printf(PRINTF_INDENT "User ID: %d\n", session->user_id);
			printf(PRINTF_INDENT "Page ID: %d\n", session->project_id);
			//printf(PRINTF_INDENT "Page Handle: %s\n", session->project_handle);
			printf(PRINTF_INDENT "\n");
		}
		else
		{
			printf(PRINTF_INDENT "\n");
			printf(PRINTF_INDENT "Unknown command: %s", command);
			printf(PRINTF_INDENT "Type \"help\" to see valid commands\n");
			printf(PRINTF_INDENT "\n");
		}
	}

	// Tell user we're exiting
	printf(PRINTF_INDENT "\n");
	printf(PRINTF_INDENT "Exiting...\n");

	// Shutdown
	printf(PRINTF_INDENT "Destroying Cohost session context\n");
	Cohost_Destroy(session);
	printf(PRINTF_INDENT "Shutting down CURL\n");
	Cohost_Shutdown();

	// Exit
	printf(PRINTF_INDENT "\n");
	printf(PRINTF_INDENT "Cya later!\n");
	printf(PRINTF_INDENT "\n");
	return EXIT_SUCCESS;
}

#if 0

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

		// print raw response data
		printf("raw POST response:\n");
		printf("%s\n", response.string);

		#if 0

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

#endif
