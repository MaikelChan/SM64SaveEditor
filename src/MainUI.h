#pragma once

#include <filesystem>
#include <vector>

#include "BaseUI.h"

#include "AboutWindow.h"
#include "PopupDialog.h"
#include "Game/GameMenuUI.h"
#include "Game/SaveEditorUI.h"

class SaveFile;
struct FileDialogParams;

constexpr uint8_t MAX_RECENT_FILES = 5;

constexpr const char* CONFIG_FILE_NAME = "config.ini";
constexpr const char* CONFIG_INI_SECTION = "Config";
constexpr const char* CONFIG_RECENT_FILE = "recentFile_%u";

constexpr const char* DEFAULT_PATH = "";

#if SUPPORT_TRANSPARENCY
constexpr const char* CONFIG_WINDOW_OPACITY = "windowOpacity";
constexpr float DEFAULT_OPACITY = 0.9f;
#endif

class MainUI : public BaseUI
{
private:
	SaveEditorUI saveEditorUi;
	GameMenuUI gameMenuUi;
	PopupDialog popupDialogUi;
	AboutWindow aboutWindowUi;

	std::vector<std::filesystem::path> recentFiles;

	SaveFile* currentSaveFile;

#if SUPPORT_TRANSPARENCY
	float windowOpacity;
#endif

public:
	MainUI(Window* window);
	~MainUI();

	inline bool IsSaveFileLoaded() const { return currentSaveFile != nullptr; }
	inline SaveFile* GetSaveFile() const { return currentSaveFile; }

#if SUPPORT_TRANSPARENCY
	inline float GetWindowOpacity() const { return windowOpacity; }
#endif

	void OpenFileCallback(std::filesystem::path filePath);

protected:
	void VisibilityChanged(const bool _isVisible) override;
	void DoRender() override;

private:
	void ClearSaveData();
	void LoadSaveData(const std::filesystem::path filePath);
	void SaveSaveData();

	void LoadConfig();
	void SaveConfig() const;

	static void OpenFileDialogCallback(const FileDialogParams* fileDialogParams, const std::filesystem::path filePath, const char* error);
};
