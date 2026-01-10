#pragma once

class Window;

class BaseUI
{
protected:
	const Window* window;
	const BaseUI* parentUI;
	bool isVisible;

private:
	bool previousIsVisible;

public:
	BaseUI(const Window* window, const BaseUI* parentUI);
	virtual ~BaseUI();

	inline bool GetIsVisible() const { return isVisible; }
	inline void SetIsVisible(const bool isVisible) { BaseUI::isVisible = isVisible; }
	inline void ToggleIsVisible() { isVisible = !isVisible; }

	void Render();

protected:
	virtual void VisibilityChanged(const bool isVisible);
	virtual void DoRender();
};
