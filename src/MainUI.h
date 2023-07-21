#pragma once

#include "BaseUI.h"
class SaveEditorUI;
class PopupDialog;
class AboutWindow;
class SaveData;
#include "imgui/imgui.h"
#include <imfilebrowser.h>

#define CONFIG_FILE_NAME "config.json"

class MainUI : public BaseUI
{
private:
	SaveEditorUI* saveEditor;
	PopupDialog* popupDialog;
	AboutWindow* aboutWindow;

	ImGui::FileBrowser fileDialog;

	std::string currentFilePath;
	std::string currentFileName;
	SaveData* saveData;

public:
	MainUI();
	~MainUI();

	inline SaveData* GetSaveData() const { return saveData; }

protected:
	virtual void VisibilityChanged(const bool isVisible) override;
	virtual void DoRender() override;

private:
	void ClearSaveData();

	void LoadConfig();
	void SaveConfig() const;
};
