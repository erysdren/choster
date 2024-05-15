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
#include <stdlib.h>
#include <stdarg.h>

#include "libcohost.h"

/*
 *
 * macros
 *
 */

#define TITLE "choster"
#define VERSION "v0.0.1"
#define AUTHOR "erysdren"

#define WIDTH (640)
#define HEIGHT (480)
#define DEPTH (8)

#define UNUSED(x) ((void)(x))

/*
 *
 * globals
 *
 */

libcohost_session_t session = {0};

/*
 *
 * shutdown everything
 *
 */

void quit(int code)
{
	/* destroy libcohost session */
	libcohost_session_destroy(&session);

	/* shutdown libcohost */
	libcohost_quit();

	/* exit program */
	exit(code);
}

/*
 *
 * log functions
 *
 */

/* log an error and kill the program */
void log_error(const char *component, const char *fmt, ...)
{
	va_list list;
	static char buffer[1024];

	va_start(list, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, list);
	va_end(list);

	fprintf(stderr, "error: %s: %s\n", component, buffer);

	quit(EXIT_FAILURE);
}

/* log debug info to stderr */
void log_debug(const char *component, const char *fmt, ...)
{
	va_list list;
	static char buffer[1024];

	va_start(list, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, list);
	va_end(list);

	fprintf(stderr, "debug: %s: %s\n", component, buffer);
}

/* log general info to stdout */
void log_info(const char *component, const char *fmt, ...)
{
	va_list list;
	static char buffer[1024];

	va_start(list, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, list);
	va_end(list);

	fprintf(stdout, "info: %s: %s\n", component, buffer);
}

/*
 *
 * utility functions
 *
 */

void print_banner(void)
{
	fprintf(stdout,
		"====================================\n"
		TITLE " " VERSION " by " AUTHOR "\n"
		"====================================\n");
	fflush(stdout);
}

/*
 *
 * io functions
 *
 */

void *io_alloc(size_t sz)
{
	return calloc(1, sz);
}

void io_free(void *ptr)
{
	free(ptr);
}

/*
 *
 * main
 *
 */

int main(int argc, char **argv)
{
	int r;

	UNUSED(argc);
	UNUSED(argv);

	print_banner();

	/* check arg count */
	if (argc != 3)
		log_error(TITLE, "incorrect number of command line arguments");

	/* startup */
	r = libcohost_init();
	if (r != LIBCOHOST_RESULT_OK)
		log_error("libcohost", libcohost_result_string(r));
	else
		log_info("libcohost", "successfully initialized");

	/* set io callbacks */
	libcohost_set_io(io_alloc, io_free);

	/* create session */
	r = libcohost_session_new(&session, argv[1], argv[2], NULL);
	if (r != LIBCOHOST_RESULT_OK)
		log_error("libcohost", libcohost_result_string(r));
	else
		log_info("libcohost", "successfully created session");

	/* shutdown */
	quit(EXIT_SUCCESS);

	return 0;
}
