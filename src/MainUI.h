#pragma once

#include <filesystem>

#include "BaseUI.h"

#include "AboutWindow.h"
#include "PopupDialog.h"
#include "Game/GameMenuUI.h"
#include "Game/SaveData.h"
#include "Game/SaveEditorUI.h"

#define CONFIG_FILE_NAME "config.ini"
#define CONFIG_INI_SECTION "Config"
#define DEFAULT_PATH ""
#if SUPPORT_TRANSPARENCY
#define DEFAULT_OPACITY 0.9f
#endif

class MainUI : public BaseUI
{
private:
	SaveEditorUI saveEditorUi;
	GameMenuUI gameMenuUi;
	PopupDialog popupDialogUi;
	AboutWindow aboutWindowUi;

	std::filesystem::path lastPath;

	std::filesystem::path currentFile;
	std::string currentFileName;
	SaveData::Types currentFileType;
	SaveData* currentSaveData;

#if SUPPORT_TRANSPARENCY
	float windowOpacity;
#endif

public:
	MainUI(Window* window);
	~MainUI();

	inline bool IsSaveDataLoaded() const { return currentSaveData != nullptr; }
	inline SaveData* GetSaveData() const { return currentSaveData; }
#if SUPPORT_TRANSPARENCY
	inline float GetWindowOpacity() const { return windowOpacity; }
#endif

	void OpenFileCallback(std::filesystem::path filePath);

protected:
	virtual void VisibilityChanged(const bool isVisible) override;
	virtual void DoRender() override;

private:
	void ClearSaveData();
	void LoadSaveData(const std::filesystem::path filePath);
	void SaveSaveData();

	void LoadConfig();
	void SaveConfig() const;
};
