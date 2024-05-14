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

#ifndef _LIBCOHOST_H_
#define _LIBCOHOST_H_
#ifdef __cplusplus
extern "C" {
#endif

/* result types */
enum {
	LIBCOHOST_RESULT_OK,
	LIBCOHOST_RESULT_GENERAL_FAIL,
	LIBCOHOST_RESULT_ALLOC_FAIL,
	LIBCOHOST_RESULT_COULDNT_CONNECT,
	LIBCOHOST_RESULT_BAD_CREDENTIALS,
	LIBCOHOST_RESULT_CURL_INIT_FAIL
};

/* session flags */
enum {
	LIBCOHOST_FLAG_LOGGEDIN = 1 << 0,
	LIBCOHOST_FLAG_ACTIVATED = 1 << 1,
	LIBCOHOST_FLAG_READONLY = 1 << 2,
	LIBCOHOST_FLAG_MODMODE = 1 << 3
};

/* cohost session */
typedef struct libcohost_session_t {
	unsigned int user_id;
	unsigned int project_id;
	unsigned int flags;
	char *session_id;
	void *curl;
} libcohost_session_t;

/* startup library */
int libcohost_init(void);

/* shutdown library */
void libcohost_quit(void);

/* get a string representing a function result */
const char *libcohost_result_string(int r);

/* login to cohost.org */
int libcohost_session_new(libcohost_session_t *session, char *email, char *password, char *cookie_save_filename);

/* destroy an active session */
void libcohost_session_destroy(libcohost_session_t *session);

#ifdef __cplusplus
}
#endif
#endif /* _LIBCOHOST_H_ */
