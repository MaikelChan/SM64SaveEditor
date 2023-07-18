#pragma once

#include "BaseUI.h"
#include <string>

enum class MessageTypes { Normal, Warning, Error };

class PopupDialog : public BaseUI
{
private:
	MessageTypes type;
	std::string title;
	std::string message;

public:
	PopupDialog();
	~PopupDialog();

	void SetMessage(const MessageTypes type, std::string title, std::string message);

protected:
	virtual void VisibilityChanged(const bool isVisible) override;
	virtual void DoRender() override;
};
