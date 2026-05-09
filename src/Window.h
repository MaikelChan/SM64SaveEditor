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
	const char* name = nullptr;
	const char* pattern = nullptr;
};

struct FileDialogParams
{
	BaseUI* ui = nullptr;
	std::filesystem::path defaultLocation = {};
	FileDialogCallback callback = {};
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
#if SUPPORT_TRANSPARENCY
	float opacity = 1.0f;
#endif

public:
	Window(const WindowParams& params);
	virtual ~Window();

	const WindowParams& GetParams() const { return params; }
	inline float GetWindowScale() const { return currentWindowScale; }

#if SUPPORT_TRANSPARENCY
	inline float GetOpacity() const { return opacity; }
	inline void SetOpacity(float _opacity) { opacity = _opacity; }
#else
	inline float GetOpacity() const { return 1.0f; }
	inline void SetOpacity(float _opacity) {}
#endif

	virtual const char* GetBackendInfo() const = 0;
	virtual const char* GetBackendUrl() const = 0;

	virtual void Run(BaseUI& ui) = 0;
	void Terminate();

	virtual void ShowOpenFileDialog(const FileDialogParams* fileDialogParams) const = 0;
	virtual void SetTaskbarProgress(const float value) = 0;

protected:
	void SetupImGui(const bool initialize, const float windowScale);
};
