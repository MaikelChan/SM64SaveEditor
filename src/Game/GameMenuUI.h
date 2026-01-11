#pragma once

#include <cstdint>

#include "../BaseUI.h"

class MainUI;
class SaveEditorUI;

class GameMenuUI : public BaseUI
{
private:
	const MainUI* mainUi;
	SaveEditorUI& saveEditorUi;

public:
	GameMenuUI(Window* window, const MainUI* parentUI, SaveEditorUI& saveEditorUI);
	~GameMenuUI();

protected:
	virtual void VisibilityChanged(const bool isVisible) override;
	virtual void DoRender() override;

private:
	void CompleteSlot(const uint8_t slotIndex) const;
	void DeleteSlot(const uint8_t slotIndex) const;
};