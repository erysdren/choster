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

#include <stdio.h>
#include "libcohost.h"

#define TITLE "choster"
#define VERSION "v0.0.1"
#define AUTHOR "erysdren"

#define WIDTH (640)
#define HEIGHT (480)
#define DEPTH (8)

#define UNUSED(x) ((void)(x))

int main(int argc, char **argv)
{
	int r;
	libcohost_session_t session;

	UNUSED(argc);
	UNUSED(argv);

	printf(TITLE " " VERSION " by " AUTHOR "\n");

	/* startup */
	if ((r = libcohost_init()) != LIBCOHOST_RESULT_OK)
	{
		printf("libcohost error: %s\n", libcohost_result_string(r));
		return 1;
	}
	else
	{
		printf("successfully initialized libcohost\n");
	}

	/* create session */
	if ((r = libcohost_session_new(&session, NULL, NULL, NULL)) != LIBCOHOST_RESULT_OK)
	{
		printf("libcohost error: %s\n", libcohost_result_string(r));
		return 1;
	}
	else
	{
		printf("successfully created libcohost session\n");
	}

	/* destroy session */
	libcohost_session_destroy(&session);

	/* shutdown */
	libcohost_quit();

	return 0;
}
