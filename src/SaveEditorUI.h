#pragma once

#include "BaseUI.h"
#include "AboutWindow.h"

class SaveEditorUI : public BaseUI
{
private:
	AboutWindow* aboutWindow;

public:
	SaveEditorUI();
	~SaveEditorUI();

protected:
	virtual void VisibilityChanged(const bool isVisible) override;
	virtual void DoRender() override;
};
