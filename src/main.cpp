// main.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <cstdio>

#include "graphics/Window.h"
#include "core/render/d3d11/DXRender.h"
#include "config.h"

config CONF;
Window GLFWHandler;
DXRender d3d11renderer;

int main(void)
{
    bool GLFW_INITIALIZED = InitGLFW();
    bool D3D11_INITIALIZED = Initd3d11();
    bool IMGUI_INITIALIZED = InitImGui();

    return 0;
}

bool InitGLFW()
{

    if (!GLFWHandler.Create(1280, 720))
    {
        glfwTerminate();
        return -1;
    }

    if (!GLFWHandler.GetWindow())
    {
        glfwTerminate();
        return -1;
    }

    // Make the OpenGL context current
    glfwMakeContextCurrent(GLFWHandler.GetWindow());

    // Main render loop
    while (!glfwWindowShouldClose(GLFWHandler.GetWindow()))
    {
    }
}

bool Initd3d11()
{
    d3d11renderer.Initialize(GLFWHandler.GetWindow());
}

bool InitGL()
{
    // TODO: make GL INIT
}

bool InitImGui()
{
    ImGui::Begin("Engine Framework", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    Color4 color;

    ImVec4 Background_color = color.toImVec4();
    char hexInput[16];

    if (ImGui::ColorPicker4("##picker", (float *)&Background_color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_DisplayHex))
    {
        color = Color4::fromImVec4(Background_color);
        CONF.updateHexInput(hexInput, color);
    }

    ImGui::Separator();
}