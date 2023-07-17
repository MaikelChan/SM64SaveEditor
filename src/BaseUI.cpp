#include "BaseUI.h"
#include <stdio.h>

BaseUI::BaseUI()
{
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
