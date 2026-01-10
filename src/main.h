#pragma once

#include <string>
#include <filesystem>
#include <SDL3/SDL.h>

#define WINDOW_TITLE "Super Mario 64 - Save Editor"

#define OPEN_DIALOG_TITLE "Open a Super Mario 64 save file"
#define OPEN_DIALOG_FILTERS_COUNT 3
const SDL_DialogFileFilter openDialogFilters[] = {
	{ "EEP Saves (*.eep)", "eep" },
	{ "BIN Saves (*.bin)", "bin" },
	{ "All files (*.*)", "*" }
};

void CloseMainWindow();
const char* GetBackend();
void ShowOpenFileDialog(std::filesystem::path defaultLocation, void* userData, SDL_DialogFileCallback callback);