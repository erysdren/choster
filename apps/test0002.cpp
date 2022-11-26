// ========================================================
//
// FILE:			/test0002.cpp
//
// AUTHORS:			Jaycie Ewald
//
// PROJECT:			libcohost
//
// LICENSE:			ACSL v1.4
//
// DESCRIPTION:		Cohost terminal app (CXX)
//
// LAST EDITED:		November 26th, 2022
//
// ========================================================

// Cohost API header
#include "cohost.hpp"

// getopt header
#include <getopt.h>

#define PRINT_INDENTED(str) do { cout << " >  " << str << "\n"; } while( false )
#define PRINT_INDENTED_INPUT(str) do { cout << " >>  " << str; } while( false )

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
	string cookies_load_filename;
	string cookies_save_filename;
	string command;
	int c;
	int mode;

	// If no arguments, print help text
	if (argc < 2)
	{
		PrintHelp();
		return EXIT_SUCCESS;
	}

	// Parse arguments
	while ((c = getopt(argc, argv, "e:p:l:s:?")) != -1)
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

			// Load cookies from file
			case 'l':
				cookies_load_filename = optarg;
				mode = CTI_LOGIN_COOKIE;
				break;

			// Save cookies to file
			case 's':
				cookies_save_filename = optarg;
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

	PRINT_INDENTED("Initializing CURL...");

	// Define this variable later so the constructor isn't called right away
	CohostUser user;

	PRINT_INDENTED("Successfully initialized CURL!");

	// Set a filename for saving cookies to
	if (cookies_save_filename.empty() == false)
		user.cookies_save_filename = cookies_save_filename;

	// Login based on the specified mode
	switch (mode)
	{
		case CTI_LOGIN_EMAILPASS:
			PRINT_INDENTED("Logging into Cohost with email and password...");
			user.LoginWithEmailPass(email, password);
			break;

		case CTI_LOGIN_COOKIE:
			PRINT_INDENTED("Logging into Cohost with cookie file...");
			user.LoginWithCookieFile(cookies_load_filename);
			break;

		default:
			COHOST_ERROR("Invalid login mode!");
			break;
	}

	// Print stuff
	PRINT_INDENTED("Successfully logged into Cohost!");
	PRINT_INDENTED("");
	PRINT_INDENTED("Printing user info:");
	PRINT_INDENTED("");
	PRINT_INDENTED("User ID: " << user.user_id);
	PRINT_INDENTED("");
	PRINT_INDENTED("Current Project ID: " << user.project_id);
	PRINT_INDENTED("Current Project Handle: " << user.project_handle);
	PRINT_INDENTED("");
	PRINT_INDENTED("Logged In: " << user.logged_in);
	PRINT_INDENTED("Mod Mode: " << user.mod_mode);
	PRINT_INDENTED("Activated: " << user.activated);
	PRINT_INDENTED("Read Only: " << user.read_only);
	PRINT_INDENTED("");

	// User input parser
	while (command.compare("exit") != 0)
	{
		PRINT_INDENTED_INPUT("");
		cin >> command;

		if (command.compare("userinfo") == 0)
		{
			PRINT_INDENTED("");
			PRINT_INDENTED("Current User Information:");
			PRINT_INDENTED("");
			PRINT_INDENTED("User ID: " << user.user_id);
			PRINT_INDENTED("Project ID: " << user.project_id);
			PRINT_INDENTED("Project Handle: " << user.project_handle);
			PRINT_INDENTED("");
			PRINT_INDENTED("Logged In: " << user.logged_in);
			PRINT_INDENTED("Mod Mode: " << user.mod_mode);
			PRINT_INDENTED("Activated: " << user.activated);
			PRINT_INDENTED("Read Only: " << user.read_only);
			PRINT_INDENTED("");
		}
		else if (command.compare("help") == 0)
		{
			PRINT_INDENTED("");
			PRINT_INDENTED("Command Help:");
			PRINT_INDENTED("");
			PRINT_INDENTED("help	- Print this message");
			PRINT_INDENTED("exit	- Exit the program");
			PRINT_INDENTED("");
			PRINT_INDENTED("userinfo	- Print current user information");
			PRINT_INDENTED("");
		}
	}

	PRINT_INDENTED("");
	PRINT_INDENTED("Shutting down...");
	PRINT_INDENTED("");

	// Exit
	return EXIT_SUCCESS;
};
