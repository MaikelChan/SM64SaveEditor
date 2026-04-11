#pragma once

#include "Window.h"

struct SDL_Window;
struct SDL_GPUDevice;
struct SDL_DialogFileFilter;

class BaseUI;

class WindowSDL : public Window
{
private:
	SDL_Window* window = nullptr;
	SDL_GPUDevice* gpuDevice = nullptr;
	bool windowClaimed = false;

	char backendInfo[64];

	float currentTaskbarProgress = -1.0f;

public:
	WindowSDL(const WindowParams& params);
	~WindowSDL();

	const char* GetBackendInfo() const override { return backendInfo; }
	const char* GetBackendUrl() const override { return "https://www.libsdl.org"; }

	void Run(BaseUI& ui) override;
	void Terminate();

	void ShowOpenFileDialog(const FileDialogParams* fileDialogParams) const override;
	void SetTaskbarProgress(const float value) override;

private:
	static void OpenFileDialogCallback(void* userdata, const char* const* filelist, int filter);
};
