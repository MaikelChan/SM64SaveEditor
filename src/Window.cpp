#include "Window.h"

#include <stdexcept>

#include <SDL3/SDL.h>

#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_sdlgpu3.h>

#include "BaseUI.h"

Window::Window(WindowParams* params)
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
	SDL_SetStringProperty(windowProperties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, params->title.c_str());
	SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, (int)(params->initialWidth * mainScale));
	SDL_SetNumberProperty(windowProperties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, (int)(params->initialHeight * mainScale));
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
	SDL_SetStringProperty(deviceProperties, SDL_PROP_GPU_DEVICE_CREATE_NAME_STRING, "vulkan");
	SDL_SetBooleanProperty(deviceProperties, SDL_PROP_GPU_DEVICE_CREATE_PREFERLOWPOWER_BOOLEAN, true);
	gpuDevice = SDL_CreateGPUDeviceWithProperties(deviceProperties);
	SDL_DestroyProperties(deviceProperties);

	if (gpuDevice == nullptr)
	{
		throw std::runtime_error(std::string("Error - SDL_CreateGPUDevice(): ") + SDL_GetError());
	}

	driverName = SDL_GetGPUDeviceDriver(gpuDevice);

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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.IniFilename = NULL;

	if (params->configureStyleCallback)
	{
		params->configureStyleCallback(ImGui::GetStyle().Colors);
	}

	if (params->configureFontsCallback)
	{
		params->configureFontsCallback(io.Fonts);
	}

	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(mainScale);
	style.FontScaleDpi = mainScale;

	// Setup Platform/Renderer backends

	ImGui_ImplSDL3_InitForSDLGPU(window);
	ImGui_ImplSDLGPU3_InitInfo init_info = {};
	init_info.Device = gpuDevice;
	init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(gpuDevice, window);
	init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
	init_info.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
	init_info.PresentMode = presentMode;
	ImGui_ImplSDLGPU3_Init(&init_info);
}

Window::~Window()
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

void Window::Run(BaseUI* ui)
{
	ui->SetIsVisible(true);

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

		ui->Render();

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

	SDL_WaitForGPUIdle(gpuDevice);
	ImGui_ImplSDL3_Shutdown();
	ImGui_ImplSDLGPU3_Shutdown();
	ImGui::DestroyContext();
}

void Window::Terminate()
{
	isRunning = false;
}