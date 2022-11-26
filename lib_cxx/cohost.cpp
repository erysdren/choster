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
// LAST EDITED:		November 26th, 2022
//
// ========================================================

// Cohost header
#include "cohost.hpp"

// Base64 header
#include "b64.h"

//=========================================
//
// Base64 Functions
//
//=========================================

extern "C" {

#include <string.h>

const char Base64_Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int Base64_Invs[] = {
	62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58,
	59, 60, 61, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
	21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
	29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
	43, 44, 45, 46, 47, 48, 49, 50, 51
};

size_t Base64_EncodedSize(size_t inlen)
{
	size_t ret;

	ret = inlen;

	if (inlen % 3 != 0)
		ret += 3 - (inlen % 3);

	ret /= 3;
	ret *= 4;

	return ret;
}

size_t Base64_DecodedSize(const char *in)
{
	size_t len;
	size_t ret;
	size_t i;

	if (in == NULL)
		return 0;

	len = strlen(in);
	ret = len / 4 * 3;

	for (i=len; i-->0; )
	{
		if (in[i] == '=')
			ret--;
		else
			break;
	}

	return ret;
}

void Base64_GenerateDecodeTable(void)
{
	int inv[80];
	size_t i;

	memset(inv, -1, sizeof(inv));

	for (i=0; i<sizeof(Base64_Chars)-1; i++)
	{
		inv[Base64_Chars[i]-43] = i;
	}
}

int Base64_IsValidChar(char c)
{
	if (c >= '0' && c <= '9')
		return 1;

	if (c >= 'A' && c <= 'Z')
		return 1;

	if (c >= 'a' && c <= 'z')
		return 1;

	if (c == '+' || c == '/' || c == '=')
		return 1;

	return 0;
}

char *Base64_Encode(const unsigned char *in, size_t len)
{
	char *out;
	size_t elen;
	size_t i;
	size_t j;
	size_t v;

	if (in == NULL || len == 0)
		return NULL;

	elen = Base64_EncodedSize(len);
	out = (char *)malloc(elen + 1);
	out[elen] = '\0';

	for (i = 0, j = 0; i < len; i += 3, j += 4)
	{
		v = in[i];
		v = i+1 < len ? v << 8 | in[i+1] : v << 8;
		v = i+2 < len ? v << 8 | in[i+2] : v << 8;

		out[j] = Base64_Chars[(v >> 18) & 0x3F];
		out[j+1] = Base64_Chars[(v >> 12) & 0x3F];

		if (i+1 < len)
			out[j+2] = Base64_Chars[(v >> 6) & 0x3F];
		else
			out[j+2] = '=';

		if (i+2 < len)
			out[j+3] = Base64_Chars[v & 0x3F];
		else
			out[j+3] = '=';
	}

	return out;
}

int Base64_Decode(const char *in, unsigned char *out, size_t outlen)
{
	size_t len;
	size_t i;
	size_t j;
	int v;

	if (in == NULL || out == NULL)
		return 0;

	len = strlen(in);

	if (outlen < Base64_DecodedSize(in) || len % 4 != 0)
		return 0;

	for ( i= 0; i < len; i++)
	{
		if (!Base64_IsValidChar(in[i]))
			return 0;
	}

	for (i = 0, j = 0; i < len; i += 4, j += 3)
	{
		v = Base64_Invs[in[i]-43];
		v = (v << 6) | Base64_Invs[in[i+1]-43];
		v = in[i+2]=='=' ? v << 6 : (v << 6) | Base64_Invs[in[i+2]-43];
		v = in[i+3]=='=' ? v << 6 : (v << 6) | Base64_Invs[in[i+3]-43];

		out[j] = (v >> 16) & 0xFF;

		if (in[i+2] != '=')
			out[j+1] = (v >> 8) & 0xFF;

		if (in[i+3] != '=')
			out[j+2] = v & 0xFF;
	}

	return 1;
}

}

//=========================================
//
// CohostNotification Class
//
//=========================================

//=========================================
//
// CohostUser Class
//
//=========================================

//
// Public functions
//

// CohostUser constructor
CohostUser::CohostUser()
{
	COHOST_DEBUG_MESSAGE("CohostUser constructor called.");

	// Initialize CURL
	CURLInit();
}

// CohostUser destructor
CohostUser::~CohostUser()
{
	COHOST_DEBUG_MESSAGE("CohostUser destructor called.");

	// Cleanup CURL
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

// Login with an email and password
void CohostUser::LoginWithEmailPass(string email, string password)
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

	COHOST_DEBUG_MESSAGE("Logging into cohost.org with an email and password.");

	// Sanity checks
	if (email.empty()) COHOST_ERROR("The email provided by the user was an empty string!");
	if (password.empty()) COHOST_ERROR("The password provided by the user was an empty string!");

	// Get our URL
	ostringstream_buffer << COHOST_URL_LOGIN_SALT << "?email=" << email;

	// Set CURLOPTs
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
	if (cookies_save_filename.empty() == false) curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookies_save_filename.c_str());
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
	DecodeSalt(response_body);

	// Encode ClientHash
	EncodeClientHash(password);

	// Sanity checks
	if (salt.empty()) COHOST_ERROR("Something went wrong decoding the Salt in LoginWithEmailPass()!");
	if (clienthash.empty()) COHOST_ERROR("Something went wrong encoding the ClientHash in LoginWithEmailPass()!");

	// Clear the string buffers
	ostringstream_buffer.str("");
	ostringstream_buffer.clear();
	response_body.clear();
	response_head.clear();

	// Make a JSON string for the POST request
	ostringstream_buffer << "{ \"email\" : \"" << email << "\", \"clientHash\" : \"" << clienthash << "\" }";

	// For some reason I have to do this
	char_buffer = (char *)calloc(ostringstream_buffer.str().length() + 1, sizeof(char));
	strcpy(char_buffer, ostringstream_buffer.str().c_str());

	// Setup POST headers
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Expect:");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charset: utf-8");

	// Set CURLOPTs
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, char_buffer);
	curl_easy_setopt(curl, CURLOPT_URL, COHOST_URL_LOGIN);

	// Perform the POST request
	response_code = curl_easy_perform(curl);
	if (response_code != CURLE_OK)
		COHOST_ERROR("curl_easy_perform() failed with error: " << curl_easy_strerror(response_code));

	// Clear the string buffers
	response_body.clear();
	response_head.clear();

	// Set CURLOPTs
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_URL, COHOST_URL_TPRC COHOST_TPRC_LOGGEDIN);

	// Perform the GET request
	response_code = curl_easy_perform(curl);
	if (response_code != CURLE_OK)
		COHOST_ERROR("curl_easy_perform() failed with error: " << curl_easy_strerror(response_code));

	// Populate the user info from the JSON response
	PopulateUserInfo(response_body);

	// Clean up memory
	curl_slist_free_all(headers);
	free(char_buffer);
}

// Login with a cookie file
void CohostUser::LoginWithCookieFile(string filename)
{
	// Variables
	string response_body;
	string response_head;
	CURLcode response_code;

	COHOST_DEBUG_MESSAGE("Logging into cohost.org with a cookie file.");

	// Sanity checks
	if (filename.empty()) COHOST_ERROR("The filename provided by the user was an empty string!");
	if (FILE *test_if_exists = fopen(filename.c_str(), "r")) fclose(test_if_exists); else COHOST_ERROR("The file provided by the user does not exist!");

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
		COHOST_ERROR("curl_easy_perform() failed with error: " << curl_easy_strerror(response_code));

	// Populate the user info from the JSON response
	PopulateUserInfo(response_body);
}

//
// Private functions
//

// Initialize CURL
void CohostUser::CURLInit()
{
	// Variables
	CURLcode response_code;

	COHOST_DEBUG_MESSAGE("Initializing CURL.");

	// Initialize global CURL and our specific instance
	response_code = curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	if (response_code != CURLE_OK || curl == NULL)
		COHOST_ERROR("libCURL failed to initialize!");
}

// Callback for catching CURL responses
size_t CohostUser::CURLResponseCallback(void *contents, size_t size, size_t n, void *user)
{
	((string*)user)->append((char*)contents, size * n);
	return size * n;
}

// Decode the Salt from the JSON response
void CohostUser::DecodeSalt(string response)
{
	// Variables
	cJSON *json;
	string string_buffer;
	int len;
	char *char_buffer;

	COHOST_DEBUG_MESSAGE("Decoding Salt.");

	// Parse the JSON response
	json = cJSON_ParseWithLength(response.c_str(), response.length());
	if (json == NULL)
		COHOST_ERROR("Failed to parse JSON response in DecodeSalt()!");

	// Get the salt string
	json = cJSON_GetObjectItemCaseSensitive(json, "salt");
	if (json == NULL)
		COHOST_ERROR("Failed to find Salt in JSON response in DecodeSalt()!");

	// Modify the string appropriately
	string_buffer = json->valuestring;
	replace(string_buffer.begin(), string_buffer.end(), '-', 'A');
	replace(string_buffer.begin(), string_buffer.end(), '_', 'A');
	string_buffer.append("==");

	// Decode Base64
	len = Base64_DecodedSize(string_buffer.c_str());
	char_buffer = (char *)calloc(len, sizeof(char));
	if (!Base64_Decode(string_buffer.c_str(), (unsigned char *)char_buffer, len))
		COHOST_ERROR("Failed to decode Base64 in DecodeSalt()!");

	// Assign Salt to the decoded string
	salt = char_buffer;

	// Clean up the memory
	cJSON_Delete(json);
	free(char_buffer);
}

// Encode the client hash with sha384
void CohostUser::EncodeClientHash(string password)
{
	// Variables
	uint8_t *char_buffer;

	// Allocate hash char buffer
	char_buffer = (uint8_t *)calloc(128, sizeof(char));

	// use nettle to generate it
	nettle_pbkdf2_hmac_sha384(password.length(), (uint8_t *)password.c_str(), 200000, salt.length(), (uint8_t *)salt.c_str(), 128, char_buffer);

	// Encode clienthash
	clienthash = Base64_Encode(char_buffer, strlen((char *)char_buffer));

	// Clean up the memory
	free(char_buffer);
}

// Populate the user info from the JSON response
void CohostUser::PopulateUserInfo(string response)
{
	// Variables
	cJSON *json;

	// Start parsing JSON string
	json = cJSON_ParseWithLength(response.c_str(), response.length());
	json = cJSON_GetObjectItemCaseSensitive(json, "result");
	json = cJSON_GetObjectItemCaseSensitive(json, "data");
	if (json == NULL)
		COHOST_ERROR("Failed to parse JSON object in PopulateUserInfo()!");

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
}
