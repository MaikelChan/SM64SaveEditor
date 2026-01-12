#pragma once

#include "BaseUI.h"

class AboutWindow : public BaseUI
{
private:
	char windowTitle[64];

public:
	AboutWindow(Window* window, BaseUI* parentUi);
	~AboutWindow();

protected:
	virtual void VisibilityChanged(const bool isVisible) override;
	virtual void DoRender() override;
};
