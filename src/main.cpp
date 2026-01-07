#include "main.h"
#include "Config.h"
#include "MainUI.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_sdlgpu3.h>

#include "sm64.ttf.h"

SDL_Window* window = nullptr;
const char* driverName = nullptr;
bool isRunning = false;

int main()
{
	// Initialize SDL

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		printf("Error: SDL_Init(): %s\n", SDL_GetError());
		return -1;
	}

	// Window title

	char title[64];
	snprintf(title, 64, "%s - v%s", WINDOW_TITLE, PROJECT_VER);

	// Create window

	float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
#if SUPPORT_TRANSPARENCY
	SDL_WindowFlags windowFlags = SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_TRANSPARENT;
#else
	SDL_WindowFlags windowFlags = SDL_WINDOW_HIGH_PIXEL_DENSITY;
#endif
	SDL_PropertiesID windowProperties = SDL_CreateProperties();
	SDL_SetStringProperty(windowProperties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title);
	SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, (int)(WINDOW_WIDTH * mainScale));
	SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, (int)(WINDOW_HEIGHT * mainScale));
	SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, windowFlags);
	SDL_Window* window = SDL_CreateWindowWithProperties(windowProperties);
	SDL_DestroyProperties(windowProperties);

	if (window == nullptr)
	{
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return -2;
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
	SDL_SetStringProperty(deviceProperties, SDL_PROP_GPU_DEVICE_CREATE_NAME_STRING, "vulkan");
	SDL_SetBooleanProperty(deviceProperties, SDL_PROP_GPU_DEVICE_CREATE_PREFERLOWPOWER_BOOLEAN, true);
	SDL_GPUDevice* gpu_device = SDL_CreateGPUDeviceWithProperties(deviceProperties);
	SDL_DestroyProperties(deviceProperties);

	if (gpu_device == nullptr)
	{
		printf("Error: SDL_CreateGPUDevice(): %s\n", SDL_GetError());
		return -3;
	}

	driverName = SDL_GetGPUDeviceDriver(gpu_device);

	// Claim window for GPU Device

	if (!SDL_ClaimWindowForGPUDevice(gpu_device, window))
	{
		printf("Error: SDL_ClaimWindowForGPUDevice(): %s\n", SDL_GetError());
		return -4;
	}

	// Configure Present mode

	const SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;
	SDL_SetGPUSwapchainParameters(gpu_device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, presentMode);
	SDL_SetGPUAllowedFramesInFlight(gpu_device, 1);

	// Imgui

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.IniFilename = NULL;

	ImFontConfig config;
	config.MergeMode = true;

	ImVector<ImWchar> ranges;
	ImFontGlyphRangesBuilder builder;
	builder.AddChar(0x0100); // Coin
	builder.AddChar(0x0101); // Star
	builder.BuildRanges(&ranges);

	io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromMemoryCompressedTTF(sm64_ttf_compressed_data, sm64_ttf_compressed_size, 14, &config, ranges.Data);

	//ImGui::StyleColorsDark();
	SetImGuiStyle();

	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(mainScale);
	style.FontScaleDpi = mainScale;

	// Setup Platform/Renderer backends

	ImGui_ImplSDL3_InitForSDLGPU(window);
	ImGui_ImplSDLGPU3_InitInfo init_info = {};
	init_info.Device = gpu_device;
	init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(gpu_device, window);
	init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
	init_info.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
	init_info.PresentMode = presentMode;
	ImGui_ImplSDLGPU3_Init(&init_info);

	MainUI* mainUI = new MainUI();
	mainUI->SetIsVisible(true);

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
				case SDL_EVENT_QUIT:
					isRunning = false;
					break;

				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
					if (event.window.windowID == SDL_GetWindowID(window)) isRunning = false;
					break;
			}
		}

		// Do delays while the window is minimized

		if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
		{
			SDL_Delay(10);
			continue;
		}

		// Start the Dear ImGui frame

		ImGui_ImplSDLGPU3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		// Process UI and ImGui

		mainUI->Render();

		ImGui::Render();

		// SDL Rendering

		ImDrawData* draw_data = ImGui::GetDrawData();
		const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

		SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(gpu_device);

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
			target_info.clear_color = SDL_FColor{ 0.1f * windowOpacity, 0.025f * windowOpacity, 0.05f * windowOpacity, windowOpacity };
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

	delete mainUI;
	mainUI = nullptr;

	SDL_WaitForGPUIdle(gpu_device);
	ImGui_ImplSDL3_Shutdown();
	ImGui_ImplSDLGPU3_Shutdown();
	ImGui::DestroyContext();

	SDL_ReleaseWindowFromGPUDevice(gpu_device, window);
	SDL_DestroyGPUDevice(gpu_device);
	SDL_DestroyWindow(window);
	window = nullptr;
	SDL_Quit();

	return 0;
}

void CloseMainWindow()
{
	isRunning = false;
}

void SetImGuiStyle()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
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
}

const char* GetBackend()
{
	return driverName;
}

void ShowOpenFileDialog(std::filesystem::path defaultLocation, void* userData, SDL_DialogFileCallback callback)
{
	if (window == nullptr) return;

	SDL_PropertiesID dialogProperties = SDL_CreateProperties();

	SDL_SetPointerProperty(dialogProperties, SDL_PROP_FILE_DIALOG_FILTERS_POINTER, (void*)openDialogFilters);
	SDL_SetNumberProperty(dialogProperties, SDL_PROP_FILE_DIALOG_NFILTERS_NUMBER, OPEN_DIALOG_FILTERS_COUNT);
	SDL_SetPointerProperty(dialogProperties, SDL_PROP_FILE_DIALOG_WINDOW_POINTER, window);
	SDL_SetBooleanProperty(dialogProperties, SDL_PROP_FILE_DIALOG_MANY_BOOLEAN, false);
	SDL_SetStringProperty(dialogProperties, SDL_PROP_FILE_DIALOG_TITLE_STRING, OPEN_DIALOG_TITLE);

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

	SDL_ShowFileDialogWithProperties(SDL_FILEDIALOG_OPENFILE, callback, userData, dialogProperties);

	SDL_DestroyProperties(dialogProperties);
}