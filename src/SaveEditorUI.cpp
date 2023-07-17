#include "SaveEditorUI.h"
#include "imgui/imgui.h"
#include "main.h"

SaveEditorUI::SaveEditorUI()
{
	aboutWindow = new AboutWindow();
}

SaveEditorUI::~SaveEditorUI()
{

	delete aboutWindow;
	aboutWindow = nullptr;
}

void SaveEditorUI::VisibilityChanged(const bool isVisible)
{
	BaseUI::VisibilityChanged(isVisible);
}

void SaveEditorUI::DoRender()
{
	BaseUI::DoRender();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load...", "CTRL+L")) {}
			if (ImGui::MenuItem("Save...", "CTRL+S")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Quit", "CTRL+Q"))
			{
				CloseMainWindow();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About...", NULL, aboutWindow->GetIsVisible()))
			{
				aboutWindow->ToggleIsVisible();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	// We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
	// Based on your use case you may want one or the other.
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	if (ImGui::Begin("Example: Fullscreen window", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground))
	{
		ImGui::Text("This is the main window.");
	}
	ImGui::End();

	aboutWindow->Render();
}