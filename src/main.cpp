#include "main.h"
#include "Config.h"
#include "MainUI.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

//#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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

int main()
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		return -1;
	}

	char title[64];
	snprintf(title, 64, "%s - v%s", WINDOW_TITLE, PROJECT_VER);

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

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	if (monitor)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		if (mode)
		{
			glfwSetWindowPos(window, (mode->width >> 1) - (WINDOW_WIDTH >> 1), (mode->height >> 1) - (WINDOW_HEIGHT >> 1));
		}
	}

	//gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	// Imgui

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	io.IniFilename = NULL;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	bool show_demo_window = true;

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

		glClearColor(0.1f, 0.025f, 0.05f, 0.9f);
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