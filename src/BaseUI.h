#pragma once

class BaseUI
{
protected:
	bool isVisible;

private:
	bool previousIsVisible;

public:
	BaseUI();
	~BaseUI();

	inline bool GetIsVisible() { return isVisible; }
	inline void SetIsVisible(const bool isVisible) { BaseUI::isVisible = isVisible; }
	inline void ToggleIsVisible() { isVisible = !isVisible; }

	void Render();

protected:
	virtual void VisibilityChanged(const bool isVisible);
	virtual void DoRender();
};
