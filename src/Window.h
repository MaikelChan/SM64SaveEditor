#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>

#include <SDL3/SDL.h>

struct SDL_Window;
struct SDL_GPUDevice;
struct SDL_DialogFileFilter;

struct ImVec4;
struct ImFontAtlas;

class BaseUI;

struct WindowParams
{
	std::string title;
	std::string description;
	std::string author;
	std::string url;

	int32_t initialWidth;
	int32_t initialHeight;
	std::string openDialogTitle;
	int32_t openDialogFiltersCount;
	const SDL_DialogFileFilter* openDialogFilters;

	std::function<void(ImVec4* colors)> configureStyleCallback;
	std::function<void(ImFontAtlas* fontAtlas)> configureFontsCallback;
};

class Window
{
private:
	const WindowParams& params;

	SDL_Window* window = nullptr;
	SDL_GPUDevice* gpuDevice = nullptr;
	bool windowClaimed = false;

	std::string driverName;
	bool isRunning = false;

public:
	Window(const WindowParams& params);
	~Window();

	const WindowParams& GetParams() const { return params; }
	const std::string& GetDriverName() const { return driverName; }

	void Run(BaseUI& ui);
	void Terminate();

	void ShowOpenFileDialog(std::filesystem::path defaultLocation, void* userData, SDL_DialogFileCallback callback) const;
};