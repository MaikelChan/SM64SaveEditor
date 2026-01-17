#include "MainUI.h"

#include <fstream>

#include <SimpleIni.h>
#include <SDL3/SDL.h>
#include <imgui/imgui.h>

#include "Window.h"
#include "Utils.h"

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
	saveData = nullptr;

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
	MainUI::Load(filePath);
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
				Save();
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

	delete saveData;
	saveData = nullptr;

	lastPath.clear();
	currentFile.clear();
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

void MainUI::Load(const std::filesystem::path filePath)
{
	ClearSaveData();

	try
	{
		saveData = new SaveData();
		currentFileType = SaveData::Load(filePath, saveData);

		lastPath = filePath.parent_path();
		currentFile = filePath;
		currentFileName = filePath.filename().u8string();

		saveEditorUi.showBackup = false;
		saveEditorUi.SetIsVisible(true);
	}
	catch (const std::runtime_error& error)
	{
		delete saveData;
		saveData = nullptr;

		popupDialogUi.SetMessage(MessageTypes::Error, "Error", error.what());
		popupDialogUi.SetIsVisible(true);
	}
}

void MainUI::Save()
{
	if (!IsSaveDataLoaded()) return;

	try
	{
		SaveData::Save(currentFile, saveData, currentFileType);
	}
	catch (const std::runtime_error& error)
	{
		popupDialogUi.SetMessage(MessageTypes::Error, "Error", error.what());
		popupDialogUi.SetIsVisible(true);
	}
}
