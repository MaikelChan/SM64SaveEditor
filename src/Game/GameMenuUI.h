#pragma once

#include <cstdint>

#include "../BaseUI.h"

class MainUI;
class SaveEditorUI;

class GameMenuUI : public BaseUI
{
private:
	MainUI* mainUi;
	SaveEditorUI* saveEditorUi;

public:
	GameMenuUI(Window* window, BaseUI* parentUi, SaveEditorUI* saveEditorUi);
	~GameMenuUI();

protected:
	void VisibilityChanged(const bool _isVisible) override;
	void DoRender() override;

private:
	void CompleteSlot(const uint8_t slotIndex) const;
	void DeleteSlot(const uint8_t slotIndex) const;
};