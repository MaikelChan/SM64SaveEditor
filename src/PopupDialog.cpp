#include "PopupDialog.h"
#include "imgui/imgui.h"

PopupDialog::PopupDialog(const BaseUI* parentUI) : BaseUI(parentUI)
{
	type = MessageTypes::Normal;
	title = "";
	message = "";
}

PopupDialog::~PopupDialog()
{

}

void PopupDialog::SetMessage(const MessageTypes type, std::string title, std::string message)
{
	PopupDialog::type = type;
	PopupDialog::title = title;
	PopupDialog::message = message;
}

void PopupDialog::VisibilityChanged(const bool isVisible)
{
	BaseUI::VisibilityChanged(isVisible);

	if (isVisible)
	{
		ImGui::OpenPopup(title.c_str());
	}
}

void PopupDialog::DoRender()
{
	BaseUI::DoRender();

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(700, 0));

	if (ImGui::BeginPopupModal(title.c_str(), &isVisible, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
	{
		switch (type)
		{
		case MessageTypes::Error:
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.4f, 1.0f));
			break;
		case MessageTypes::Warning:
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.7f, 0.4f, 1.0f));
			break;
		default:
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			break;
		}

		ImGui::TextWrapped("%s", message.c_str());
		ImGui::PopStyleColor();

		ImGui::Separator();

		if (ImGui::Button("Ok", ImVec2(80, 0)))
		{
			SetIsVisible(false);
		}

		ImGui::EndPopup();
	}
}
