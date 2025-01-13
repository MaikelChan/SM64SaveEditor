#include "main.h"
#include "Config.h"
#include "MainUI.h"
#include "GLState.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "sm64.ttf.h"

#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

int glfwMajor, glfwMinor, glfwRevision{ 0 };
GLFWwindow* window = nullptr;

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void CloseMainWindow()
{
	if (window == nullptr) return;
	glfwSetWindowShouldClose(window, GLFW_TRUE);
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

int main()
{
	glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRevision);
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		return -1;
	}

	char title[64];
	snprintf(title, 64, "%s - v%s", WINDOW_TITLE, PROJECT_VER);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();

#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4)
	// In case the program is linked against GLFW shared library instead of static,
	// make sure the user has the correct version installed to use the position flags.
	const bool hasPositionFlags = (glfwMajor > 3 || (glfwMajor == 3 && glfwMinor >= 4));

	if (hasPositionFlags && monitor)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		if (mode)
		{
			glfwWindowHint(GLFW_POSITION_X, (mode->width >> 1) - (WINDOW_WIDTH >> 1));
			glfwWindowHint(GLFW_POSITION_Y, (mode->height >> 1) - (WINDOW_HEIGHT >> 1));
		}
	}
#else
	const bool hasPositionFlags = false;
#endif

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title, NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		printf("Failed to initialize OpenGL context.\n");
		return -1;
	}

	if (!hasPositionFlags && monitor)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		if (mode)
		{
			glfwSetWindowPos(window, (mode->width >> 1) - (WINDOW_WIDTH >> 1), (mode->height >> 1) - (WINDOW_HEIGHT >> 1));
		}
	}

	glfwSwapInterval(1);

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

	/*ImFont* defaultFont =*/ io.Fonts->AddFontDefault();
	/*ImFont* sm64Font =*/ io.Fonts->AddFontFromMemoryCompressedTTF(sm64_ttf_compressed_data, sm64_ttf_compressed_size, 14, &config, ranges.Data);
	io.Fonts->Build();

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	SetImGuiStyle();

	GLState glState;

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330", &glState);

	MainUI* mainUI = new MainUI();
	mainUI->SetIsVisible(true);

	while (!glfwWindowShouldClose(window))
	{
		/* Poll for and process events */
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		mainUI->Render();

		ImGui::Render();

		/* Render here */

		// Disable scissor so when clearing the buffer to make sure it clears the whole screen
		glState.EnableScissorTest(false);

		float windowOpacity = mainUI->GetWindowOpacity();
		glState.ClearColor(0.1f * windowOpacity, 0.025f * windowOpacity, 0.05f * windowOpacity, windowOpacity);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */

		glfwSwapBuffers(window);
	}

	delete mainUI;
	mainUI = nullptr;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}