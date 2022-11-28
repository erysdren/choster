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

#ifndef __COHOST_HEADER__
#define __COHOST_HEADER__

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

//=========================================
//
// Cohost namespace (public)
//
//=========================================

enum CohostStringProps
{
	CohostStringProp_None,
	CohostStringProp_CookieFile,
};

namespace Cohost
{
	//
	// Classes
	//

	// Notification class
	class Notification
	{
		public:
			// The type of notification that this is.
			// Can be "like", "comment", or "follow"
			string type;

			// The time this notification was created
			// Format: (year)-(month)-(day)T(hour):(minute):(second).(milisecond)(timezone)
			string created_at;

			// The ID of the project (user) that this notification came from
			int from_project_id;

			// The post ID that this notification is in response to
			int to_post_id;

			// The relationship ID between the user and the notification sender (?)
			int relationship_id;

			// The ID of the comment left by the user
			string comment_id;

			// The ID of the comment that the notification is replying to
			string in_reply_to;
	};

	// User class
	class UserInfo
	{
		public:
			// Numerical ID of the user, ordered sequentially
			int user_id;

			// The ID of the project that the user is currently using
			int project_id;

			// The handle of the project that the user is currently using
			string project_handle;

			// User booleans
			bool logged_in;
			bool mod_mode;
			bool activated;
			bool read_only;
	};

	//
	// Functions
	//

	// Bootstrap
	bool Initialize();
	bool Shutdown();
	bool SetStringProperty(int property, string str);

	// Login functions
	bool LoginWithEmailPass(string email, string password);
	bool LoginWithSessionID(string id);
	bool LoginWithCookieFile(string filename);

	// Notification
	bool GetNotifications(int offset, int limit, vector<Notification> &out);

	// UserInfo
	UserInfo GetUserInfo();
}

#endif // __COHOST_HEADER__
