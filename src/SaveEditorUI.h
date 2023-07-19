#pragma once

#include "main.h"
#include "BaseUI.h"
#include "PopupDialog.h"
#include "AboutWindow.h"
#include "SaveData.h"
#include "imgui/imgui.h"

const char* const tabNames[]
{
	"Mario A",
	"Mario B",
	"Mario C",
	"Mario D"
};

class SaveEditorUI : public BaseUI
{
private:
	PopupDialog* popupDialog;
	AboutWindow* aboutWindow;

	const char* currentFileName;
	SaveData* saveData;

	bool showBackup;

public:
	SaveEditorUI();
	~SaveEditorUI();

protected:
	virtual void VisibilityChanged(const bool isVisible) override;
	virtual void DoRender() override;

private:
	inline bool CheckboxSaveFlags(const char* label, const uint8_t saveSlot, const uint8_t copyIndex, const uint32_t flag)
	{
		bool value = saveData->saveSlots[saveSlot][copyIndex].GetFlag(flag);

		if (ImGui::Checkbox(label, &value))
		{
			saveData->saveSlots[saveSlot][copyIndex].SetFlag(flag, value);
			saveData->saveSlots[saveSlot][copyIndex].CalculateChecksum();
		}

		return value;
	}

	inline bool CheckboxCourseData(const char* label, const uint8_t saveSlot, const uint8_t copyIndex, const uint8_t courseIndex, const uint8_t flag)
	{
		bool value = saveData->saveSlots[saveSlot][copyIndex].GetCourseDataFlag(courseIndex, flag);

		if (ImGui::Checkbox(label, &value))
		{
			saveData->saveSlots[saveSlot][copyIndex].SetCourseDataFlag(courseIndex, flag, value);
			saveData->saveSlots[saveSlot][copyIndex].CalculateChecksum();
		}

		return value;
	}

	inline void PrintChecksum(const uint16_t checksum)
	{
		ImGui::SetCursorPosX(WINDOW_WIDTH - ImGui::CalcTextSize("Checksum: 0xFFFF").x - 32);
		ImGui::Text("Checksum: 0x%x", checksum);
	}
};
