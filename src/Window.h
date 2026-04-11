#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

#include <imgui/imgui.h>

struct ImFontAtlas;

class BaseUI;

struct FileDialogParams;
typedef void (*FileDialogCallback)(const FileDialogParams* fileDialogParams, const std::filesystem::path filePath, const char* error);

struct FileDialogFilter
{
	const char* name;
	const char* pattern;
};

struct FileDialogParams
{
	BaseUI* ui;
	std::filesystem::path defaultLocation;
	FileDialogCallback callback;
};

typedef void (*ConfigureStyleCallback)(ImVec4* colors);
typedef void (*ConfigureFontsCallback)(ImFontAtlas* fontAtlas);

struct WindowParams
{
	std::string title;
	std::string description;
	std::string author;
	std::string url;

	int32_t initialWidth;
	int32_t initialHeight;
	ImVec4 backgroundColor;
	std::string openDialogTitle;
	int32_t openDialogFiltersCount;
	const FileDialogFilter* openDialogFilters;

	ConfigureStyleCallback configureStyleCallback;
	ConfigureFontsCallback configureFontsCallback;
};

class Window
{
protected:
	const WindowParams& params;
	bool isRunning = false;

	float currentWindowScale = -1.0f;

public:
	Window(const WindowParams& params);
	virtual ~Window();

	const WindowParams& GetParams() const { return params; }
	inline float GetWindowScale() const { return currentWindowScale; }

	virtual const char* GetBackendInfo() const = 0;
	virtual const char* GetBackendUrl() const = 0;

	virtual void Run(BaseUI& ui) = 0;
	void Terminate();

	virtual void ShowOpenFileDialog(const FileDialogParams* fileDialogParams) const = 0;
	virtual void SetTaskbarProgress(const float value) = 0;

protected:
	void SetupImGui(const bool initialize, const float windowScale);
};
