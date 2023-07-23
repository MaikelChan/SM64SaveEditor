#pragma once

#include "BaseUI.h"
#include <imgui/imgui.h>

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

private:
	inline void AddUnderLine(ImColor color) const;
	inline void TextURL(const char* url) const;
};
