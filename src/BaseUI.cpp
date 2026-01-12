#include "BaseUI.h"

BaseUI::BaseUI(Window* window, BaseUI* parentUi)
{
	this->window = window;
	this->parentUi = parentUi;

	isVisible = false;
	previousIsVisible = false;
}

BaseUI::~BaseUI()
{

}

void BaseUI::Render()
{
	if (previousIsVisible != isVisible)
	{
		previousIsVisible = isVisible;
		VisibilityChanged(isVisible);
	}

	if (!isVisible) return;

	DoRender();
}

void BaseUI::VisibilityChanged(const bool isVisible)
{

}

void BaseUI::DoRender()
{

}
