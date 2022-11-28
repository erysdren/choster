// ========================================================
//
// FILE:			/apps/chstterm.cpp
//
// AUTHORS:			Jaycie Ewald
//
// PROJECT:			libcohost
//
// LICENSE:			ACSL v1.4
//
// DESCRIPTION:		Cohost terminal app (CXX)
//
// LAST EDITED:		November 26th, 2022
//
// ========================================================

// Determine which version to build
#ifndef GRAPHICS
#define TERMINAL
#endif

// Cohost Header
#include "cohost.hpp"

#define TITLE "Cohost Terminal Interface v0.1"

// ImGUI / ImTUI Headers
#ifdef GRAPHICS
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "chfontr.h"
#include "chfontb.h"
#include "chfonti.h"
#include "chfontbi.h"
#define INI_FILENAME "chstgrph.ini"
#else
#include "imtui/imtui.h"
#include "imtui/imtui-impl-ncurses.h"
#define INI_FILENAME "chstterm.ini"
#endif

#ifdef GRAPHICS

#define FONT_WIDTH ImGui::GetFontSize() * 0.5f
#define FONT_HEIGHT ImGui::GetFontSize()

ImGuiIO &StartupSDL(SDL_Window **window, SDL_GLContext *context)
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		exit(-1);
	}

	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window* win = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
	SDL_GLContext ctx = SDL_GL_CreateContext(win);
	SDL_GL_MakeCurrent(win, ctx);
	SDL_GL_SetSwapInterval(1);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	io.Fonts->AddFontFromMemoryTTF(Atkinson_Hyperlegible_Regular_102_ttf, Atkinson_Hyperlegible_Regular_102_ttf_len, 24.0f, NULL, NULL);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(win, ctx);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Return values
	*window = win;
	*context = ctx;
	return io;
}

void ShutdownSDL(SDL_Window *window, SDL_GLContext context)
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void RenderSDL(SDL_Window *window, ImGuiIO &io, ImVec4 clear_color)
{
	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(window);
}

void NewFrameSDL(SDL_Window *window)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();
}

void PollEventsSDL(bool *running, SDL_Window *window)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		if (event.type == SDL_QUIT)
			*running = false;
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
			*running = false;
	}
}

#else

#define FONT_WIDTH 1
#define FONT_HEIGHT 1

// Initialize
auto StartupTUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto window = ImTui_ImplNcurses_Init(true);
	ImTui_ImplText_Init();

	return window;
}

// Shutdown
void ShutdownTUI()
{
	ImTui_ImplText_Shutdown();
	ImTui_ImplNcurses_Shutdown();
}

// Start a new frame
void NewFrameTUI()
{
	ImTui_ImplNcurses_NewFrame();
	ImTui_ImplText_NewFrame();
	ImGui::NewFrame();
}

// Render frame
void RenderTUI(ImTui::TScreen *window)
{
	ImGui::Render();
	ImTui_ImplText_RenderDrawData(ImGui::GetDrawData(), window);
	ImTui_ImplNcurses_DrawScreen();
}

#endif

// Entry point
int main(int argc, char *argv[])
{
	// Variables
	bool b_running = true;

	#ifdef GRAPHICS
	SDL_Window *window;
	SDL_GLContext gl_context;
	ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	ImGuiIO &io = StartupSDL(&window, &gl_context);
	float font_scale = 1.0f;
	#else
	auto window = StartupTUI();
	#endif

	// Cohost variables
	bool b_logged_in = false;
	bool b_logging_in = false;
	int num_notifications = 0;
	int num_bookmarks = 0;
	static char email[64];
	static char password[64];

	// Set cookie filename
	Cohost::SetStringProperty(CohostStringProp_CookieFile, INI_FILENAME);
	Cohost::Initialize();

	// Try a cookie if exists
	if (Cohost::LoginWithCookieFile(INI_FILENAME) == true)
		b_logged_in = true;

	while (b_running)
	{
		// Start new frame & poll events
		#ifdef GRAPHICS
		PollEventsSDL(&b_running, window);
		NewFrameSDL(window);
		ImGui::GetIO().FontGlobalScale = font_scale;
		#else
		NewFrameTUI();
		#endif

		// Main window
		{
			// Push styles
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.51f, 0.14f, 0.31f, 1.0f));

			// Set window properties
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

			// Begin window
			if (!ImGui::Begin(TITLE, &b_running, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus))
				break;

			// Menubar
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Actions "))
				{
					if (ImGui::MenuItem(" Login ", "begin new session", nullptr, !b_logged_in)) b_logging_in = true;
					ImGui::MenuItem(" Post ", "chost like a champ", nullptr, b_logged_in);

					ImGui::NewLine();
					if (ImGui::MenuItem(" Quit ", "stop chosting")) b_running = false;
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
		}

		// Left sidebar window
		{
			// Set window properties
			ImGui::SetNextWindowPos(ImVec2(0, 3 * FONT_HEIGHT), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(15.0f * FONT_WIDTH, ImGui::GetIO().DisplaySize.y - (3.0f * FONT_HEIGHT)), ImGuiCond_Always);

			// Begin window
			if (!ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
				return EXIT_FAILURE;

			// Window elements
			ImGui::NewLine();
			ImGui::Button(" Notifs ");
			ImGui::SameLine();
			ImGui::Text("(%d)", num_notifications);
			ImGui::NewLine();
			ImGui::Button(" Bookmarks ");
			ImGui::NewLine();
			ImGui::Button(" Search ");
			ImGui::NewLine();
			ImGui::Button(" Profile ");
			ImGui::NewLine();
			ImGui::Button(" Drafts ");
			ImGui::NewLine();
			ImGui::Button(" Following ");
			ImGui::NewLine();
			ImGui::Button(" Followers ");
			ImGui::NewLine();
			ImGui::Button(" Settings ");
			#ifdef GRAPHICS
			ImGui::NewLine();
			ImGui::Text("Font Scale:");
			ImGui::SliderFloat("##fontscale", &font_scale, 1.0f, 2.0f);
			#endif

			// End window
			ImGui::End();
		}

		// Timeline window
		{
			// Push styles
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));

			// Set window properties
			ImGui::SetNextWindowPos(ImVec2(16 * FONT_WIDTH, 3 * FONT_HEIGHT), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x - (17.0f * FONT_WIDTH), ImGui::GetIO().DisplaySize.y - (3.0f * FONT_HEIGHT)), ImGuiCond_Always);

			// Begin window
			if (!ImGui::Begin("Timeline", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
				return EXIT_FAILURE;

			// Window elements
			ImGui::NewLine();
			ImGui::TextWrapped("Im trying to explain how i came to the understanding that this reality here on earth is truly a matrix and that there is a reptilian race from the constellation of astro-world who are controlling virtual reality here on earth.");
			ImGui::NewLine();
			ImGui::TextWrapped("reincarnation here has nothing to do with our spiritual growth from cradle to grave and beyond and were never going to get out of this situation without planet x. planet x is not a catastrophe, it is a prison break.");
			ImGui::NewLine();
			ImGui::TextWrapped("the force of this planet as it tears apart the electro-magnetic force field that surrounds the earth will finally reveal the matrix and everyone is going to see it; there will be no doubt, there will be no fighting over belief systems, we are going to have all of the masks pulled away. that is the future for us with planet x.");
			ImGui::NewLine();
			ImGui::TextWrapped("Im trying to explain how i came to the understanding that this reality here on earth is truly a matrix and that there is a reptilian race from the constellation of astro-world who are controlling virtual reality here on earth.");
			ImGui::NewLine();
			ImGui::TextWrapped("reincarnation here has nothing to do with our spiritual growth from cradle to grave and beyond and were never going to get out of this situation without planet x. planet x is not a catastrophe, it is a prison break.");
			ImGui::NewLine();
			ImGui::TextWrapped("the force of this planet as it tears apart the electro-magnetic force field that surrounds the earth will finally reveal the matrix and everyone is going to see it; there will be no doubt, there will be no fighting over belief systems, we are going to have all of the masks pulled away. that is the future for us with planet x.");

			// End window
			ImGui::End();

			// Pop styles
			ImGui::PopStyleColor(1);
		}

		// Login window
		if (b_logging_in == true && b_logged_in == false)
		{
			// Push styles
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

			// Set window properties
			ImGui::SetNextWindowPos(ImVec2(4 * FONT_WIDTH, 4 * FONT_HEIGHT), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(48 * FONT_WIDTH, 10 * FONT_HEIGHT), ImGuiCond_Once);

			// Begin window
			if (!ImGui::Begin("Login", nullptr, ImGuiWindowFlags_NoCollapse))
				return EXIT_FAILURE;

			// Window elements
			ImGui::NewLine();
			ImGui::Text("Please enter your email and password:");
			ImGui::NewLine();
			ImGui::InputText("Email", email, IM_ARRAYSIZE(email), ImGuiInputTextFlags_None);
			ImGui::NewLine();
			ImGui::InputText("Password", password, IM_ARRAYSIZE(password), ImGuiInputTextFlags_Password | ImGuiInputTextFlags_CharsNoBlank);
			ImGui::NewLine();

			if (ImGui::Button(" Login ") && strlen(password) > 0 && strlen(email) > 0)
			{
				if (Cohost::LoginWithEmailPass(email, password) == true)
					b_logged_in = true;
			}

			// End window
			ImGui::End();

			// Pop styles
			ImGui::PopStyleColor(1);
		}

		// Render result
		#ifdef GRAPHICS
		RenderSDL(window, io, clear_color);
		#else
		RenderTUI(window);
		#endif
	}

	// Shutdown

	#ifdef GRAPHICS
	ShutdownSDL(window, gl_context);
	#else
	Cohost::Shutdown();
	ShutdownTUI();
	#endif

	return EXIT_SUCCESS;
}
