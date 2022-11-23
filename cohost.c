// ========================================================
//
// FILE:			/cohost.c
//
// AUTHORS:			Jaycie Ewald
//
// PROJECT:			libcohost
//
// LICENSE:			ACSL v1.4
//
// DESCRIPTION:		Cohost API functions
//
// LAST EDITED:		November 23rd, 2022
//
// ========================================================

#include "cohost.h"

//
// Public functions
//

// Initialize CURL
CURL *Cohost_Initialize(void)
{
	curl_global_init(CURL_GLOBAL_ALL);
	return curl_easy_init();
}

// Attempt to login to cohost.org and return a session context
cohost_session_t *Cohost_Login(char *email, char *password, char *cookie_save_filename, CURL *curl)
{
	// Variables
	struct curl_slist *headers = NULL;
	cohost_session_t *session;
	curl_response_t resp_body;
	curl_response_t resp_head;
	CURLcode resp_code;
	cJSON *json;
	cJSON *json_item;
	char *url;
	char *salt;
	char *clienthash;
	char *post;
	char *token;
	int i;

	// Sanity checks
	if (curl == NULL)
	{
		printf("ERROR: Provided CURL context was not properly initialized!\n");
		return NULL;
	}

	if (email == NULL)
	{
		printf("ERROR: Provided email string is NULL!\n");
		return NULL;
	}

	if (password == NULL)
	{
		printf("ERROR: Provided password string is NULL!\n");
		return NULL;
	}

	// Allocate data
	session = calloc(1, sizeof(cohost_session_t));
	url = calloc(512, sizeof(char));

	// Setup URL for GET request
	snprintf(url, 512, "https://cohost.org/api/v1/login/salt?email=%s", email);

	// Set CURLOPTs and send GET request
	CURL_ResponseAllocate(&resp_body);
	CURL_ResponseAllocate(&resp_head);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
	#ifndef __DJGPP__
	if (cookie_save_filename != NULL) curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookie_save_filename);
	#endif
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURL_ResponseCatch);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp_body);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, CURL_ResponseCatch);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &resp_head);
	resp_code = curl_easy_perform(curl);

	// Error check
	if (resp_code != CURLE_OK)
	{
		printf("ERROR: curl_easy_perform() failed with error \"%s\"\n", curl_easy_strerror(resp_code));
		return NULL;
	}

	// Get salt string
	salt = Cryptography_DecodeSalt(&resp_body);

	// Generate clienthash
	clienthash = Cryptography_GenerateClientHash(password, salt);

	// Generate POST request string
	post = calloc(1024, sizeof(char));
	snprintf(post, 1024, "{ \"email\" : \"%s\", \"clientHash\" : \"%s\" }", email, clienthash);

	// Setup POST headers
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Expect:");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charset: utf-8");

	// Set CURLOPTs and send POST request
	CURL_ResponseAllocate(&resp_body);
	CURL_ResponseAllocate(&resp_head);
	curl_easy_setopt(curl, CURLOPT_URL, "https://cohost.org/api/v1/login");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);
	resp_code = curl_easy_perform(curl);

	// Error check
	if (resp_code != CURLE_OK)
	{
		printf("ERROR: curl_easy_perform() failed with error \"%s\"\n", curl_easy_strerror(resp_code));
		return NULL;
	}

	// Get the session ID from the cookie
	resp_code = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &session->cookies);

	// Error check
	if (resp_code != CURLE_OK)
	{
		printf("ERROR: curl_easy_perform() failed with error \"%s\"\n", curl_easy_strerror(resp_code));
		return NULL;
	}

	// Really, really lazy cookie parsing
	token = strtok(session->cookies->data, "\t");

	while(token != NULL)
	{
		if (!strcmp(token, "connect.sid"))
		{
			token = strtok(NULL, "\t");
			break;
		}

		token = strtok(NULL, "\t");
	}

	// Set CURLOPTs and send GET request for user info
	CURL_ResponseAllocate(&resp_body);
	CURL_ResponseAllocate(&resp_head);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_URL, "https://cohost.org/api/v1/trpc/login.loggedIn");
	resp_code = curl_easy_perform(curl);

	// Error check
	if (resp_code != CURLE_OK)
	{
		printf("ERROR: curl_easy_perform() failed with error \"%s\"\n", curl_easy_strerror(resp_code));
		return NULL;
	}

	// Get the user ID from the POST response
	json = cJSON_ParseWithLength(resp_body.string, resp_body.len_string);

	// Error check
	if (json == NULL)
	{
		printf("ERROR: Failed to find valid JSON in given POST response in Cohost_Login()!\n");
		return NULL;
	}

	// Populate user info
	json = cJSON_GetObjectItemCaseSensitive(json, "result");
	json = cJSON_GetObjectItemCaseSensitive(json, "data");
	json_item = cJSON_GetObjectItemCaseSensitive(json, "projectHandle");

	// Set the values on the session we allocated
	session->user_id = cJSON_GetObjectItemCaseSensitive(json, "userId")->valueint;
	session->project_id = cJSON_GetObjectItemCaseSensitive(json, "projectId")->valueint;

	if (cJSON_GetObjectItemCaseSensitive(json, "loggedIn")->valueint > 0) session->flags |= USERFLAG_LOGGEDIN;
	if (cJSON_GetObjectItemCaseSensitive(json, "activated")->valueint > 0) session->flags |= USERFLAG_ACTIVATED;
	if (cJSON_GetObjectItemCaseSensitive(json, "readOnly")->valueint > 0) session->flags |= USERFLAG_READONLY;
	if (cJSON_GetObjectItemCaseSensitive(json, "modMode")->valueint > 0) session->flags |= USERFLAG_MODMODE;

	session->email = calloc(strlen(email) + 1, sizeof(char));
	memcpy(session->email, email, strlen(email));
	session->email[strlen(email) + 1] = '\0';

	session->session_id = calloc(strlen(token) + 1, sizeof(char));
	memcpy(session->session_id, token, strlen(token));
	session->session_id[strlen(token) + 1] = '\0';

	session->project_handle = calloc(strlen(json_item->valuestring) + 1, sizeof(char));
	memcpy(session->project_handle, json_item->valuestring, strlen(json_item->valuestring));
	session->project_handle[strlen(json_item->valuestring)] = '\0';

	// Free the memory we used
	curl_slist_free_all(headers);
	cJSON_Delete(json);
	free(resp_body.string);
	free(resp_head.string);
	free(url);
	free(salt);
	free(clienthash);
	free(post);

	// Return a pointer to the session
	return session;
}

// Attempt to login to cohost.org with a cookie and return a session context
cohost_session_t *Cohost_LoginWithCookie(char *cookie_load_filename, char *cookie_save_filename, CURL *curl)
{
	// Variables
	cohost_session_t *session;
	curl_response_t resp_body;
	curl_response_t resp_head;
	CURLcode resp_code;
	FILE *cookie_file;
	int i;

	// Sanity checks
	if (curl == NULL)
	{
		printf("ERROR: Provided CURL context was not properly initialized!\n");
		return NULL;
	}

	if (cookie_load_filename == NULL)
	{
		printf("ERROR: Provided credentials filename string is NULL!\n");
		return NULL;
	}

	if ((cookie_file = fopen(cookie_load_filename, "r")) == NULL)
	{
		printf("ERROR: Failed to find credentials file!\n");
		return NULL;
	}
	else
	{
		fclose(cookie_file);
	}

	// Allocate data
	session = calloc(1, sizeof(cohost_session_t));

	// Set CURLOPTs and send GET request for user info
	CURL_ResponseAllocate(&resp_body);
	CURL_ResponseAllocate(&resp_head);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie_load_filename);
	#ifndef __DJGPP__
	if (cookie_save_filename != NULL) curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookie_save_filename);
	#endif
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURL_ResponseCatch);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp_body);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, CURL_ResponseCatch);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &resp_head);
	curl_easy_setopt(curl, CURLOPT_URL, "https://cohost.org/api/v1/trpc/login.loggedIn");
	resp_code = curl_easy_perform(curl);

	// Error check
	if (resp_code != CURLE_OK)
	{
		printf("ERROR: curl_easy_perform() failed with error \"%s\"\n", curl_easy_strerror(resp_code));
		return NULL;
	}

	// Debug print
	printf("%s\n", resp_body.string);
	printf("%s\n", resp_head.string);

	// Free the memory we used
	free(resp_body.string);
	free(resp_head.string);

	return session;
}

// Close the connection and destroy the provided session context.
void Cohost_Destroy(cohost_session_t *session)
{
	if (session == NULL) return;
	if (session->session_id) free(session->session_id);
	if (session->project_handle) free(session->project_handle);
	if (session->email) free(session->email);
	if (session->cookies) curl_slist_free_all(session->cookies);
	if (session) free(session);
}

// Shutdown CURL
void Cohost_Shutdown(CURL *curl)
{
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

//
// Cryptography functions
//

// Generate the appropriate sha384 hash from the password and salt
char *Cryptography_GenerateClientHash(char *password, char *salt)
{
	// variables
	char *clienthash;

	// allocate hash buffer
	clienthash = calloc(128, sizeof(char));

	#ifndef __DJGPP__

	// use nettle to generate it
	nettle_pbkdf2_hmac_sha384(strlen(password), password, 200000, strlen(salt), salt, 128, clienthash);

	#endif

	// Encode clienthash
	return Base64_Encode((unsigned char *)clienthash, strlen(clienthash));
}

// Process and decode the salt value
char *Cryptography_DecodeSalt(curl_response_t *response)
{
	// Variables
	cJSON *json;
	char *salt;
	int salt_decoded_len;
	char *salt_decoded;

	// Parse json
	json = cJSON_ParseWithLength(response->string, response->len_string);

	// Error check
	if (json == NULL)
	{
		printf("Failed to find parse CURL response string into JSON in Cohost_DecodeSalt()!\n");
		return NULL;
	}

	json = cJSON_GetObjectItemCaseSensitive(json, "salt");

	// Error check
	if (json == NULL)
	{
		printf("Failed to find salt in given CURL response in Cohost_DecodeSalt()!\n");
		return NULL;
	}

	// Modify the salt hash appropriately
	salt = calloc(strlen(json->valuestring) + 3, sizeof(char));
	strncpy(salt, json->valuestring, strlen(json->valuestring) + 1);
	salt = Utility_ReplaceCharInString(salt, '-', 'A');
	salt = Utility_ReplaceCharInString(salt, '_', 'A');
	strcat(salt, "==");

	salt_decoded_len = Base64_DecodedSize(salt) + 1;
	salt_decoded = calloc(salt_decoded_len, sizeof(char));

	// Error check
	if (!Base64_Decode(salt, (unsigned char *)salt_decoded, salt_decoded_len))
	{
		printf("Failed to decode Base64 in Cohost_DecodeSalt()!\n");
		return NULL;
	}

	return salt_decoded;
}

//
// Base64 functions
//
// https://nachtimwald.com/2017/11/18/base64-encode-and-decode-in-c/
//

const char Base64_Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int Base64_Invs[] = { 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58,
	59, 60, 61, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
	21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
	29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
	43, 44, 45, 46, 47, 48, 49, 50, 51 };

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
	out = malloc(elen + 1);
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

//
// CURL response functions
//
// https://stackoverflow.com/questions/2329571/c-libcurl-get-output-into-a-string
//

void CURL_ResponseAllocate(curl_response_t *response)
{
	response->len_string = 0;
	response->string = malloc(response->len_string + 1);

	if (response->string == NULL)
	{
		fprintf(stderr, "malloc failed in CURL_ResponseAllocate()!\n");
		exit(EXIT_FAILURE);
	}

	response->string[0] = '\0';
}

size_t CURL_ResponseCatch(void *pointer, size_t size, size_t nmemb, curl_response_t *response)
{
	size_t new_length = response->len_string + size * nmemb;

	response->string = realloc(response->string, new_length + 1);

	if (response->string == NULL)
	{
		fprintf(stderr, "realloc failed in CURL_ResponseCatch()!\n");
		exit(EXIT_FAILURE);
	}

	memcpy(response->string + response->len_string, pointer, size * nmemb);

	response->string[new_length] = '\0';
	response->len_string = new_length;

	return size * nmemb;
}

//
// Utility functions
//

char *Utility_ReplaceCharInString(char *string, char find, char replace)
{
	char *current_pos = strchr(string, find);
 
	while (current_pos)
	{
		*current_pos = replace;
		current_pos = strchr(current_pos + 1, find);
	}

	return string;
}
