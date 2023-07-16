#include "AboutWindow.h"
#include "imgui/imgui.h"
#include "main.h"
#include "Config.h"

AboutWindow::AboutWindow()
{
	printf("Constructor: %s\n", typeid(*this).name());

	sprintf_s(windowTitle, 64, "About %s", WINDOW_TITLE);
	sprintf_s(description, 64, "%s - v%s", WINDOW_TITLE, PROJECT_VER);
}

AboutWindow::~AboutWindow()
{
	printf("Destructor: %s\n", typeid(*this).name());
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
		ImGui::Text(description);
		ImGui::Separator();
		ImGui::Text("By PacoChan.\n\n");
		ImGui::Text("This save editor is compatible with the PC port.");

		ImGui::EndPopup();
	}
}
