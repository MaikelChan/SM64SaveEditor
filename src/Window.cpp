#include "Window.h"

Window::Window(const WindowParams& params) : params(params)
{

}

Window::~Window()
{

}

void Window::Terminate()
{
	isRunning = false;
}

void Window::SetupImGui(const bool initialize, const float windowScale)
{
	if (initialize)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.IniFilename = NULL;

		if (params.configureFontsCallback)
		{
			params.configureFontsCallback(io.Fonts);
		}
	}

	if (currentWindowScale != windowScale)
	{
		currentWindowScale = windowScale;

		ImGuiStyle& style = ImGui::GetStyle();
		if (!initialize) style = ImGuiStyle();

		if (params.configureStyleCallback)
		{
			params.configureStyleCallback(style.Colors);
		}

		style.ScaleAllSizes(windowScale);
		style.FontScaleDpi = windowScale;
	}
}