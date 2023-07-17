#pragma once

#include "BaseUI.h"
#include "AboutWindow.h"
#include "SaveData.h"

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
	AboutWindow* aboutWindow;

	const char* currentFileName;
	SaveData* saveData;

public:
	SaveEditorUI();
	~SaveEditorUI();

protected:
	virtual void VisibilityChanged(const bool isVisible) override;
	virtual void DoRender() override;
};
