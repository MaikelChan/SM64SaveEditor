#include "MainUI.h"

#include <fstream>

#include <SimpleIni.h>
#include <imgui/imgui.h>

#include "Window.h"

MainUI::MainUI(Window* window) : BaseUI(window, nullptr),
saveEditorUi(window, this),
gameMenuUi(window, this, &saveEditorUi),
popupDialogUi(window, this),
aboutWindowUi(window, this)
{
	gameMenuUi.SetIsVisible(true);

	lastPath.clear();

	currentFile.clear();
	currentFileName.clear();
	currentFileType = SaveData::Types::NotValid;
	currentSaveData = nullptr;

#if SUPPORT_TRANSPARENCY
	windowOpacity = DEFAULT_OPACITY;
#endif

	LoadConfig();
}

MainUI::~MainUI()
{
	SaveConfig();
	ClearSaveData();
}

void MainUI::OpenFileCallback(std::filesystem::path filePath)
{
	MainUI::LoadSaveData(filePath);
	MainUI::SaveConfig();
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
				auto callback = [](void* userdata, const char* const* filelist, int filter) -> void
					{
						if (filelist == nullptr)
						{
							printf("Error in OpenFileDialog: %s", SDL_GetError());
							return;
						}
						else if (*filelist == nullptr)
						{
							// The user did not select any file.
							return;
						}

						MainUI* mainUI = (MainUI*)userdata;
						std::filesystem::path filePath = std::filesystem::u8path(filelist[0]);
						mainUI->OpenFileCallback(filePath);
					};

				window->ShowOpenFileDialog(lastPath, (void*)this, callback);
			}

			if (ImGui::MenuItem("Save", NULL, false, IsSaveDataLoaded()))
			{
				SaveSaveData();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Quit"))
			{
				window->Terminate();
			}

			ImGui::EndMenu();
		}

		if (IsSaveDataLoaded())
		{
			gameMenuUi.Render();
		}

#if SUPPORT_TRANSPARENCY
		if (ImGui::BeginMenu("Settings"))
		{
			ImGui::SliderFloat("Window Opacity", &windowOpacity, 0.0f, 1.0f);

			ImGui::EndMenu();
		}
#endif

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About...", NULL, aboutWindowUi.GetIsVisible()))
			{
				aboutWindowUi.ToggleIsVisible();
			}

			ImGui::EndMenu();
		}

		if (IsSaveDataLoaded())
		{
			std::string fileText = std::string("Current file: ") + currentFileName;

			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize(fileText.c_str()).x - 32);
			ImGui::Text("%s", fileText.c_str());
		}

		ImGui::EndMainMenuBar();
	}

	//ImGui::ShowDemoWindow();

	saveEditorUi.Render();
	popupDialogUi.Render();
	aboutWindowUi.Render();
}

void MainUI::ClearSaveData()
{
	if (!IsSaveDataLoaded()) return;

	delete currentSaveData;
	currentSaveData = nullptr;

	lastPath.clear();
	currentFile.clear();
	currentFileName.clear();
	currentFileType = SaveData::Types::NotValid;
}

void MainUI::LoadSaveData(const std::filesystem::path filePath)
{
	ClearSaveData();

	std::ifstream stream = std::ifstream(filePath, std::ios::binary);

	if (!stream || !stream.is_open())
	{
		popupDialogUi.SetMessage(MessageTypes::Error, "Error", "There was an error trying to open the file.");
		popupDialogUi.SetIsVisible(true);

		return;
	}

	stream.seekg(0, std::ios_base::end);
	size_t size = stream.tellg();

	if (size < SAVE_DATA_SIZE)
	{
		stream.close();

		popupDialogUi.SetMessage(MessageTypes::Error, "Error", "The selected file is not a valid Super Mario 64 save file.");
		popupDialogUi.SetIsVisible(true);

		return;
	}

	SaveData* newSaveData = new SaveData();

	stream.seekg(0, std::ios_base::beg);
	stream.read((char*)newSaveData, SAVE_DATA_SIZE);
	stream.close();

	SaveData::InitializationResult result = newSaveData->CheckAndInitialize();
	if (result.type == SaveData::Types::NotValid)
	{
		delete newSaveData;
		newSaveData = nullptr;

		popupDialogUi.SetMessage(MessageTypes::Error, "Error", result.message);
		popupDialogUi.SetIsVisible(true);

		return;
	}

	currentFileType = result.type;
	currentSaveData = newSaveData;

	lastPath = filePath.parent_path();
	currentFile = filePath;
	currentFileName = filePath.filename().u8string();

	saveEditorUi.showBackup = false;
	saveEditorUi.SetIsVisible(true);

	if (!result.message.empty())
	{
		popupDialogUi.SetMessage(MessageTypes::Warning, "Warnings", result.message);
		popupDialogUi.SetIsVisible(true);
	}
}

void MainUI::SaveSaveData()
{
	if (!IsSaveDataLoaded()) return;

	std::ofstream stream = std::ofstream(currentFile, std::ios::binary);

	if (!stream || !stream.is_open())
	{
		popupDialogUi.SetMessage(MessageTypes::Error, "Error", std::string("Can't save file \"") + currentFile.u8string() + "\".");
		popupDialogUi.SetIsVisible(true);

		return;
	}

	currentSaveData->BeginSaving(currentFileType);

	stream.write((char*)currentSaveData, SAVE_DATA_SIZE);
	stream.close();

	currentSaveData->FinishSaving(currentFileType);
}

void MainUI::LoadConfig()
{
	CSimpleIniA ini;
	ini.SetUnicode();

	SI_Error errorCode = ini.LoadFile(CONFIG_FILE_NAME);
	if (errorCode < 0)
	{
		if (errorCode == SI_FILE) printf("The config file \"%s\" is missing or corrupt. Creating a new one.\n", CONFIG_FILE_NAME);
		else printf("Error loading config file \"%s\". Code: %i.\n", CONFIG_FILE_NAME, errorCode);
		return;
	};

	lastPath = std::filesystem::u8path(ini.GetValue(CONFIG_INI_SECTION, "lastPath", DEFAULT_PATH));
#if SUPPORT_TRANSPARENCY
	windowOpacity = (float)ini.GetDoubleValue(CONFIG_INI_SECTION, "windowOpacity", DEFAULT_OPACITY);
#endif
}

void MainUI::SaveConfig() const
{
	CSimpleIniA ini;
	ini.SetUnicode();

	SI_Error errorCode;

	errorCode = ini.SetValue(CONFIG_INI_SECTION, "lastPath", lastPath.u8string().c_str());
#if SUPPORT_TRANSPARENCY
	errorCode = ini.SetDoubleValue(CONFIG_INI_SECTION, "windowOpacity", windowOpacity);
#endif

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
