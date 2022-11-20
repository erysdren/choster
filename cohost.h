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
// LAST EDITED:		November 20th, 2022
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

// Response data struct
typedef struct
{
	char *string;
	size_t len_string;
} response_t;

// Base64 handling (base64.c)
size_t b64_encoded_size(size_t inlen);
size_t b64_decoded_size(const char *in);
void b64_generate_decode_table();
int b64_isvalidchar(char c);
char *b64_encode(const unsigned char *in, size_t len);
int b64_decode(const char *in, unsigned char *out, size_t outlen);

// Cohost API functions (cohost.c)
void Cohost_Login(char *email, char *password);

// Hash handling (hash.c)
char *hash_generate(char *password, char *salt);

// Network handling (network.c)
CURLcode login_post_request(CURL *curl, char *email, char *clienthash);

// Response handling (response.c)
void response_allocate(response_t *str);
size_t response_catch(void *pointer, size_t size, size_t nmemb, response_t *response);

// Salt handling (salt.c)
char *salt_process(response_t *response);
char *salt_decode(char *salt);

// String utilities (strutils.c)
char *replace_char(char *string, char find, char replace);

#endif // __COHOST_H__