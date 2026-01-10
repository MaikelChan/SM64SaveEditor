#include "main.h"
#include "Window.h"
#include "Config.h"
#include "MainUI.h"

#include "sm64.ttf.h"

int main()
{
	char title[64];
	snprintf(title, 64, "%s - v%s", "Super Mario 64 - Save Editor", PROJECT_VER);

	WindowParams windowParams = {};
	windowParams.title = title;
	windowParams.initialWidth = 800;
	windowParams.initialHeight = 600;
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
			ImFontConfig config;
			config.MergeMode = true;

			ImVector<ImWchar> ranges;
			ImFontGlyphRangesBuilder builder;
			builder.AddChar(0x0100); // Coin
			builder.AddChar(0x0101); // Star
			builder.BuildRanges(&ranges);

			fontAtlas->AddFontDefault();
			fontAtlas->AddFontFromMemoryCompressedTTF(sm64_ttf_compressed_data, sm64_ttf_compressed_size, 14, &config, ranges.Data);
		};

	try
	{
		Window window(&windowParams);

		MainUI mainUi(&window);
		window.Run(&mainUi);
	}
	catch (const std::runtime_error& error)
	{
		printf("%s\n", error.what());
	}
}


void CloseMainWindow()
{

}

void SetImGuiStyle()
{

}

const char* GetBackend()
{
	return "driverName";
}

void ShowOpenFileDialog(std::filesystem::path defaultLocation, void* userData, SDL_DialogFileCallback callback)
{

}


//
//void CloseMainWindow()
//{
//	isRunning = false;
//}
//

//
//const char* GetBackend()
//{
//	return driverName;
//}
//
//void ShowOpenFileDialog(std::filesystem::path defaultLocation, void* userData, SDL_DialogFileCallback callback)
//{
//	if (window == nullptr) return;
//
//	SDL_PropertiesID dialogProperties = SDL_CreateProperties();
//
//	SDL_SetPointerProperty(dialogProperties, SDL_PROP_FILE_DIALOG_FILTERS_POINTER, (void*)openDialogFilters);
//	SDL_SetNumberProperty(dialogProperties, SDL_PROP_FILE_DIALOG_NFILTERS_NUMBER, OPEN_DIALOG_FILTERS_COUNT);
//	SDL_SetPointerProperty(dialogProperties, SDL_PROP_FILE_DIALOG_WINDOW_POINTER, window);
//	SDL_SetBooleanProperty(dialogProperties, SDL_PROP_FILE_DIALOG_MANY_BOOLEAN, false);
//	SDL_SetStringProperty(dialogProperties, SDL_PROP_FILE_DIALOG_TITLE_STRING, OPEN_DIALOG_TITLE);
//
//	if (defaultLocation.empty())
//	{
//		SDL_SetStringProperty(dialogProperties, SDL_PROP_FILE_DIALOG_LOCATION_STRING, nullptr);
//	}
//	else
//	{
//		// Make sure the path ends with a slash, or else the File Dialog will treat the last
//		// part of the path as a default file, instead of a folder.
//		defaultLocation += '/';
//
//		// Normalizing it will make sure that there are no repeated slashes, and they will be
//		// converted to the OS preferred format (on Windows / -> \), or else it will not be
//		// considered a valid path and the File Dialog will show the default OS path instead.
//		defaultLocation = defaultLocation.lexically_normal();
//
//		SDL_SetStringProperty(dialogProperties, SDL_PROP_FILE_DIALOG_LOCATION_STRING, defaultLocation.u8string().c_str());
//	}
//
//	SDL_ShowFileDialogWithProperties(SDL_FILEDIALOG_OPENFILE, callback, userData, dialogProperties);
//
//	SDL_DestroyProperties(dialogProperties);
//}