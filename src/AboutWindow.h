#pragma once

#include "BaseUI.h"

class AboutWindow : public BaseUI
{
private:
	char windowTitle[64];
	char description[64];

public:
	AboutWindow(const BaseUI* parentUI);
	~AboutWindow();

protected:
	virtual void VisibilityChanged(const bool isVisible) override;
	virtual void DoRender() override;
};
