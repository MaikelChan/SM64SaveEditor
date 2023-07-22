#include "main.h"
#include "MainUI.h"
#include "SaveEditorUI.h"
#include "PopupDialog.h"
#include "AboutWindow.h"
#include "Utils.h"
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
	currentFileType = SaveData::Types::NotValid;

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
				Save();
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
		Load();
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
	currentFileType = SaveData::Types::NotValid;
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

void MainUI::Load()
{
	ClearSaveData();

	try
	{
		saveData = SaveData::Load(fileDialog.GetSelected().string().c_str());
		currentFilePath = fileDialog.GetSelected().string();
		currentFileName = fileDialog.GetSelected().filename().string();
		currentFileType = saveData->GetType();

		EndianSwap();
		LoadingProcess();

		saveEditor->SetIsVisible(true);
	}
	catch (const std::runtime_error& error)
	{
		popupDialog->SetMessage(MessageTypes::Error, "Error", error.what());
		popupDialog->SetIsVisible(true);
	}
}

void MainUI::LoadingProcess()
{
	if (!saveData) return;

	std::string message;

	for (int s = 0; s < NUM_SAVE_SLOTS; s++)
	{
		if (saveData->saveSlots[s][0].IsValid()) continue;

		if (saveData->saveSlots[s][1].IsValid())
		{
			memcpy(&saveData->saveSlots[s][0], &saveData->saveSlots[s][1], SAVE_SLOT_SIZE);
			message += "Save slot " + std::to_string(s + 1) + " is corrupted, but valid data has been restored from the backup data.\n\n";
		}
		else
		{
			saveData->saveSlots[s][0].UpdateChecksum();
			message += "Save slot " + std::to_string(s + 1) + " is corrupted along with its backup. Data might be completely wrong.\n\n";
		}
	}

	if (!saveData->settings[0].IsValid())
	{
		if (saveData->settings[1].IsValid())
		{
			memcpy(&saveData->settings[0], &saveData->settings[1], SETTINGS_DATA_SIZE);
			message += "Settings data is corrupted, but valid data has been restored from the backup data.\n\n";
		}
		else
		{
			saveData->settings[0].UpdateChecksum();
			message += "Settings data is corrupted along with its backup. Data might be completely wrong.\n\n";
		}
	}

	if (!message.empty())
	{
		popupDialog->SetMessage(MessageTypes::Warning, "Load warnings", message);
		popupDialog->SetIsVisible(true);
	}
}

void MainUI::Save()
{
	if (!saveData) return;

	SavingProcess();
	EndianSwap();

	try
	{
		SaveData::Save(currentFilePath.c_str(), saveData);
	}
	catch (const std::runtime_error& error)
	{
		popupDialog->SetMessage(MessageTypes::Error, "Error", error.what());
		popupDialog->SetIsVisible(true);
	}

	EndianSwap();
}

void MainUI::SavingProcess()
{
	if (!saveData) return;

	for (int s = 0; s < NUM_SAVE_SLOTS; s++)
	{
		memcpy(&saveData->saveSlots[s][1], &saveData->saveSlots[s][0], SAVE_SLOT_SIZE);
	}

	memcpy(&saveData->settings[1], &saveData->settings[0], SETTINGS_DATA_SIZE);
}

void MainUI::EndianSwap()
{
	if (!saveData) return;
	if (currentFileType != SaveData::Types::Nintendo64) return;

	for (int s = 0; s < NUM_SAVE_SLOTS; s++)
	{
		for (int cp = 0; cp < NUM_COPIES; cp++)
		{
			saveData->saveSlots[s][cp].CapPos[0] = Swap16(saveData->saveSlots[s][cp].CapPos[0]);
			saveData->saveSlots[s][cp].CapPos[1] = Swap16(saveData->saveSlots[s][cp].CapPos[1]);
			saveData->saveSlots[s][cp].CapPos[2] = Swap16(saveData->saveSlots[s][cp].CapPos[2]);

			saveData->saveSlots[s][cp].Flags = Swap32(saveData->saveSlots[s][cp].Flags);

			saveData->saveSlots[s][cp].Magic = Swap16(saveData->saveSlots[s][cp].Magic);
			saveData->saveSlots[s][cp].Checksum = Swap16(saveData->saveSlots[s][cp].Checksum);
		}
	}

	for (int cp = 0; cp < NUM_COPIES; cp++)
	{
		for (int s = 0; s < NUM_SAVE_SLOTS; s++)
		{
			saveData->settings[cp].CoinScoreAges[s] = Swap32(saveData->settings[cp].CoinScoreAges[s]);
		}

		saveData->settings[cp].soundMode = Swap16(saveData->settings[cp].soundMode);
		saveData->settings[cp].language = Swap16(saveData->settings[cp].language);
		saveData->settings[cp].Magic = Swap16(saveData->settings[cp].Magic);
		saveData->settings[cp].Checksum = Swap16(saveData->settings[cp].Checksum);
	}
}
