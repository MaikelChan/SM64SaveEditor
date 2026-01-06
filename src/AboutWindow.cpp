#include "AboutWindow.h"
#include "main.h"
#include "Config.h"

#include <stdio.h>
#include <imgui/imgui.h>
#include <SDL3/SDL_version.h>

AboutWindow::AboutWindow(const BaseUI* parentUI) : BaseUI(parentUI)
{
	snprintf(windowTitle, 64, "About %s", WINDOW_TITLE);
	snprintf(description, 64, "%s - v%s", WINDOW_TITLE, PROJECT_VER);
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
		ImGui::SeparatorText(description);
		ImGui::Text("By PacoChan.");
		ImGui::SameLine();
		ImGui::TextLinkOpenURL("https://pacochan.net/software/sm64-save-editor/");
		ImGui::Text("\nThis save editor is compatible with the Nintendo 64 version and the PC port.\n\n");

		ImGui::SeparatorText("Libraries");

		ImGui::BulletText("ImGui %s:", ImGui::GetVersion());
		ImGui::SameLine();
		ImGui::TextLinkOpenURL("https://github.com/ocornut/imgui");

		int sdlVersion = SDL_GetVersion();
		const char* backend = GetBackend();
		if (backend == nullptr) backend = "?";
		ImGui::BulletText("SDL %i.%i.%i (Lib %i.%i.%i, Backend \"%s\"):", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION, SDL_VERSIONNUM_MAJOR(sdlVersion), SDL_VERSIONNUM_MINOR(sdlVersion), SDL_VERSIONNUM_MICRO(sdlVersion), GetBackend());
		ImGui::SameLine();
		ImGui::TextLinkOpenURL("https://www.libsdl.org");

		ImGui::BulletText("simpleini (Commit %s):", SIMPLEINI_COMMIT_HASH);
		ImGui::SameLine();
		ImGui::TextLinkOpenURL("https://github.com/brofield/simpleini");

		ImGui::EndPopup();
	}
}