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

// Entry point
int main(int argc, char *argv[])
{
	// Variables
	bool b_running = true;
	auto window = StartupTUI();
	int window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus;

	// Cohost variables
	CohostUser user;
	bool b_logged_in = false;
	bool b_logging_in = false;
	int num_notifications = 0;
	int num_bookmarks = 0;
	static char email[64];
	static char password[64];

	// Yea
	user.cookies_save_filename = "chstterm.ini";

	// Try a cookie if exists
	if (FILE *test = fopen("chstterm.ini", "r"))
	{
		fclose(test);
		user.LoginWithCookieFile("chstterm.ini");
		b_logged_in = true;
	}

	while (b_running)
	{
		// Start new frame
		NewFrameTUI();

		// Main window
		{
			// Push styles
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.51f, 0.14f, 0.31f, 1.0f));

			// Set window properties
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

			// Begin window
			if (!ImGui::Begin("Cohost Terminal Interface v0.1", nullptr, window_flags))
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

			// Text
			if (ImGui::BeginTable("homepage", 1))
			{
				ImGui::TableSetupColumn(" Settings ", ImGuiTableColumnFlags_WidthFixed, 16.0f);

				ImGui::TableNextColumn();

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

				ImGui::EndTable();
			}

			// End ImGui processing
			ImGui::End();

			// Pop styles
			ImGui::PopStyleColor(2);
		}

		// Timeline window
		{
			// Push styles
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));

			// Set window properties
			ImGui::SetNextWindowPos(ImVec2(16, 3), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x - 17, ImGui::GetIO().DisplaySize.y - 3), ImGuiCond_Always);

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
			ImGui::SetNextWindowPos(ImVec2(4, 4), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(48, 10), ImGuiCond_Once);

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
				user.LoginWithEmailPass(email, password);
				b_logged_in = true;
			}

			// End window
			ImGui::End();

			// Pop styles
			ImGui::PopStyleColor(1);
		}

		// Render result
		RenderTUI(window);
	}

	// Shutdown
	ShutdownTUI();

	return EXIT_SUCCESS;
}
