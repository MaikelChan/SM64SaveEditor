#if ENABLE_OPENGL

#include "WindowSDL_GL.h"

#include <exception>

#include <imgui/imgui_impl_opengl3_loader.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_opengl3.h>

#include "BaseUI.h"

WindowSDL_GL::WindowSDL_GL(const WindowParams& params) : Window(params)
{
	// Initialize SDL

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		throw std::runtime_error(std::string("Error - SDL_Init(): ") + SDL_GetError());
	}

	// Create window

	float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
#if SUPPORT_TRANSPARENCY
	SDL_WindowFlags windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_TRANSPARENT;
#else
	SDL_WindowFlags windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY;
#endif
	SDL_PropertiesID windowProperties = SDL_CreateProperties();
	SDL_SetStringProperty(windowProperties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, params.title.c_str());
	SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, (int)(params.initialWidth * mainScale));
	SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, (int)(params.initialHeight * mainScale));
	SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, windowFlags);
	window = SDL_CreateWindowWithProperties(windowProperties);
	SDL_DestroyProperties(windowProperties);

	if (window == nullptr)
	{
		throw std::runtime_error(std::string("Error - SDL_CreateWindow(): ") + SDL_GetError());
	}

	// Initialize OpenGL

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	const char* glsl_version = "#version 130";

	glContext = SDL_GL_CreateContext(window);
	if (glContext == nullptr)
	{
		throw std::runtime_error(std::string("Error - SDL_GL_CreateContext(): ") + SDL_GetError());
	}

	SDL_GL_MakeCurrent(window, glContext);

	// Create GPU Device

#if NDEBUG
	constexpr bool debugEnabled = false;
#else
	constexpr bool debugEnabled = true;
#endif

	int sdlVersion = SDL_GetVersion();
	snprintf(backendInfo, 64, "SDL %i.%i.%i (Lib %i.%i.%i, Backend \"%s\"):", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION, SDL_VERSIONNUM_MAJOR(sdlVersion), SDL_VERSIONNUM_MINOR(sdlVersion), SDL_VERSIONNUM_MICRO(sdlVersion), "OpenGL 3.3");

	// Configure Present mode

	if (!SDL_GL_SetSwapInterval(1))
	{
		throw std::runtime_error(std::string("Error - SDL_GL_SetSwapInterval(): ") + SDL_GetError());
	}

	// Imgui

	SetupImGui(true, mainScale);

	ImGui_ImplSDL3_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

WindowSDL_GL::~WindowSDL_GL()
{
	if (window != nullptr)
	{
		SDL_DestroyWindow(window);
		window = nullptr;
	}

	// This is safe to call even if SDL_Init() failed.
	SDL_Quit();
}

void WindowSDL_GL::Run(BaseUI& ui)
{
	ui.SetIsVisible(true);

	isRunning = true;

	while (isRunning)
	{
		// Poll events

		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL3_ProcessEvent(&event);

			switch (event.type)
			{
				case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
				{
					float windowScale = SDL_GetWindowDisplayScale(window);
					SDL_SetWindowSize(window, (int)(params.initialWidth * windowScale), (int)(params.initialHeight * windowScale));
					SetupImGui(false, windowScale);

					break;
				}

				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				{
					if (event.window.windowID == SDL_GetWindowID(window)) isRunning = false;
					break;
				}

				case SDL_EVENT_QUIT:
				{
					isRunning = false;
					break;
				}
			}
		}

		// Do delays while the window is minimized

		if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
		{
			SDL_Delay(33);
			continue;
		}

		// Start the Dear ImGui frame

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		// Process UI and ImGui

		ui.Render();

		ImGui::Render();

		// SDL Rendering

		ImGuiIO& io = ImGui::GetIO();
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClearColor(params.backgroundColor.x * opacity, params.backgroundColor.y * opacity, params.backgroundColor.z * opacity, opacity);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DestroyContext(glContext);
}

void WindowSDL_GL::ShowOpenFileDialog(const FileDialogParams* fileDialogParams) const
{
	if (fileDialogParams == nullptr) return;
	if (window == nullptr) return;

	SDL_PropertiesID dialogProperties = SDL_CreateProperties();
	SDL_SetPointerProperty(dialogProperties, SDL_PROP_FILE_DIALOG_FILTERS_POINTER, (void*)params.openDialogFilters);
	SDL_SetNumberProperty(dialogProperties, SDL_PROP_FILE_DIALOG_NFILTERS_NUMBER, params.openDialogFiltersCount);
	SDL_SetPointerProperty(dialogProperties, SDL_PROP_FILE_DIALOG_WINDOW_POINTER, window);
	SDL_SetBooleanProperty(dialogProperties, SDL_PROP_FILE_DIALOG_MANY_BOOLEAN, false);
	SDL_SetStringProperty(dialogProperties, SDL_PROP_FILE_DIALOG_TITLE_STRING, params.openDialogTitle.c_str());

	std::filesystem::path defaultLocation = fileDialogParams->defaultLocation;

	if (defaultLocation.empty())
	{
		SDL_SetStringProperty(dialogProperties, SDL_PROP_FILE_DIALOG_LOCATION_STRING, nullptr);
	}
	else
	{
		// Make sure the path ends with a slash, or else the File Dialog will treat the last
		// part of the path as a default file, instead of a folder.
		defaultLocation += '/';

		// Normalizing it will make sure that there are no repeated slashes, and they will be
		// converted to the OS preferred format (on Windows / -> \), or else it will not be
		// considered a valid path and the File Dialog will show the default OS path instead.
		defaultLocation = defaultLocation.lexically_normal();

		SDL_SetStringProperty(dialogProperties, SDL_PROP_FILE_DIALOG_LOCATION_STRING, defaultLocation.u8string().c_str());
	}

	SDL_ShowFileDialogWithProperties(SDL_FILEDIALOG_OPENFILE, OpenFileDialogCallback, (void*)fileDialogParams, dialogProperties);

	SDL_DestroyProperties(dialogProperties);
}

void WindowSDL_GL::SetTaskbarProgress(const float value)
{
	if (currentTaskbarProgress == value) return;
	currentTaskbarProgress = value;

	if (value > 0.0f)
	{
		SDL_SetWindowProgressState(window, SDL_PROGRESS_STATE_NORMAL);
		SDL_SetWindowProgressValue(window, value);
	}
	else
	{
		SDL_SetWindowProgressState(window, SDL_PROGRESS_STATE_NONE);
	}
}

void WindowSDL_GL::OpenFileDialogCallback(void* userdata, const char* const* filelist, int filter)
{
	const FileDialogParams* fileDialogParams = (const FileDialogParams*)userdata;

	if (fileDialogParams->callback == nullptr) return;

	if (filelist == nullptr)
	{
		fileDialogParams->callback(fileDialogParams, "", SDL_GetError());
		return;
	}
	else if (*filelist == nullptr)
	{
		// The user did not select any file.
		fileDialogParams->callback(fileDialogParams, "", nullptr);
		return;
	}

	fileDialogParams->callback(fileDialogParams, std::filesystem::u8path(filelist[0]), nullptr);
}

#endif