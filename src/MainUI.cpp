#include "main.h"
#include "MainUI.h"
#include "SaveEditorUI.h"
#include "PopupDialog.h"
#include "AboutWindow.h"
#include "Utils.h"
#include <fstream>
#include <SimpleIni.h>

MainUI::MainUI() : BaseUI(nullptr)
{
	saveEditor = new SaveEditorUI(this);
	aboutWindow = new AboutWindow(this);
	popupDialog = new PopupDialog(this);

	currentFilePath.clear();
	currentFileName.clear();
	currentFileType = SaveData::Types::NotValid;
	saveData = nullptr;

	windowOpacity = 0.9f;

	fileDialog.SetTitle("Open a Super Mario 64 save file");
	fileDialog.SetTypeFilters({ ".eep", ".bin", ".*" });

	LoadConfig();
}

MainUI::~MainUI()
{
	ClearSaveData();
	SaveConfig();

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

		if (saveData && ImGui::BeginMenu("Tools"))
		{
			for (uint8_t s = 0; s < NUM_SAVE_SLOTS; s++)
			{
				if (ImGui::BeginMenu(tabNames[s]))
				{
					if (ImGui::MenuItem("100% Complete"))
					{
						CompleteSlot(s);
					}

					if (ImGui::MenuItem("Delete"))
					{
						DeleteSlot(s);
					}

					ImGui::EndMenu();
				}
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Show backup data", NULL, saveEditor->showBackup))
			{
				saveEditor->showBackup = !saveEditor->showBackup;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Settings"))
		{
			ImGui::SliderFloat("Window Opacity", &windowOpacity, 0.0f, 1.0f);

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
	CSimpleIniA ini;
	ini.SetUnicode();

	SI_Error errorCode = ini.LoadFile(CONFIG_FILE_NAME);
	if (errorCode < 0)
	{
		printf("Error converting INI config data to string format. Error code: %i.\n", errorCode);
		return;
	};

	fileDialog.SetDirectory(std::filesystem::u8path(ini.GetValue(CONFIG_INI_SECTION, "lastPath", fileDialog.GetDirectory().u8string().c_str())));
	windowOpacity = (float)ini.GetDoubleValue(CONFIG_INI_SECTION, "windowOpacity", windowOpacity);
}

void MainUI::SaveConfig() const
{
	CSimpleIniA ini;
	ini.SetUnicode();

	SI_Error errorCode;

	errorCode = ini.SetValue(CONFIG_INI_SECTION, "lastPath", fileDialog.GetDirectory().u8string().c_str());
	errorCode = ini.SetDoubleValue(CONFIG_INI_SECTION, "windowOpacity", windowOpacity);

	std::string data;
	errorCode = ini.Save(data);

	if (errorCode < 0)
	{
		printf("Error getting INI data in string format. Error code: %i.\n", errorCode);
		return;
	};

	errorCode = ini.SaveFile(CONFIG_FILE_NAME);
	if (errorCode < 0)
	{
		printf("Error saving INI file to %s. Error code: %i.\n", CONFIG_FILE_NAME, errorCode);
	};
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

		saveEditor->showBackup = false;
		saveEditor->SetIsVisible(true);
	}
	catch (const std::runtime_error& error)
	{
		popupDialog->SetMessage(MessageTypes::Error, "Error", error.what());
		popupDialog->SetIsVisible(true);
	}
}

void MainUI::LoadingProcess() const
{
	if (!saveData) return;

	std::string message;

	for (int s = 0; s < NUM_SAVE_SLOTS; s++)
	{
		if (saveData->saveSlots[s][0].IsValid()) continue;

		if (saveData->saveSlots[s][1].IsValid())
		{
			memcpy(&saveData->saveSlots[s][0], &saveData->saveSlots[s][1], SAVE_SLOT_SIZE);
			message += std::string("Save slot \"") + tabNames[s] + "\" is corrupted, but valid data has been restored from the backup data.\n\n";
		}
		else
		{
			saveData->saveSlots[s][0].UpdateChecksum();
			message += std::string("Save slot \"") + tabNames[s] + "\" is corrupted along with its backup. Data might be completely wrong.\n\n";
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

void MainUI::Save() const
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

void MainUI::SavingProcess() const
{
	if (!saveData) return;

	for (int s = 0; s < NUM_SAVE_SLOTS; s++)
	{
		saveData->saveSlots[s][0].Magic = SAVE_SLOT_MAGIC_LE;
		memcpy(&saveData->saveSlots[s][1], &saveData->saveSlots[s][0], SAVE_SLOT_SIZE);
	}

	saveData->settings[0].Magic = SETTINGS_DATA_MAGIC_LE;
	memcpy(&saveData->settings[1], &saveData->settings[0], SETTINGS_DATA_SIZE);
}

void MainUI::EndianSwap() const
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

void MainUI::CompleteSlot(const uint8_t slotIndex) const
{
	if (!saveData) return;

	memset(&saveData->saveSlots[slotIndex][0], 0, SAVE_SLOT_SIZE);

	saveData->saveSlots[slotIndex][0].Flags = 0x1f10ffcf;

	for (int c = 0; c < COURSE_COUNT; c++)
	{
		for (int st = 0; st < courseStarCount[c]; st++)
		{
			saveData->saveSlots[slotIndex][0].CourseData[c] |= (1 << st);
		}

		if (courseHasCannon[c])
		{
			saveData->saveSlots[slotIndex][0].CourseData[c + 1] |= SAVE_COURSE_FLAG_STAR_CANNON_OPEN;
		}
	}

	for (int c = 0; c < COURSE_STAGES_COUNT; c++)
	{
		saveData->saveSlots[slotIndex][0].CourseCoinScores[c] = 100;
	}

	saveData->saveSlots[slotIndex][0].Magic = SAVE_SLOT_MAGIC_LE;
	saveData->saveSlots[slotIndex][0].UpdateChecksum();
}

void MainUI::DeleteSlot(const uint8_t slotIndex) const
{
	if (!saveData) return;

	memset(&saveData->saveSlots[slotIndex][0], 0, SAVE_SLOT_SIZE);
	saveData->saveSlots[slotIndex][0].Magic = SAVE_SLOT_MAGIC_LE;
	saveData->saveSlots[slotIndex][0].UpdateChecksum();
}
