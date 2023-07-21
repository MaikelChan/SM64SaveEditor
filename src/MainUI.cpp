#include "main.h"
#include "MainUI.h"
#include "SaveEditorUI.h"
#include "PopupDialog.h"
#include "AboutWindow.h"
#include "SaveData.h"
#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>
using json = nlohmann::json;

MainUI::MainUI() : BaseUI(nullptr)
{
	saveEditor = new SaveEditorUI(this);
	aboutWindow = new AboutWindow(this);
	popupDialog = new PopupDialog(this);

	saveData = nullptr;
	currentFilePath.clear();
	currentFileName.clear();

	fileDialog.SetTitle("Open a Super Mario 64 save file");
	fileDialog.SetTypeFilters({ ".bin", ".eep", ".*" });

	LoadConfig();
}

MainUI::~MainUI()
{
	ClearSaveData();

	delete saveEditor;
	saveEditor = nullptr;

	delete popupDialog;
	popupDialog = nullptr;

	delete aboutWindow;
	aboutWindow = nullptr;
}

void MainUI::VisibilityChanged(const bool isVisible)
{
	BaseUI::VisibilityChanged(isVisible);
}

void MainUI::DoRender()
{
	BaseUI::DoRender();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open..."))
			{
				fileDialog.Open();
			}

			if (ImGui::MenuItem("Save"))
			{
				if (saveData)
				{
					try
					{
						SaveData::Save(currentFilePath.c_str(), saveData);
					}
					catch (const std::runtime_error& error)
					{
						popupDialog->SetMessage(MessageTypes::Error, "Error", error.what());
						popupDialog->SetIsVisible(true);
					}
				}
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Quit"))
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

		if (saveData)
		{
			std::string fileText = std::string("Current file: ") + currentFileName;

			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize(fileText.c_str()).x - 32);
			ImGui::Text("%s", fileText.c_str());
		}

		ImGui::EndMainMenuBar();
	}

	//ImGui::ShowDemoWindow();

	saveEditor->Render();
	popupDialog->Render();
	aboutWindow->Render();

	fileDialog.Display();

	if (fileDialog.HasSelected())
	{
		ClearSaveData();

		try
		{
			saveData = SaveData::Load(fileDialog.GetSelected().string().c_str());
			currentFilePath = fileDialog.GetSelected().string();
			currentFileName = fileDialog.GetSelected().filename().string();

			saveEditor->SetIsVisible(true);
		}
		catch (const std::runtime_error& error)
		{
			popupDialog->SetMessage(MessageTypes::Error, "Error", error.what());
			popupDialog->SetIsVisible(true);
		}

		SaveConfig();

		fileDialog.ClearSelected();
	}
}

void MainUI::ClearSaveData()
{
	if (!saveData) return;

	delete saveData;
	saveData = nullptr;

	currentFilePath.clear();
	currentFileName.clear();
}

void MainUI::LoadConfig()
{
	try
	{
		std::ifstream stream(CONFIG_FILE_NAME);

		if (!stream || !stream.is_open())
		{
			return;
		}

		json config;
		stream >> config;
		stream.close();

		std::string path = config["lastPath"].template get<std::string>();
		fileDialog.SetPwd(std::filesystem::u8path(path));
	}
	catch (const json::parse_error& error)
	{
		printf("Json Error: %s\n", error.what());
	}
}

void MainUI::SaveConfig() const
{
	try
	{
		json config;
		config["lastPath"] = fileDialog.GetPwd().u8string();

		// The setw manipulator was overloaded to set the indentation for pretty printing.

		std::ofstream stream(CONFIG_FILE_NAME);
		stream << std::setw(4) << config << std::endl;
		stream.close();
	}
	catch (const json::type_error& error)
	{
		printf("Json Error: %s\n", error.what());
	}
}
