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

#include "eui_sdl2.h"
#include "palette_vga.h"
#include "SDL.h"

/*
 *
 * macros
 *
 */

#define NAME "choster"
#define VERSION "v0.0.1"
#define AUTHOR "erysdren"
#define TITLE NAME " " VERSION " by " AUTHOR

#define WIDTH (640)
#define HEIGHT (480)
#define DEPTH (8)

#define UNUSED(x) ((void)(x))

/*
 *
 * globals
 *
 */

static libcohost_session_t session;

static SDL_Window *window;
static SDL_Surface *surface8;
static SDL_Surface *surface32;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static SDL_Rect rect;
static SDL_Color colors[256];
static SDL_Event event;

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

	/* eui */
	eui_quit();

	/* sdl */
	SDL_FreeSurface(surface8);
	SDL_FreeSurface(surface32);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

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
		TITLE "\n"
		"====================================\n");
	fflush(stdout);
}

/*
 *
 * gfx handling
 *
 */

void gfx_init(void)
{
	uint32_t format;
	unsigned int rmask, gmask, bmask, amask;
	int bpp, i;

	/* init */
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		log_error("SDL", SDL_GetError());

	/* create window */
	window = SDL_CreateWindow(TITLE,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WIDTH, HEIGHT,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

	/* create renderer */
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);
	SDL_RenderSetIntegerScale(renderer, SDL_TRUE);
	SDL_SetWindowMinimumSize(window, WIDTH, HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	/* create our render surface */
	surface8 = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 8, 0, 0, 0, 0);
	SDL_FillRect(surface8, NULL, 0);

	/* create our render surface */
	surface8 = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 8, 0, 0, 0, 0);
	SDL_FillRect(surface8, NULL, 0);

	/* init palette */
	for (i = 0; i < 256; i++)
	{
		colors[i].r = palette_vga[i * 3];
		colors[i].g = palette_vga[i * 3 + 1];
		colors[i].b = palette_vga[i * 3 + 2];
	}

	/* install palette */
	SDL_SetPaletteColors(surface8->format->palette, colors, 0, 256);

	/* create display surface */
	format = SDL_GetWindowPixelFormat(window);
	SDL_PixelFormatEnumToMasks(format, &bpp, &rmask, &gmask, &bmask, &amask);
	surface32 = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, bpp, rmask, gmask, bmask, amask);
	texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

	/* make sure relative mouse mode is disabled */
	SDL_SetRelativeMouseMode(SDL_FALSE);

	/* setup blit rect */
	rect.x = 0;
	rect.y = 0;
	rect.w = WIDTH;
	rect.h = HEIGHT;

	/* init eui */
	eui_init(surface8->w, surface8->h, surface8->format->BitsPerPixel, surface8->pitch, surface8->pixels);
}

void gfx_main(void)
{
	/* clear screen */
	eui_screen_clear(0x01);

	/* set alignment to the center of the frame */
	eui_frame_align_set(EUI_ALIGN_MIDDLE, EUI_ALIGN_MIDDLE);

	/* draw 192x64 box and draw a 2px border */
	eui_draw_box(0, 0, 192, 64, 0x0F);
	eui_draw_box_border(0, 0, 192, 64, 2, 0x02);

	/* create child frame with 8 pixels of padding on each side */
	eui_frame_push(0, 0, 176, 48);

	/* set child frame alignment to top-left */
	eui_frame_align_set(EUI_ALIGN_START, EUI_ALIGN_START);

	/* draw a string with the default 8x8 font */
	eui_draw_text(0, 0, 0x00, "Hello, world!");

	/* destroy child frame */
	eui_frame_pop();
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

	/* create session */
	r = libcohost_session_new(&session, argv[1], argv[2], NULL);
	if (r != LIBCOHOST_RESULT_OK)
		log_error("libcohost", libcohost_result_string(r));
	else
		log_info("libcohost", "successfully created session");

	/* create window */
	gfx_init();

	/* main loop */
	while (!SDL_QuitRequested())
	{
		/* push events */
		while (SDL_PollEvent(&event))
			eui_sdl2_event_push(&event);

		/* process events */
		eui_event_queue_process();

		/* clear screen */
		SDL_FillRect(surface8, NULL, 0x00);

		/* run eui context */
		if (eui_context_begin())
		{
			/* do main program */
			gfx_main();

			/* end eui context */
			eui_context_end();
		}

		/* copy to screen */
		SDL_BlitSurface(surface8, &rect, surface32, &rect);
		SDL_UpdateTexture(texture, NULL, surface32->pixels, surface32->pitch);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	/* shutdown */
	quit(EXIT_SUCCESS);

	return 0;
}
