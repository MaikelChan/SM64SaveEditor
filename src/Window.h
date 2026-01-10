#pragma once

#include <cstdint>
#include <functional>
#include <string>

struct SDL_Window;
struct SDL_GPUDevice;

struct ImVec4;
struct ImFontAtlas;

class BaseUI;

struct WindowParams
{
public:
	std::string title;
	int32_t initialWidth;
	int32_t initialHeight;

	std::function<void(ImVec4* colors)> configureStyleCallback;
	std::function<void(ImFontAtlas* fontAtlas)> configureFontsCallback;
};

class Window
{
private:
	SDL_Window* window = nullptr;
	SDL_GPUDevice* gpuDevice = nullptr;
	bool windowClaimed = false;

	std::string driverName = {};
	bool isRunning = false;

public:
	Window(WindowParams* params);
	~Window();

	std::string GetDriverName() const { return driverName; }

	void Run(BaseUI* ui);
	void Terminate();
};