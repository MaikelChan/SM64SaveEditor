#pragma once

class BaseUI
{
protected:
	const BaseUI* parentUI;
	bool isVisible;

private:
	bool previousIsVisible;

public:
	BaseUI(const BaseUI* parentUI);
	~BaseUI();

	inline bool GetIsVisible() const { return isVisible; }
	inline void SetIsVisible(const bool isVisible) { BaseUI::isVisible = isVisible; }
	inline void ToggleIsVisible() { isVisible = !isVisible; }

	void Render();

protected:
	virtual void VisibilityChanged(const bool isVisible);
	virtual void DoRender();
};
