#include "AboutWindow.h"
#include "Window.h"
#include "Config.h"

#include <stdio.h>
#include <imgui/imgui.h>
#include <SDL3/SDL_version.h>

AboutWindow::AboutWindow(Window* window, BaseUI* parentUi) : BaseUI(window, parentUi)
{
	const char* title = window->GetParams().title.c_str();
	snprintf(windowTitle, 64, "About %s - v%s", title, PROJECT_VER);
}

AboutWindow::~AboutWindow()
{

}

void AboutWindow::VisibilityChanged(const bool isVisible)
{
	BaseUI::VisibilityChanged(isVisible);

	if (isVisible)
	{
		ImGui::OpenPopup(windowTitle);
	}
}

void AboutWindow::DoRender()
{
	BaseUI::DoRender();

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(windowTitle, &isVisible, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
	{
		const WindowParams& params = window->GetParams();

		ImGui::Text(params.description.c_str());
		ImGui::NewLine();
		ImGui::Text("By %s:", params.author.c_str());
		ImGui::SameLine();
		ImGui::TextLinkOpenURL(params.url.c_str());
		ImGui::NewLine();

		ImGui::SeparatorText("Libraries");

		ImGui::BulletText("ImGui %s:", ImGui::GetVersion());
		ImGui::SameLine();
		ImGui::TextLinkOpenURL("https://github.com/ocornut/imgui");

		int sdlVersion = SDL_GetVersion();
		const std::string& driverName = window->GetDriverName();
		ImGui::BulletText("SDL %i.%i.%i (Lib %i.%i.%i, Backend \"%s\"):", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION, SDL_VERSIONNUM_MAJOR(sdlVersion), SDL_VERSIONNUM_MINOR(sdlVersion), SDL_VERSIONNUM_MICRO(sdlVersion), driverName.c_str());
		ImGui::SameLine();
		ImGui::TextLinkOpenURL("https://www.libsdl.org");

		ImGui::BulletText("simpleini (Commit %s):", SIMPLEINI_COMMIT_HASH);
		ImGui::SameLine();
		ImGui::TextLinkOpenURL("https://github.com/brofield/simpleini");

		ImGui::EndPopup();
	}
}