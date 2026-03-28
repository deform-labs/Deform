#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "Editor.h"
using namespace Editor;

#include <../core/render/opengl/GLRender.h>


bool GLRender::Initgl(GLFWwindow* glfwWindow) {
	return true;
}

bool GLRender::InitImGui(GLFWwindow* glfwWindow) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true))
    {
        Logger::Logger::FatalError("Failed to initialize ImGui GLFW backend\n");
        return false;
    }

    /*
    if (!ImGui_ImplOpenGL3_Init())
    {
        Logger::FatalError("Failed to initialize ImGui OpenGL backend\n");
        return false;
    }
    */

    Logger::Log("ImGui initialization for Opengl successful.");
    return true;
}

