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
#if SUPPORT_TRANSPARENCY
#define DEFAULT_OPACITY 0.9f
#endif

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

#if SUPPORT_TRANSPARENCY
	float windowOpacity;
#endif

public:
	MainUI();
	~MainUI();

	inline bool IsSaveDataLoaded() const { return saveData != nullptr; }
	inline SaveData* GetSaveData() const { return saveData; }
#if SUPPORT_TRANSPARENCY
	inline float GetWindowOpacity() const { return windowOpacity; }
#endif

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
