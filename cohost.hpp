// ========================================================
//
// FILE:			/cohost.hpp
//
// AUTHORS:			Jaycie Ewald
//
// PROJECT:			libcohost
//
// LICENSE:			ACSL v1.4
//
// DESCRIPTION:		Header for Cohost API functions (C++)
//
// LAST EDITED:		November 25th, 2022
//
// ========================================================

//
// Headers
//

// Standard headers
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

// Library headers
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <nettle/pbkdf2.h>

// Namespace
using namespace std;

//
// Definitions
//

// Cohost URLs
#define COHOST_URL_API				"https://cohost.org/api/v1/"
#define COHOST_URL_LOGIN			COHOST_URL_API "login/"
#define COHOST_URL_LOGIN_SALT		COHOST_URL_LOGIN "salt/"
#define COHOST_URL_TPRC				COHOST_URL_API "trpc/"
#define COHOST_URL_NOTIFS			COHOST_URL_API "notifications/"
#define COHOST_URL_NOTIFS_LIST		COHOST_URL_NOTIFS "list"

// Cohost TPRC commands
#define COHOST_TPRC_LOGGEDIN		"login.loggedIn"

// Messages
#define COHOST_ERROR(str) do { cerr << "Error: " << str << "\n"; exit(EXIT_FAILURE); } while( false )
#define COHOST_WARNING(str) do { cerr << "Warning: " << str << "\n"; } while( false )
#define COHOST_MESSAGE(str) do { cout << str << "\n"; } while( false )

// Verbose-only
#ifdef VERBOSE
	#define COHOST_DEBUG_MESSAGE(str) do { cout << str << "\n"; } while( false )
#else
	#define COHOST_DEBUG_MESSAGE(string) do { } while ( false )
#endif
