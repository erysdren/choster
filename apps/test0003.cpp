// ========================================================
//
// FILE:			/apps/test0003.cpp
//
// AUTHORS:			Jaycie Ewald
//
// PROJECT:			libcohost
//
// LICENSE:			ACSL v1.4
//
// DESCRIPTION:		Cohost terminal app (CXX)
//
// LAST EDITED:		November 28th, 2022
//
// ========================================================

// Cohost API header
#include "cohost.hpp"

// getopt header
#include <getopt.h>

#define PRINT_INDENTED(str) do { cout << " >  " << str << "\n"; } while( false )
#define PRINT_INDENTED_INPUT(str) do { cout << " >>  " << str; } while( false )

#define INI_FILENAME "test0003.ini"

enum CTI_LoginModes
{
	CTI_LOGIN_EMAILPASS = 1,
	CTI_LOGIN_COOKIE = 2,
};

void PrintLogo()
{
	PRINT_INDENTED("  _____  ____    __ __  ____    ____ ______     ____    ___   _____");
	PRINT_INDENTED(" / ___/ / __ \\  / // / / __ \\  / __//_  __/    / __ \\  / _ \\ / ___/");
	PRINT_INDENTED("/ /__  / /_/ / / _  / / /_/ / _\\ \\   / /    _ / /_/ / / , _// (_ / ");
	PRINT_INDENTED("\\___/  \\____/ /_//_/  \\____/ /___/  /_/    (_)\\____/ /_/|_| \\___/  ");
	PRINT_INDENTED("");
}

void PrintHelp()
{
	PRINT_INDENTED("");
	PRINT_INDENTED("Cohost Terminal Interface v0.1");
	PRINT_INDENTED("");
	PRINT_INDENTED("Commandline Usage:");
	PRINT_INDENTED("");
	PRINT_INDENTED("Email address:");
	PRINT_INDENTED("	-e <email>");
	PRINT_INDENTED("Password:");
	PRINT_INDENTED("	-p <password>");
	PRINT_INDENTED("Load cookies from file:");
	PRINT_INDENTED("	-l <filename>");
	PRINT_INDENTED("Save cookies to file:");
	PRINT_INDENTED("	-s <filename>");
	PRINT_INDENTED("");
}

// Entry point
int main(int argc, char *argv[])
{
	// Variables
	string email;
	string password;
	string command;
	vector<Cohost::Notification> notifications;
	int i;
	int c;
	int mode;

	// If no arguments, print help text
	if (argc < 2)
	{
		PrintHelp();
		return EXIT_SUCCESS;
	}

	// Parse arguments
	while ((c = getopt(argc, argv, "e:p:?")) != -1)
	{
		switch (c)
		{
			// Email
			case 'e':
				email = optarg;
				mode = CTI_LOGIN_EMAILPASS;
				break;

			// Password
			case 'p':
				password = optarg;
				mode = CTI_LOGIN_EMAILPASS;
				break;

			// Help
			case '?':
				PrintHelp();
				return EXIT_SUCCESS;
				break;

			default:
				break;
		}
	}

	// Print a cute logo
	PrintLogo();

	PRINT_INDENTED("Initializing Cohost...");

	if (Cohost::Initialize() == false)
	{
		PRINT_INDENTED("Failed to initialize Cohost!");
		return EXIT_FAILURE;
	}

	PRINT_INDENTED("Successfully initialized Cohost!");
	PRINT_INDENTED("Logging into Cohost...");

	if (Cohost::SetStringProperty(CohostStringProp_CookieFile, INI_FILENAME) == false)
		PRINT_INDENTED("Failed to set string property on Cohost!");

	if (Cohost::LoginWithCookieFile(INI_FILENAME) == false)
	{
		PRINT_INDENTED("Failed to login to Cohost with cookie file!");
		PRINT_INDENTED("Trying email/password combo instead...");

		if (Cohost::LoginWithEmailPass(email, password) == false)
		{
			PRINT_INDENTED("Failed to login to Cohost!");
			Cohost::Shutdown();
			return EXIT_FAILURE;
		}
	}

	PRINT_INDENTED("Successfully logged in to Cohost!");

	PRINT_INDENTED("Project Handle: " << Cohost::GetUserInfo().project_handle);

	Cohost::GetNotifications(0, 5, notifications);

	for (i = 0; i < 5; i++)
	{
		PRINT_INDENTED("");
		PRINT_INDENTED("notification " << i << ":");
		PRINT_INDENTED("comment_id: "<< notifications[i].comment_id);
		PRINT_INDENTED("created_at: "<< notifications[i].created_at);
		PRINT_INDENTED("from_project_id: "<< notifications[i].from_project_id);
		PRINT_INDENTED("in_reply_to: "<< notifications[i].in_reply_to);
		PRINT_INDENTED("relationship_id: "<< notifications[i].relationship_id);
		PRINT_INDENTED("to_post_id: "<< notifications[i].to_post_id);
		PRINT_INDENTED("type: "<< notifications[i].type);
		PRINT_INDENTED("");
	}

	PRINT_INDENTED("Shutting down Cohost...");

	if (Cohost::Shutdown() == false)
	{
		PRINT_INDENTED("Failed to shutdown Cohost!");
		return EXIT_FAILURE;
	}

	PRINT_INDENTED("Successfully shutdown Cohost!");

	// Exit
	return EXIT_SUCCESS;
};
