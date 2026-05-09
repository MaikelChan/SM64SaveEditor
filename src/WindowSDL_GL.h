#pragma once

#if ENABLE_OPENGL

#include "Window.h"

#include <SDL3/SDL.h>

struct SDL_Window;
struct SDL_DialogFileFilter;

class BaseUI;

class WindowSDL_GL : public Window
{
private:
	SDL_Window* window = nullptr;
	SDL_GLContext glContext = nullptr;

	char backendInfo[64];

	float currentTaskbarProgress = -1.0f;

public:
	WindowSDL_GL(const WindowParams& params);
	~WindowSDL_GL();

	const char* GetBackendInfo() const override { return backendInfo; }
	const char* GetBackendUrl() const override { return "https://www.libsdl.org"; }

	void Run(BaseUI& ui) override;
	void Terminate();

	void ShowOpenFileDialog(const FileDialogParams* fileDialogParams) const override;
	void SetTaskbarProgress(const float value) override;

private:
	static void OpenFileDialogCallback(void* userdata, const char* const* filelist, int filter);
};

#endif