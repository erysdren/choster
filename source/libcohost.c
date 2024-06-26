/*
ANTI-CAPITALIST SOFTWARE LICENSE (v 1.4)

Copyright (c) 2022-2024 erysdren (it/she/they)

This is anti-capitalist software, released for free use by individuals
and organizations that do not operate by capitalist principles.

Permission is hereby granted, free of charge, to any person or
organization (the "User") obtaining a copy of this software and
associated documentation files (the "Software"), to use, copy, modify,
merge, distribute, and/or sell copies of the Software, subject to the
following conditions:

  1. The above copyright notice and this permission notice shall be
  included in all copies or modified versions of the Software.

  2. The User is one of the following:
    a. An individual person, laboring for themselves
    b. A non-profit organization
    c. An educational institution
    d. An organization that seeks shared profit for all of its members,
    and allows non-members to set the cost of their labor

  3. If the User is an organization with owners, then all owners are
  workers and all workers are owners with equal equity and/or equal vote.

  4. If the User is an organization, then the User is not law enforcement
  or military, or working for or under either.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT EXPRESS OR IMPLIED WARRANTY OF
ANY KIND, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#include "thirdparty/cJSON.h"

#include "libcohost.h"

#define ASIZE(a) (sizeof(a)/sizeof(a[0]))
#define UNUSED(x) ((void)(x))

/* these must always end with a forward slash */
#define COHOST_API_BASE "https://cohost.org/api/v1/"
#define COHOST_API_LOGIN COHOST_API_BASE "login/"

/* curl response catcher */
typedef struct curl_response_t {
	char *string;
	size_t len_string;
} curl_response_t;

/* startup library */
int libcohost_init(void)
{
	if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0)
		return LIBCOHOST_RESULT_CURL_INIT_FAIL;

	return LIBCOHOST_RESULT_OK;
}

/* shutdown library */
void libcohost_quit(void)
{
	curl_global_cleanup();
}

/* get a string representing a function result */
const char *libcohost_result_string(int r)
{
	static const char *results[] = {
		"Ok",
		"General library failure",
		"Memory allocation failure",
		"Couldn't connect to cohost.org",
		"Bad login credentials",
		"Failed to initialize libcurl",
		"General libcurl failure"
	};

	if (r < 0 || r >= (int)ASIZE(results))
		return NULL;

	return results[r];
}

/* free curl response */
static void curl_response_free(curl_response_t *response)
{
	if (response->string)
		free(response->string);

	response->len_string = 0;
}

/* catch curl response */
static size_t curl_response_catch(void *pointer, size_t size, size_t nmemb, curl_response_t *response)
{
	size_t new_length = size * nmemb;

	/* setup response string */
	response->string = malloc(new_length + 1);
	response->string[new_length] = '\0';
	response->len_string = new_length;

	/* copy in response */
	memcpy(response->string, pointer, new_length);

	return new_length;
}

/* create a new cohost session */
int libcohost_session_new(libcohost_session_t *session, char *email, char *password, char *cookie_save_filename)
{
	static char url[512];
	static char post[1024];
	curl_response_t resp_body = {0};
	curl_response_t resp_head = {0};
	cJSON *json = NULL;
	cJSON *json_item = NULL;

	UNUSED(cookie_save_filename);

	if (email == NULL || password == NULL)
		return LIBCOHOST_RESULT_BAD_CREDENTIALS;

	/* setup curl */
	session->curl = curl_easy_init();
	if (session->curl == NULL)
		return LIBCOHOST_RESULT_CURL_INIT_FAIL;

	/* setup url for GET request */
	snprintf(url, sizeof(url), COHOST_API_LOGIN "salt?email=%s", email);

	/* set CURLOPTs and send GET request */
	curl_easy_setopt(session->curl, CURLOPT_URL, url);
	curl_easy_setopt(session->curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(session->curl, CURLOPT_COOKIEFILE, "");
	curl_easy_setopt(session->curl, CURLOPT_WRITEFUNCTION, curl_response_catch);
	curl_easy_setopt(session->curl, CURLOPT_WRITEDATA, &resp_body);
	curl_easy_setopt(session->curl, CURLOPT_HEADERFUNCTION, curl_response_catch);
	curl_easy_setopt(session->curl, CURLOPT_HEADERDATA, &resp_head);
	if (curl_easy_perform(session->curl) != CURLE_OK)
		return LIBCOHOST_RESULT_CURL_FAIL;

	/* free response structs */
	curl_response_free(&resp_head);
	curl_response_free(&resp_body);

	return LIBCOHOST_RESULT_OK;
}

/* destroy an active session */
void libcohost_session_destroy(libcohost_session_t *session)
{
	if (session)
	{
		if (session->curl) curl_easy_cleanup(session->curl);
		if (session->session_id) free(session->session_id);
	}
}

/* replace char in string */
static char *str_replace(char *string, char find, char replace)
{
	char *current_pos = strchr(string, find);

	while (current_pos)
	{
		*current_pos = replace;
		current_pos = strchr(current_pos + 1, find);
	}

	return string;
}

