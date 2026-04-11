#include "PopupDialog.h"

#include <imgui/imgui.h>

PopupDialog::PopupDialog(Window* window, BaseUI* parentUi) : BaseUI(window, parentUi)
{
	type = MessageTypes::Normal;
	title.clear();
	message.clear();
}

PopupDialog::~PopupDialog()
{

}

void PopupDialog::SetMessage(const MessageTypes _type, const std::string _title, const std::string _message)
{
	type = _type;
	title = _title;
	message = _message;
}

void PopupDialog::VisibilityChanged(const bool _isVisible)
{
	BaseUI::VisibilityChanged(_isVisible);

	if (_isVisible)
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
