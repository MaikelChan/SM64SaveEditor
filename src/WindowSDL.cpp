#include "WindowSDL.h"

#include <exception>

#include <SDL3/SDL.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_sdlgpu3.h>

#include "BaseUI.h"

WindowSDL::WindowSDL(const WindowParams& params) : Window(params)
{
	// Initialize SDL

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		throw std::runtime_error(std::string("Error - SDL_Init(): ") + SDL_GetError());
	}

	// Create window

	float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
#if SUPPORT_TRANSPARENCY
	SDL_WindowFlags windowFlags = SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_TRANSPARENT;
#else
	SDL_WindowFlags windowFlags = SDL_WINDOW_HIGH_PIXEL_DENSITY;
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

	// Create GPU Device

#if NDEBUG
	constexpr bool debugEnabled = false;
#else
	constexpr bool debugEnabled = true;
#endif

	SDL_PropertiesID deviceProperties = SDL_CreateProperties();
	SDL_SetBooleanProperty(deviceProperties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_SPIRV_BOOLEAN, true);
	SDL_SetBooleanProperty(deviceProperties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_DXIL_BOOLEAN, true);
	SDL_SetBooleanProperty(deviceProperties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_MSL_BOOLEAN, true);
	SDL_SetBooleanProperty(deviceProperties, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_METALLIB_BOOLEAN, true);
	SDL_SetBooleanProperty(deviceProperties, SDL_PROP_GPU_DEVICE_CREATE_DEBUGMODE_BOOLEAN, debugEnabled);
	SDL_SetBooleanProperty(deviceProperties, SDL_PROP_GPU_DEVICE_CREATE_FEATURE_CLIP_DISTANCE_BOOLEAN, false);
	//SDL_SetBooleanProperty(deviceProperties, SDL_PROP_GPU_DEVICE_CREATE_FEATURE_DEPTH_CLAMPING_BOOLEAN, false); // Used by ImGui
	SDL_SetBooleanProperty(deviceProperties, SDL_PROP_GPU_DEVICE_CREATE_FEATURE_INDIRECT_DRAW_FIRST_INSTANCE_BOOLEAN, false);
	SDL_SetBooleanProperty(deviceProperties, SDL_PROP_GPU_DEVICE_CREATE_FEATURE_ANISOTROPY_BOOLEAN, false);
	SDL_SetBooleanProperty(deviceProperties, SDL_PROP_GPU_DEVICE_CREATE_D3D12_ALLOW_FEWER_RESOURCE_SLOTS_BOOLEAN, true);
	//SDL_SetStringProperty(deviceProperties, SDL_PROP_GPU_DEVICE_CREATE_NAME_STRING, "vulkan");
	SDL_SetBooleanProperty(deviceProperties, SDL_PROP_GPU_DEVICE_CREATE_PREFERLOWPOWER_BOOLEAN, true);
	gpuDevice = SDL_CreateGPUDeviceWithProperties(deviceProperties);
	SDL_DestroyProperties(deviceProperties);

	if (gpuDevice == nullptr)
	{
		throw std::runtime_error(std::string("Error - SDL_CreateGPUDevice(): ") + SDL_GetError());
	}

	const char* driverName = SDL_GetGPUDeviceDriver(gpuDevice);
	int sdlVersion = SDL_GetVersion();
	snprintf(backendInfo, 64, "SDL %i.%i.%i (Lib %i.%i.%i, Backend \"%s\"):", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION, SDL_VERSIONNUM_MAJOR(sdlVersion), SDL_VERSIONNUM_MINOR(sdlVersion), SDL_VERSIONNUM_MICRO(sdlVersion), driverName);

	// Claim window for GPU Device

	if (!SDL_ClaimWindowForGPUDevice(gpuDevice, window))
	{
		throw std::runtime_error(std::string("Error - SDL_ClaimWindowForGPUDevice(): ") + SDL_GetError());
	}

	windowClaimed = true;

	// Configure Present mode

	const SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;
	SDL_SetGPUSwapchainParameters(gpuDevice, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, presentMode);
	SDL_SetGPUAllowedFramesInFlight(gpuDevice, 1);

	// Imgui

	SetupImGui(true, mainScale);

	ImGui_ImplSDL3_InitForSDLGPU(window);
	ImGui_ImplSDLGPU3_InitInfo init_info = {};
	init_info.Device = gpuDevice;
	init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(gpuDevice, window);
	init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
	init_info.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
	init_info.PresentMode = presentMode;
	ImGui_ImplSDLGPU3_Init(&init_info);
}

WindowSDL::~WindowSDL()
{
	if (windowClaimed)
	{
		SDL_ReleaseWindowFromGPUDevice(gpuDevice, window);
		windowClaimed = false;
	}

	if (gpuDevice != nullptr)
	{
		SDL_DestroyGPUDevice(gpuDevice);
		gpuDevice = nullptr;
	}

	if (window != nullptr)
	{
		SDL_DestroyWindow(window);
		window = nullptr;
	}

	// This is safe to call even if SDL_Init() failed.
	SDL_Quit();
}

void WindowSDL::Run(BaseUI& ui)
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

		ImGui_ImplSDLGPU3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		// Process UI and ImGui

		ui.Render();

		ImGui::Render();

		// SDL Rendering

		ImDrawData* draw_data = ImGui::GetDrawData();
		const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

		SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(gpuDevice);

		SDL_GPUTexture* swapchain_texture;
		SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer, window, &swapchain_texture, nullptr, nullptr);

		if (swapchain_texture != nullptr && !is_minimized)
		{
			ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, command_buffer);

#if SUPPORT_TRANSPARENCY
			const float windowOpacity = mainUI->GetWindowOpacity();
#else
			constexpr float windowOpacity = 1.0f;
#endif

			// Setup and start a render pass
			SDL_GPUColorTargetInfo target_info = {};
			target_info.texture = swapchain_texture;
			target_info.clear_color = SDL_FColor{ params.backgroundColor.x * windowOpacity, params.backgroundColor.y * windowOpacity, params.backgroundColor.z * windowOpacity, windowOpacity };
			target_info.load_op = SDL_GPU_LOADOP_CLEAR;
			target_info.store_op = SDL_GPU_STOREOP_STORE;
			target_info.mip_level = 0;
			target_info.layer_or_depth_plane = 0;
			target_info.cycle = false;
			SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer, &target_info, 1, nullptr);

			ImGui_ImplSDLGPU3_RenderDrawData(draw_data, command_buffer, render_pass);

			SDL_EndGPURenderPass(render_pass);
		}

		SDL_SubmitGPUCommandBuffer(command_buffer);
	}

	SDL_WaitForGPUIdle(gpuDevice);
	ImGui_ImplSDL3_Shutdown();
	ImGui_ImplSDLGPU3_Shutdown();
	ImGui::DestroyContext();
}

void WindowSDL::ShowOpenFileDialog(const FileDialogParams* fileDialogParams) const
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

void WindowSDL::SetTaskbarProgress(const float value)
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

void WindowSDL::OpenFileDialogCallback(void* userdata, const char* const* filelist, int filter)
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
