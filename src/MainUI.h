#pragma once

#include "BaseUI.h"
class SaveEditorUI;
class PopupDialog;
class AboutWindow;
#include "SaveData.h"
#include <filesystem>
#include <imgui/imgui.h>

#define CONFIG_FILE_NAME "config.ini"
#define CONFIG_INI_SECTION "Config"
#define DEFAULT_PATH ""
#define DEFAULT_OPACITY 0.9f

class MainUI : public BaseUI
{
private:
	SaveEditorUI* saveEditor;
	PopupDialog* popupDialog;
	AboutWindow* aboutWindow;

	std::filesystem::path currentFilePath;
	std::string currentFileName;
	SaveData::Types currentFileType;
	SaveData* saveData;

	float windowOpacity;

public:
	MainUI();
	~MainUI();

	inline bool IsSaveDataLoaded() const { return saveData != nullptr; }
	inline SaveData* GetSaveData() const { return saveData; }
	inline float GetWindowOpacity() const { return windowOpacity; }

	void OpenFileCallback(std::filesystem::path filePath);

protected:
	virtual void VisibilityChanged(const bool isVisible) override;
	virtual void DoRender() override;

private:
	void ClearSaveData();

	void LoadConfig();
	void SaveConfig() const;

	void Load(std::filesystem::path filePath);
	void LoadingProcess() const;
	void Save() const;
	void SavingProcess() const;
	void EndianSwap() const;

	void CompleteSlot(const uint8_t slotIndex) const;
	void DeleteSlot(const uint8_t slotIndex) const;
};
