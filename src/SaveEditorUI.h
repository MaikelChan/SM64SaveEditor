#pragma once

#include <cstdint>
#include "BaseUI.h"
class MainUI;

#define CONFIG_FILE_NAME "config.json"

const char* const tabNames[]
{
	"Mario A",
	"Mario B",
	"Mario C",
	"Mario D"
};

class SaveEditorUI : public BaseUI
{
public:
	bool showBackup;

private:
	const MainUI* mainUI;

public:
	SaveEditorUI(const MainUI* mainUI);
	~SaveEditorUI();

protected:
	virtual void VisibilityChanged(const bool isVisible) override;
	virtual void DoRender() override;

private:
	bool CheckboxSaveFlags(const char* label, const uint8_t saveSlot, const uint8_t copyIndex, const uint32_t flag) const;
	bool CheckboxCourseData(const char* label, const uint8_t saveSlot, const uint8_t copyIndex, const uint8_t courseIndex, const uint8_t flag) const;
	void PrintChecksum(const uint16_t checksum) const;
};
