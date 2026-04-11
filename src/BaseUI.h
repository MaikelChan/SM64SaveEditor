#pragma once

class Window;

class BaseUI
{
protected:
	Window* window;
	BaseUI* parentUi;
	bool isVisible;

private:
	bool previousIsVisible;

public:
	BaseUI(Window* window, BaseUI* parentUi);
	virtual ~BaseUI();

	inline bool GetIsVisible() const { return isVisible; }
	inline void SetIsVisible(const bool _isVisible) { isVisible = _isVisible; }
	inline void ToggleIsVisible() { isVisible = !isVisible; }

	void Render();

protected:
	virtual void VisibilityChanged(const bool _isVisible) = 0;
	virtual void DoRender() = 0;
};
