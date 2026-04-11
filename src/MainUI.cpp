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

	recentFiles.clear();
	currentSaveFile = nullptr;

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

void MainUI::VisibilityChanged(const bool _isVisible)
{
	BaseUI::VisibilityChanged(_isVisible);
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
				std::filesystem::path lastPath = DEFAULT_PATH;
				if (recentFiles.size() > 0) lastPath = recentFiles[0].parent_path();

				FileDialogParams* params = new FileDialogParams();
				params->ui = this;
				params->defaultLocation = lastPath;
				params->callback = OpenFileDialogCallback;
				window->ShowOpenFileDialog(params);
			}

			if (ImGui::BeginMenu("Open recent"))
			{
				for (uint8_t f = 0; f < recentFiles.size(); f++)
				{
					if (ImGui::MenuItem(recentFiles[f].u8string().c_str()))
					{
						if (!recentFiles[f].empty())
						{
							OpenFileCallback(recentFiles[f]);
						}
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Save", NULL, false, IsSaveFileLoaded()))
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

		if (IsSaveFileLoaded())
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

		if (IsSaveFileLoaded())
		{
			std::string fileText = std::string("Current file: ") + currentSaveFile->GetFileName();

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
	if (!IsSaveFileLoaded()) return;

	saveEditorUi.SetIsVisible(false);
	window->SetTaskbarProgress(0.0f);

	delete currentSaveFile;
	currentSaveFile = nullptr;
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

		popupDialogUi.SetMessage(MessageTypes::Error, "Error", "The selected file is not a valid save file.");
		popupDialogUi.SetIsVisible(true);

		return;
	}

	SaveFile* newSaveFile = new SaveFile();

	stream.seekg(0, std::ios_base::beg);
	std::string result = newSaveFile->Read(stream, size);
	stream.close();

	if (newSaveFile->GetFileType() == SaveFileTypes::NotValid)
	{
		delete newSaveFile;
		newSaveFile = nullptr;

		popupDialogUi.SetMessage(MessageTypes::Error, "Error", result);
		popupDialogUi.SetIsVisible(true);

		return;
	}

	newSaveFile->SetFilePath(filePath);

	currentSaveFile = newSaveFile;

	for (uint8_t f = 0; f < recentFiles.size(); f++)
	{
		if (recentFiles[f].compare(filePath) == 0)
		{
			recentFiles.erase(recentFiles.begin() + f);
		}
	}

	recentFiles.insert(recentFiles.begin(), filePath);
	if (recentFiles.size() > MAX_RECENT_FILES) recentFiles.resize(MAX_RECENT_FILES);

	saveEditorUi.SetIsVisible(true);

	if (!result.empty())
	{
		popupDialogUi.SetMessage(MessageTypes::Warning, "Warnings", result);
		popupDialogUi.SetIsVisible(true);
	}
}

void MainUI::SaveSaveData()
{
	if (!IsSaveFileLoaded()) return;

	std::ofstream stream = std::ofstream(currentSaveFile->GetFilePath(), std::ios::binary);

	if (!stream || !stream.is_open())
	{
		popupDialogUi.SetMessage(MessageTypes::Error, "Error", std::string("Can't save file \"") + currentSaveFile->GetFilePath().u8string() + "\".");
		popupDialogUi.SetIsVisible(true);

		return;
	}

	currentSaveFile->Write(stream);
	stream.close();
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

#if SUPPORT_TRANSPARENCY
	windowOpacity = (float)ini.GetDoubleValue(CONFIG_INI_SECTION, CONFIG_WINDOW_OPACITY, DEFAULT_OPACITY);
#endif

	recentFiles.clear();
	for (uint8_t f = 0; f < MAX_RECENT_FILES; f++)
	{
		char key[16];
		snprintf(key, 16, CONFIG_RECENT_FILE, f);

		std::filesystem::path filePath = std::filesystem::u8path(ini.GetValue(CONFIG_INI_SECTION, key, DEFAULT_PATH));
		if (filePath.empty()) continue;

		recentFiles.push_back(filePath);
	}
}

void MainUI::SaveConfig() const
{
	CSimpleIniA ini;
	ini.SetUnicode();

	SI_Error errorCode;

#if SUPPORT_TRANSPARENCY
	errorCode = ini.SetDoubleValue(CONFIG_INI_SECTION, CONFIG_WINDOW_OPACITY, windowOpacity);
#endif

	for (uint8_t f = 0; f < recentFiles.size(); f++)
	{
		char key[16];
		snprintf(key, 16, CONFIG_RECENT_FILE, f);
		errorCode = ini.SetValue(CONFIG_INI_SECTION, key, recentFiles[f].u8string().c_str());
	}

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

void MainUI::OpenFileDialogCallback(const FileDialogParams* fileDialogParams, const std::filesystem::path filePath, const char* error)
{
	MainUI* mainUi = (MainUI*)fileDialogParams->ui;
	delete fileDialogParams;

	if (error != nullptr)
	{
		char errorText[256];
		snprintf(errorText, 256, "Error in OpenFileDialog: %s", error);

		mainUi->popupDialogUi.SetMessage(MessageTypes::Error, "Error", errorText);
		mainUi->popupDialogUi.SetIsVisible(true);

		return;
	}
	else
	{
		if (filePath.empty())
		{
			return;
		}

		mainUi->OpenFileCallback(filePath);
	}
}
