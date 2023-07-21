#pragma once

#include "main.h"
#include "BaseUI.h"
#include "PopupDialog.h"
#include "AboutWindow.h"
#include "SaveData.h"
#include "imgui/imgui.h"
#include <imfilebrowser.h>

#define CONFIG_FILE_NAME "config.json"

const char* const tabNames[]
{
	"Mario A",
	"Mario B",
	"Mario C",
	"Mario D"
};

enum class FileDialogTypes { None, Open, Save };

class SaveEditorUI : public BaseUI
{
private:
	PopupDialog* popupDialog;
	AboutWindow* aboutWindow;

	ImGui::FileBrowser fileDialog;

	std::string currentFileName;
	SaveData* saveData;

	bool showBackup;

public:
	SaveEditorUI();
	~SaveEditorUI();

protected:
	virtual void VisibilityChanged(const bool isVisible) override;
	virtual void DoRender() override;

private:
	bool CheckboxSaveFlags(const char* label, const uint8_t saveSlot, const uint8_t copyIndex, const uint32_t flag);
	bool CheckboxCourseData(const char* label, const uint8_t saveSlot, const uint8_t copyIndex, const uint8_t courseIndex, const uint8_t flag);
	void PrintChecksum(const uint16_t checksum);

	void ClearSaveData();

	void LoadConfig();
	void SaveConfig();
};
