#include "BaseUI.h"
#include <typeinfo>
#include <stdio.h>

BaseUI::BaseUI()
{
	printf("Constructor: %s\n", typeid(*this).name());

	isVisible = false;
	previousIsVisible = false;
}

BaseUI::~BaseUI()
{
	printf("Destructor: %s\n", typeid(*this).name());
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
