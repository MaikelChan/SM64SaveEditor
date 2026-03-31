#if !NDEBUG
// Test leaks with _CrtDumpMemoryLeaks()
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <SDL3/SDL.h>
#include <imgui/imgui.h>

#include "Window.h"
#include "MainUI.h"

#include "Game/sm64.ttf.h"

int RunMain()
{
	const SDL_DialogFileFilter openDialogFilters[] =
	{
		{ "EEP Saves (*.eep)", "eep" },
		{ "BIN Saves (*.bin)", "bin" },
		{ "All files (*.*)", "*" }
	};

	WindowParams windowParams = {};
	windowParams.title = "Super Mario 64 - Save Editor";
	windowParams.description = "This is a Super Mario 64 cross-platform save editor.\nIt is compatible with saves of the Nintendo 64 version and the PC port.";
	windowParams.author = "PacoChan";
	windowParams.url = "https://pacochan.net/software/sm64-save-editor/";
	windowParams.initialWidth = 800;
	windowParams.initialHeight = 600;
	windowParams.openDialogTitle = "Open a Super Mario 64 save file";
	windowParams.openDialogFiltersCount = SDL_arraysize(openDialogFilters);
	windowParams.openDialogFilters = openDialogFilters;
	windowParams.configureStyleCallback = [](ImVec4* colors)
		{
			ImGui::StyleColorsDark();

			colors[ImGuiCol_FrameBg] = ImVec4(0.30f, 0.08f, 0.15f, 0.54f);
			colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.12f, 0.20f, 0.54f);
			colors[ImGuiCol_FrameBgActive] = ImVec4(0.39f, 0.12f, 0.21f, 0.67f);
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.39f, 0.11f, 0.20f, 1.00f);
			colors[ImGuiCol_CheckMark] = ImVec4(0.68f, 0.33f, 0.45f, 1.00f);
			colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.20f, 0.31f, 1.00f);
			colors[ImGuiCol_SliderGrabActive] = ImVec4(0.59f, 0.24f, 0.36f, 1.00f);
			colors[ImGuiCol_Button] = ImVec4(0.40f, 0.14f, 0.23f, 0.59f);
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.51f, 0.20f, 0.31f, 1.00f);
			colors[ImGuiCol_ButtonActive] = ImVec4(0.27f, 0.07f, 0.14f, 1.00f);
			colors[ImGuiCol_Header] = ImVec4(0.50f, 0.15f, 0.26f, 0.31f);
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.46f, 0.13f, 0.24f, 0.80f);
			colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.15f, 0.28f, 0.98f);
			colors[ImGuiCol_Separator] = ImVec4(0.47f, 0.30f, 0.36f, 0.50f);
			colors[ImGuiCol_Tab] = ImVec4(0.31f, 0.09f, 0.16f, 0.86f);
			colors[ImGuiCol_TabHovered] = ImVec4(0.55f, 0.18f, 0.31f, 1.00f);
			colors[ImGuiCol_TabSelected] = ImVec4(0.54f, 0.21f, 0.32f, 1.00f);
			colors[ImGuiCol_TabDimmed] = ImVec4(0.13f, 0.03f, 0.06f, 0.97f);
			colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.25f, 0.06f, 0.13f, 1.00f);
		};
	windowParams.configureFontsCallback = [](ImFontAtlas* fontAtlas)
		{
			ImFontConfig config1;
			config1.MergeMode = false;
			config1.OversampleH = 2;
			config1.OversampleV = 2;

			ImFontConfig config2;
			config2.MergeMode = true;
			config2.OversampleH = 2;
			config2.OversampleV = 2;

			//ImVector<ImWchar> ranges;
			//ImFontGlyphRangesBuilder builder;
			//builder.AddChar(0x0100); // Coin
			//builder.AddChar(0x0101); // Star
			//builder.BuildRanges(&ranges);

			fontAtlas->AddFontDefaultVector(&config1);
			fontAtlas->AddFontFromMemoryCompressedTTF(sm64_ttf_compressed_data, sm64_ttf_compressed_size, 0.0f, &config2);
		};

	try
	{
		Window window(windowParams);

		MainUI mainUi(&window);
		window.Run(mainUi);
	}
	catch (const std::runtime_error& error)
	{
		printf("%s\n", error.what());
		return -1;
	}

	return 0;
}

int main()
{
	// Run everything in RunMain so when the function exits, all the destructors,
	// including those of objects with automatic storage duration like std::string are called.
	// That will prevent _CrtDumpMemoryLeaks() from having some false positives.

	int result = RunMain();

#if !NDEBUG
	// Cause an intentional leak to check if the leak detector is working

	char* leakTest = new char[10];
	snprintf(leakTest, 10, "%s", "LEAK TEST");

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();
#endif

	return result;
}