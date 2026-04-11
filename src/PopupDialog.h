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
	PopupDialog(Window* window, BaseUI* parentUi);
	~PopupDialog();

	void SetMessage(const MessageTypes _type, const std::string _title, const std::string _message);

protected:
	void VisibilityChanged(const bool _isVisible) override;
	void DoRender() override;
};
