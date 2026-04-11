#include "AboutWindow.h"

#include <imgui/imgui.h>

#include "Config.h"
#include "Window.h"

AboutWindow::AboutWindow(Window* window, BaseUI* parentUi) : BaseUI(window, parentUi)
{
	const char* title = window->GetParams().title.c_str();
	snprintf(windowTitle, 64, "About %s", title);
}

AboutWindow::~AboutWindow()
{

}

void AboutWindow::VisibilityChanged(const bool _isVisible)
{
	BaseUI::VisibilityChanged(_isVisible);

	if (_isVisible)
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

		ImGui::Text("%s", params.description.c_str());
		ImGui::NewLine();
		ImGui::Text("By %s:", params.author.c_str());
		ImGui::SameLine();
		ImGui::TextLinkOpenURL(params.url.c_str());
		ImGui::NewLine();

		ImGui::SeparatorText("Libraries");

		ImGui::BulletText("ImGui %s:", ImGui::GetVersion());
		ImGui::SameLine();
		ImGui::TextLinkOpenURL("https://github.com/ocornut/imgui");

		ImGui::BulletText("%s", window->GetBackendInfo());
		ImGui::SameLine();
		ImGui::TextLinkOpenURL(window->GetBackendUrl());

		ImGui::BulletText("simpleini (Commit %s):", SIMPLEINI_COMMIT_HASH);
		ImGui::SameLine();
		ImGui::TextLinkOpenURL("https://github.com/brofield/simpleini");

		ImGui::EndPopup();
	}
}
