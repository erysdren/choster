// ========================================================
//
// FILE:			/network.c
//
// AUTHORS:			Jaycie Ewald
//
// PROJECT:			libcohost
//
// LICENSE:			ACSL v1.4
//
// DESCRIPTION:		Networking functions
//
// LAST EDITED:		November 20th, 2022
//
// ========================================================

#include "cohost.h"

CURLcode login_post_request(CURL *curl, char *email, char *clienthash)
{
	// variables
	struct curl_slist *headers = NULL;
	char *post;

	// generate POST request string
	post = calloc(1024, sizeof(char));
	snprintf(post, 1024, "{ \"email\" : \"%s\", \"clientHash\" : \"%s\" }", email, clienthash);

	// setup POST headers
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Expect:");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charset: utf-8");

	// set curlopts
	curl_easy_setopt(curl, CURLOPT_URL, "https://cohost.org/api/v1/login");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);

	// return response code
	return curl_easy_perform(curl);
}
