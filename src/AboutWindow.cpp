#include "AboutWindow.h"
#include "main.h"
#include "Config.h"

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
		ImGui::Text("%s", description);
		ImGui::Separator();
		ImGui::Text("By PacoChan.");
		TextURL("https://pacochan.net/software/sm64-save-editor/");
		ImGui::Text("\nThis save editor is compatible with the Nintendo 64 version and the PC port.");

		ImGui::EndPopup();
	}
}

void AboutWindow::AddUnderLine(ImColor color) const
{
	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 max = ImGui::GetItemRectMax();
	min.y = max.y;
	ImGui::GetWindowDrawList()->AddLine(min, max, color, 1.0f);
}

void AboutWindow::TextURL(const char* url) const
{
	ImVec4 color = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
	color.x *= 1.5f;
	color.y *= 1.5f;
	color.z *= 1.5f;

	ImGui::PushStyleColor(ImGuiCol_Text, color);
	ImGui::Text(url);
	ImGui::PopStyleColor();

	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			std::string str;

#if defined(_WIN32) || defined(_WIN64)
			str = "explorer ";
#elif defined(__linux__)
			str = "xdg-open ";
#elif defined(__APPLE__)
			str = "open ";
#endif

			str.append(url);
			std::system(str.c_str());
		}

		color.x *= 1.5f;
		color.y *= 1.5f;
		color.z *= 1.5f;
		AddUnderLine(color);
	}
	else
	{
		AddUnderLine(color);
	}
}