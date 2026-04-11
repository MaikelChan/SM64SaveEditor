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
	void VisibilityChanged(const bool _isVisible) override;
	void DoRender() override;
};
