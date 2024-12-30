#include "AboutWindow.h"
#include "main.h"
#include "Config.h"

#include <imgui/imgui.h>
#include <SimpleIni.h>
#include <GLFW/glfw3.h>

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

		//ImGui::NewLine();
		ImGui::SeparatorText("Libraries");

		ImGui::BulletText("ImGui %s:", ImGui::GetVersion());
		ImGui::SameLine();
		ImGui::TextLinkOpenURL("https://github.com/ocornut/imgui");

		ImGui::BulletText("GLFW %i.%i.%i:", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
		ImGui::SameLine();
		ImGui::TextLinkOpenURL("https://www.glfw.org");

		ImGui::BulletText("simpleini (Commit bfcc0d2):");
		ImGui::SameLine();
		ImGui::TextLinkOpenURL("https://github.com/brofield/simpleini");

		ImGui::BulletText("imgui-filebrowser (Commit db5048c):");
		ImGui::SameLine();
		ImGui::TextLinkOpenURL("https://github.com/AirGuanZ/imgui-filebrowser");

		ImGui::EndPopup();
	}
}