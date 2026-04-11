#include <GLFW/glfw3.h>
#include "Editor.h"
#include "../external/imgui/backends/imgui_impl_glfw.h"
#include "../external/imgui/backends/imgui_impl_dx11.h"

namespace Main
{
    class EditorApp
    {
    public:
        int Run()
        {
            if (!Initialize())
                return -1;
            Editor::UIState state;
            while (!m_window.ShouldClose())
            {
                m_window.PollEvents();

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
                ImGui::PopStyleVar(2);
                ImGui::DockSpace(ImGui::GetID("MainDockSpace"));
                ImGui::End();

                Editor::UI::MainBar::UpdateMainBar(state);

                if (state.showHierarchy)
                    Editor::UI::Hierarchy::UpdateHierarchy();
                if (state.showInspector)
                    Editor::UI::Inspector::UpdateInspector();
                if (state.showConsole)
                    Editor::UI::Console::UpdateConsole();
                if (state.showScene)
                {
                    ImGui::Begin("Scene", &state.showScene);
                    ImVec2 size = ImGui::GetContentRegionAvail();
                    if (m_renderer.m_sceneSRV)
                        ImGui::Image((ImTextureID)m_renderer.m_sceneSRV, size);
                    else
                        ImGui::TextUnformatted("(Scene texture not available)");
                    ImGui::End();
                }

                ImGui::SetNextWindowPos(ImVec2(8.0f, 8.0f), ImGuiCond_Always);
                ImGui::SetNextWindowBgAlpha(0.35f);

                ImGui::Begin("##fps", nullptr,
                    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
                    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                    ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
                    ImGuiWindowFlags_NoMove);
                ImGui::Text("%.1f FPS (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
                ImGui::End();

                m_renderer.m_context->OMSetRenderTargets(0, &m_renderer.m_rtv, nullptr);
                m_renderer.m_context->ClearRenderTargetView(m_renderer.m_rtv, state.clearColor);
                ImGui::Render();
                ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
                m_renderer.Present();
            }
            Shutdown();
            return 0;
        }

        bool Initialize()
        {
            bool Window = m_window.Create("Deform Editor", 1280, 720, WindowGraphicsAPI::NoAPI);
            bool d3d11 = m_renderer.Initialize(m_window.GetWindow());
            GLFWwindow* CreatedWindow = m_window.GetWindow();

            glfwSetWindowUserPointer(CreatedWindow, this);
            glfwSetFramebufferSizeCallback(CreatedWindow,
                [](GLFWwindow* window, int width, int height)
                {
                    auto* app = static_cast<EditorApp*>(glfwGetWindowUserPointer(window));
                    if (app)
                        app->m_renderer.Resize((UINT)width, (UINT)height);
                });

            if (!CreatedWindow)
            {
                deform::Logger::FatalError("Window failed");
                return false;
            }
            if (!d3d11)
            {
                deform::Logger::FatalError("Renderer initialization failed.");
                Shutdown();
                return false;
            }
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            ImGui::StyleColorsDark();
            bool ImGuiGlfwInitialized = ImGui_ImplGlfw_InitForOther(m_window.GetWindow(), true);
            //bool ImguiDX11Initialized = ImGui_ImplDX11_Init(m_renderer.GetDevice(), m_renderer.GetContext());
            if (!ImGuiGlfwInitialized)
            {
                deform::Logger::FatalError("ImGui GLFW backend initialization failed.");
                Shutdown();
                return false;
            }
            /*
            
            if (!ImguiDX11Initialized)
            {
                deform::Logger::FatalError("ImGui DX11 backend initialization failed.");
                Shutdown();
                return false;
            }
                
            */
            deform::Logger::Log("ImGui GLFW backend initialized successfully.");
            deform::Logger::Log("ImGui DX11 backend initialized successfully.");
            return true;
        }
    private:
        void Shutdown()
        {
            if (ImGui::GetCurrentContext())
            {
                ImGui_ImplDX11_Shutdown();
                ImGui_ImplGlfw_Shutdown();
                ImGui::DestroyContext();
            }
            if (m_renderer.IsInitialized())
                m_renderer.Shutdown();
            m_window.Destroy();
            glfwTerminate();
        }
        Window m_window;
        DXRender m_renderer;
        Editor::UIState m_state;
    };
}
int main()
{
    Main::EditorApp app;
    app.Initialize();
    return app.Run();
}
