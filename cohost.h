// ========================================================
//
// FILE:			/cohost.h
//
// AUTHORS:			Jaycie Ewald
//
// PROJECT:			libcohost
//
// LICENSE:			ACSL v1.4
//
// DESCRIPTION:		Cohost API header
//
// LAST EDITED:		November 22nd, 2022
//
// ========================================================

// https://cohost.org/iliana/post/180187-eggbug-rs-v0-1-3-d
// https://curl.se/libcurl/c/http-post.html
// https://curl.se/libcurl/c/simple.html
// https://curl.se/libcurl/c/post-callback.html
// https://github.com/valknight/Cohost.py/blob/main/cohost/models/user.py
// https://cohost.org/rc/tagged/cohost%20api
// https://github.com/iliana/eggbug-rs/blob/main/src/client.rs
// https://curl.se/libcurl/c/cookie_interface.html
// https://nachtimwald.com/2017/11/18/base64-encode-and-decode-in-c/

#ifndef __COHOST_H__
#define __COHOST_H__

// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Library headers
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <nettle/pbkdf2.h>

//
//
// Cohost API
//
//

//
// Datatypes
//

// Cohost Session struct
typedef struct
{
	int user_id;
	char *email;
	char *session_id;
	char *salt;
} cohost_session_t;

// CURL Response struct
typedef struct
{
	char *string;
	size_t len_string;
} curl_response_t;

//
// Public functions
//

CURL *Cohost_Initialize(void);
cohost_session_t *Cohost_Login(char *email, char *password, CURL *curl);
void Cohost_Destroy(cohost_session_t *session);
void Cohost_Shutdown(CURL *curl);

//
// Private functions
//

// Cryptography functions
char *Cryptography_GenerateClientHash(char *password, char *salt);
char *Cryptography_DecodeSalt(curl_response_t *response);

// Base64 functions
size_t Base64_EncodedSize(size_t inlen);
size_t Base64_DecodedSize(const char *in);
void Base64_GenerateDecodeTable(void);
int Base64_IsValidChar(char c);
char *Base64_Encode(const unsigned char *in, size_t len);
int Base64_Decode(const char *in, unsigned char *out, size_t outlen);

// Utility functions
char *Utility_ReplaceCharInString(char *string, char find, char replace);

// CURL Response handling
void CURL_ResponseAllocate(curl_response_t *response);
size_t CURL_ResponseCatch(void *pointer, size_t size, size_t nmemb, curl_response_t *response);

#endif // __COHOST_H__
