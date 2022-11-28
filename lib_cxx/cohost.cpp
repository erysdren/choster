// ========================================================
//
// FILE:			/cohost.cpp
//
// AUTHORS:			Jaycie Ewald
//
// PROJECT:			libcohost
//
// LICENSE:			ACSL v1.4
//
// DESCRIPTION:		Cohost API functions (C++)
//
// LAST EDITED:		November 28th, 2022
//
// ========================================================

// Cohost header
#include "cohost.hpp"

// Base64 header
#include "b64.h"

//=========================================
//
// Cohost namespace (private)
//
//=========================================

namespace Cohost
{
	//
	// Variables
	//

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

	// The ID of the current session
	string session_id;

	// Filename to save cookies to
	string cookies_filename;

	// CURL
	CURL *curl;

	// Salt string returned from login
	string salt;

	// Client hash string generated from the salt
	string client_hash;

	//
	// Functions
	//

	// CURL
	static size_t CURLResponseCallback(void *contents, size_t size, size_t n, void *user);

	// Cryptography
	bool DecodeSalt(string response);
	bool EncodeClientHash(string password);

	// Helpers
	bool PopulateUserInfo(string response);
}

//
//
// Public functions
//
//

//
// Bootstrap
//

// Initialize CURL
bool Cohost::Initialize()
{
	// Variables
	CURLcode response_code;

	// Initialize global CURL and our specific instance
	response_code = curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	if (response_code != CURLE_OK || curl == NULL)
		return false;

	return true;
}

// Shutdown CURL
bool Cohost::Shutdown()
{
	// Cleanup CURL
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return true;
}

// Set a string property in the Cohost instance
bool Cohost::SetStringProperty(int property, string str)
{
	switch (property)
	{
		case CohostStringProp_None:
			return true;

		case CohostStringProp_CookieFile:
			cookies_filename = str;
			return true;

		default:
			break;
	}

	return false;
}

//
// Login
//

// Login with an email and password
bool Cohost::LoginWithEmailPass(string email, string password)
{
	// Variables
	ostringstream ostringstream_buffer;
	string string_buffer;
	string response_body;
	string response_head;
	CURLcode response_code;
	struct curl_slist *headers = NULL;
	struct curl_slist *cookies = NULL;
	char *char_buffer;

	// Sanity checks
	if (email.empty()) return false; // JAYCIE: add an error message here
	if (password.empty()) return false; // JAYCIE: add an error message here

	// Get our URL
	ostringstream_buffer << COHOST_URL_LOGIN_SALT << "?email=" << email;

	// Set CURLOPTs
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
	if (cookies_filename.empty() == false) curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookies_filename.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURLResponseCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, CURLResponseCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response_head);
	curl_easy_setopt(curl, CURLOPT_URL, ostringstream_buffer.str().c_str());

	// Perform the GET request
	response_code = curl_easy_perform(curl);
	if (response_code != CURLE_OK)
		COHOST_ERROR("curl_easy_perform() failed with error: " << curl_easy_strerror(response_code));

	// Decode Salt
	if (DecodeSalt(response_body) == false)
		return false;
		// JAYCIE: add an error message here

	// Encode ClientHash
	if (EncodeClientHash(password) == false)
		return false;
		// JAYCIE: add an error message here

	// Sanity checks
	if (salt.empty()) return false; // JAYCIE: add an error message here
	if (client_hash.empty()) return false; // JAYCIE: add an error message here

	// Clear the string buffers
	ostringstream_buffer.str("");
	ostringstream_buffer.clear();
	response_body.clear();
	response_head.clear();

	// Make a JSON string for the POST request
	ostringstream_buffer << "{ \"email\" : \"" << email << "\", \"clientHash\" : \"" << client_hash << "\" }";

	// For some reason I have to do this
	char_buffer = (char *)calloc(ostringstream_buffer.str().length() + 1, sizeof(char));
	strcpy(char_buffer, ostringstream_buffer.str().c_str());

	// Setup POST headers
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Expect:");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charset: utf-8");

	// Set CURLOPTs
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, char_buffer);
	curl_easy_setopt(curl, CURLOPT_URL, COHOST_URL_LOGIN);

	// Perform the POST request
	response_code = curl_easy_perform(curl);
	if (response_code != CURLE_OK)
		return false;
		// JAYCIE: add an error message here

	// Clear the string buffers
	response_body.clear();
	response_head.clear();

	// Set CURLOPTs
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_URL, COHOST_URL_TPRC COHOST_TPRC_LOGGEDIN);

	// Perform the GET request
	response_code = curl_easy_perform(curl);
	if (response_code != CURLE_OK)
		return false;
		// JAYCIE: add an error message here

	// Populate the user info from the JSON response
	if (PopulateUserInfo(response_body) == false)
		return false;
		// JAYCIE: add an error message here

	// Clean up memory
	curl_slist_free_all(headers);
	free(char_buffer);

	return true;
}

// Login with a cookie file
bool Cohost::LoginWithCookieFile(string filename)
{
	// Variables
	string response_body;
	string response_head;
	CURLcode response_code;

	// Sanity checks
	if (filename.empty())
		return false;
		// JAYCIE: add an error message here

	if (FILE *test_if_exists = fopen(filename.c_str(), "r"))
		fclose(test_if_exists);
	else
		return false;
		// JAYCIE: add an error message here

	// Set CURLOPTs
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, filename.c_str());
	curl_easy_setopt(curl, CURLOPT_COOKIELIST, "RELOAD");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURLResponseCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, CURLResponseCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response_head);
	curl_easy_setopt(curl, CURLOPT_URL, COHOST_URL_TPRC COHOST_TPRC_LOGGEDIN);

	// Perform the GET request
	response_code = curl_easy_perform(curl);
	if (response_code != CURLE_OK)
		return false;
		// JAYCIE: add an error message here

	// Populate the user info from the JSON response
	if (PopulateUserInfo(response_body) == false)
		return false;
		// JAYCIE: add an error message here

	return true;
}

//
// Notification
//

bool Cohost::GetNotifications(int offset, int limit, vector<Notification> &out)
{
	// Variables
	string response_body;
	string response_head;
	CURLcode response_code;
	cJSON *json_root;
	cJSON *json_arrayitem;
	cJSON *json_object;
	int i;
	Notification notification;
	ostringstream url;

	url << COHOST_URL_NOTIFS_LIST << "?offset=" << offset << "&limit=" << limit;

	// Set CURLOPTs
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURLResponseCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, CURLResponseCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response_head);
	curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());

	// Perform the GET request
	response_code = curl_easy_perform(curl);
	if (response_code != CURLE_OK)
		return false;
		// JAYCIE: add an error message here

	// Parse the json response
	json_root = cJSON_ParseWithLength(response_body.c_str(), response_body.length());
	json_root = cJSON_GetObjectItemCaseSensitive(json_root, "notifications");
	if (json_root == NULL)
		return false;
		// JAYCIE: add an error message here

	// Populate notification array
	for (i = 0; i < limit; i++)
	{
		json_arrayitem = cJSON_GetArrayItem(json_root, i);

		// JAYCIE: make this prettier plz

		json_object = cJSON_GetObjectItemCaseSensitive(json_arrayitem, "commentId");
		if (json_object != NULL) notification.comment_id = json_object->valuestring;
		else notification.comment_id = "";

		json_object = cJSON_GetObjectItemCaseSensitive(json_arrayitem, "createdAt");
		if (json_object != NULL) notification.created_at = json_object->valuestring;
		else notification.created_at = "";

		json_object = cJSON_GetObjectItemCaseSensitive(json_arrayitem, "fromProjectId");
		if (json_object != NULL) notification.from_project_id = json_object->valueint;
		else notification.from_project_id = -1;

		json_object = cJSON_GetObjectItemCaseSensitive(json_arrayitem, "inReplyTo");
		if (json_object != NULL) notification.in_reply_to = json_object->valuestring;
		else notification.in_reply_to = "";

		json_object = cJSON_GetObjectItemCaseSensitive(json_arrayitem, "relationshipId");
		if (json_object != NULL) notification.relationship_id = json_object->valueint;
		else notification.relationship_id = -1;

		json_object = cJSON_GetObjectItemCaseSensitive(json_arrayitem, "toPostId");
		if (json_object != NULL) notification.to_post_id = json_object->valueint;
		else notification.to_post_id = -1;

		json_object = cJSON_GetObjectItemCaseSensitive(json_arrayitem, "type");
		if (json_object != NULL) notification.type = json_object->valuestring;
		else notification.type = "";

		out.push_back(notification);
	}

	// Clean up memory
	cJSON_Delete(json_root);

	return true;
}

//
// UserInfo
//

// Return information about the user currently logged in
Cohost::UserInfo Cohost::GetUserInfo()
{
	// Variables
	UserInfo u;

	// Booleans
	u.activated = activated;
	u.logged_in = logged_in;
	u.mod_mode = mod_mode;
	u.read_only = read_only;

	// Strings
	u.project_handle = project_handle;
	u.project_id = project_id;

	// Integers
	u.user_id = user_id;

	// Return the UserInfo object
	return u;
}

//
//
// Private functions
//
//

//
// CURL
//

// Callback for catching CURL responses
size_t Cohost::CURLResponseCallback(void *contents, size_t size, size_t n, void *user)
{
	((string*)user)->append((char*)contents, size * n);
	return size * n;
}

//
// Cryptography
//

// Decode the Salt from the JSON response
bool Cohost::DecodeSalt(string response)
{
	// Variables
	cJSON *json;
	string string_buffer;
	int len;
	char *char_buffer;

	// Parse the JSON response
	json = cJSON_ParseWithLength(response.c_str(), response.length());
	if (json == NULL)
		return false;
		// JAYCIE: add an error message here

	// Get the salt string
	json = cJSON_GetObjectItemCaseSensitive(json, "salt");
	if (json == NULL)
		return false;
		// JAYCIE: add an error message here

	// Modify the string appropriately
	string_buffer = json->valuestring;
	replace(string_buffer.begin(), string_buffer.end(), '-', 'A');
	replace(string_buffer.begin(), string_buffer.end(), '_', 'A');
	string_buffer.append("==");

	// Decode Base64
	len = Base64_DecodedSize(string_buffer.c_str());
	char_buffer = (char *)calloc(len, sizeof(char));
	if (!Base64_Decode(string_buffer.c_str(), (unsigned char *)char_buffer, len))
		return false;
		// JAYCIE: add an error message here

	// Assign Salt to the decoded string
	salt = char_buffer;

	// Clean up the memory
	cJSON_Delete(json);
	free(char_buffer);

	return true;
}

// Encode the client hash with sha384
bool Cohost::EncodeClientHash(string password)
{
	// Variables
	uint8_t *char_buffer;

	// Allocate hash char buffer
	char_buffer = (uint8_t *)calloc(128, sizeof(char));

	if (char_buffer == NULL)
		return false;
		// JAYCIE: add an error message here

	// use nettle to generate it
	nettle_pbkdf2_hmac_sha384(password.length(), (uint8_t *)password.c_str(), 200000, salt.length(), (uint8_t *)salt.c_str(), 128, char_buffer);

	// Encode clienthash
	client_hash = Base64_Encode(char_buffer, strlen((char *)char_buffer));

	// Clean up the memory
	free(char_buffer);

	return true;
}

//
// Helpers
//

// Populate the user info from the JSON response
bool Cohost::PopulateUserInfo(string response)
{
	// Variables
	cJSON *json;

	// Start parsing JSON string
	json = cJSON_ParseWithLength(response.c_str(), response.length());
	json = cJSON_GetObjectItemCaseSensitive(json, "result");
	json = cJSON_GetObjectItemCaseSensitive(json, "data");
	if (json == NULL)
		return false;
		// JAYCIE: add an error message here

	// Populate variables
	user_id = cJSON_GetObjectItemCaseSensitive(json, "userId")->valueint;
	project_id = cJSON_GetObjectItemCaseSensitive(json, "projectId")->valueint;
	project_handle = cJSON_GetObjectItemCaseSensitive(json, "projectHandle")->valuestring;
	logged_in = cJSON_GetObjectItemCaseSensitive(json, "loggedIn")->valueint;
	activated = cJSON_GetObjectItemCaseSensitive(json, "activated")->valueint;
	read_only = cJSON_GetObjectItemCaseSensitive(json, "readOnly")->valueint;
	mod_mode = cJSON_GetObjectItemCaseSensitive(json, "modMode")->valueint;

	// Clean up the memory
	cJSON_Delete(json);

	return true;
}
