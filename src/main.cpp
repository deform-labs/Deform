// main.cpp
#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#endif

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_dx11.h>
#include <cstdio>

#include "init.h"
#include "config.h"

#include "Editor/Editor.h"

Init Initializer;

Config config;

DXRender d3d11renderer;
Window GLFWHandler;




// Editor stuff
using namespace Editor;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    if (!Initializer.InitGLFW()) { Logger::Log("InitGLFW FAILED\n");  return -1; }
    if (!Initializer.InitD3d11()) { Logger::Log("InitD3d11 FAILED\n"); return -1; }

    ImGuiIO& io = ImGui::GetIO();


    ImFont* font = io.Fonts->AddFontFromFileTTF("src/assets/Fonts/Roboto-Regular.ttf", 14.0f);

    if (!font)
		Logger::Log("Failed to load font: Roboto-Regular.ttf\n");

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.FontDefault = font;

	Logger::Log("Initialization complete. Entering main loop.");



    while (!glfwWindowShouldClose(GLFWHandler.GetWindow()))
    {
        glfwPollEvents();

        {
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::Begin("DockSpaceHost", nullptr, window_flags);

            ImGui::SetNextWindowPos(ImVec2(8, 8), ImGuiCond_Always);
            ImGui::Text("%.1f FPS (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

            ImGui::PopStyleVar(2);

            ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

            ImGui::End();


            ImGuiViewport* main_viewport = ImGui::GetMainViewport();
            ImGui::DockSpaceOverViewport(main_viewport->ID);
        }

        UI::MainBar::UpdateMainBar();

        static bool showHierarchy = true;
        static bool showInspector = true;
        static bool showScene = true;
        static bool showConsole = true;

        {
            if (showHierarchy)
            {
                UI::Hierarchy::UpdateHierarchy();
            }

            if (showInspector)
            {
                UI::Inspector::UpdateInspector();
            }

            if (showConsole)
            {
                UI::Console::UpdateConsole();
            }

            if (showScene)
            {
                ImGui::Begin("Scene", &showScene);
                ImVec2 size = ImGui::GetContentRegionAvail();
                if (d3d11renderer.m_sceneSRV)
                    ImGui::Image((ImTextureID)d3d11renderer.m_sceneSRV, size);
                else
                    ImGui::TextUnformatted("(Scene texture not available)");
                ImGui::End();
            }
        }      
        {
            if (d3d11renderer.m_sceneRTV)
            {
                d3d11renderer.m_context->OMSetRenderTargets(1, &d3d11renderer.m_sceneRTV, nullptr);
                d3d11renderer.m_context->ClearRenderTargetView(d3d11renderer.m_sceneRTV, config.clearColor);
            }

            ID3D11DepthStencilView* nullDSV = nullptr;

            float clearColor[4] = { 0.05f, 0.5f, 1.0f, 0.0f };
            d3d11renderer.m_context->OMSetRenderTargets(1, &d3d11renderer.m_rtv, nullptr);
            d3d11renderer.m_context->ClearRenderTargetView(d3d11renderer.m_rtv, clearColor);

            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            d3d11renderer.m_swapChain->Present(1, 0);
        }
    }


	Logger::Log("Exiting main loop. Cleaning up and shutting down.");
	Sleep(100); // Give some time for the log to flush before cleanup
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    d3d11renderer.Resize((UINT)width, (UINT)height);
}

bool Init::InitGLFW()
{
    if (!glfwInit())
        return false;

    if (!GLFWHandler.Create(1280, 720))
    {
        glfwTerminate();
        return false;
    }

    if (!GLFWHandler.GetWindow())
    {
        glfwTerminate();
        return false;
    }

    glfwSetFramebufferSizeCallback(GLFWHandler.GetWindow(), framebuffer_size_callback);

	Logger::Log("GLFW initialization successful.");
    return true;
}

bool Init::InitD3d11()
{
    if (!d3d11renderer.Initialize(GLFWHandler.GetWindow()))
        return false;
    if (!d3d11renderer.InitializeImgui(GLFWHandler.GetWindow()))
        return false;
    return true;
}