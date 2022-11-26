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

// Cohost Header
#include "cohost.hpp"

// ImGUI / ImTUI Headers
#include "imtui/imtui.h"
#include "imtui/imtui-impl-ncurses.h"

// Globals
CohostUser cohost_user;

// Initialize
auto tui_init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto window = ImTui_ImplNcurses_Init(true);
	ImTui_ImplText_Init();

	return window;
}

// Shutdown
void tui_shutdown()
{
	ImTui_ImplText_Shutdown();
	ImTui_ImplNcurses_Shutdown();
}

// Login window
void tui_window_login(bool *b_logged_in)
{
	// Variables
	static char email[64];
	static char password[64];

	// Set window properties
	ImGui::SetNextWindowPos(ImVec2(4, 4), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(48, 10), ImGuiCond_Once);

	// Push styles
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

	// Begin window
	if (!ImGui::Begin("Login", nullptr, ImGuiWindowFlags_NoCollapse))
		return;

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
		cohost_user.LoginWithEmailPass(email, password);
		*b_logged_in = true;
	}

	// End window
	ImGui::End();

	// Pop styles
	ImGui::PopStyleColor(1);
}

// Entry point
int main(int argc, char *argv[])
{
	// Variables
	bool b_running = true;
	auto window = tui_init();
	int nframes = 0;
	float fval = 1.23f;
	int window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus;

	// Cohost variables
	bool b_logged_in = false;
	bool b_logging_in = false;
	int num_notifications = 0;

	// Yea
	cohost_user.cookies_save_filename = "chstterm.ini";

	// Try a cookie if exists
	if (FILE *test = fopen("chstterm.ini", "r"))
	{
		fclose(test);
		cohost_user.LoginWithCookieFile("chstterm.ini");
		b_logged_in = true;
	}

	while (b_running)
	{
		// Start new frame
		ImTui_ImplNcurses_NewFrame();
		ImTui_ImplText_NewFrame();
		ImGui::NewFrame();

		// Set window properties
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

		// Push styles
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.51f, 0.14f, 0.31f, 1.0f));

		// Begin window
		if (!ImGui::Begin("Cohost Terminal Interface", nullptr, window_flags))
			break;

		// Menubar
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu(" Actions "))
			{
				if (ImGui::MenuItem(" Login ", "begin new session", nullptr, !b_logged_in)) b_logging_in = true;
				ImGui::MenuItem(" Post ", "chost like a champ", nullptr, b_logged_in);

				ImGui::NewLine();
				if (ImGui::MenuItem(" Quit ", "stop chosting")) b_running = false;
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		// Text
		if (ImGui::BeginTable("homepage", 3))
		{
			ImGui::TableNextColumn();

			ImGui::NewLine();
			ImGui::Button(" Notifications ");
			ImGui::SameLine();
			ImGui::Text(" ( %d )", num_notifications);
			ImGui::NewLine();
			ImGui::Button(" Bookmarked Tags ");
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

			ImGui::TableNextColumn();

			/*
			ImGui::Text("Demo text:");
			ImGui::NewLine();
			ImGui::Text("NFrames = %d", nframes++);
			ImGui::Text("Mouse Pos : x = %g, y = %g", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
			ImGui::Text("Time per frame %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Float:");
			ImGui::SameLine();
			ImGui::SliderFloat("##float", &fval, 0.0f, 10.0f);
			*/

			ImGui::EndTable();
		}

		// End ImGui processing
		ImGui::End();

		// Pop styles
		ImGui::PopStyleColor(2);

		if (b_logging_in == true && b_logged_in == false)
		{
			tui_window_login(&b_logged_in);
		}
	
		// Render result
		ImGui::Render();
		ImTui_ImplText_RenderDrawData(ImGui::GetDrawData(), window);
		ImTui_ImplNcurses_DrawScreen();
	}

	tui_shutdown();

	return EXIT_SUCCESS;
}
