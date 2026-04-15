//header files
#pragma warning(disable: 4005) // Disable macro redefinition warning for ImGui headers

#include <GLFW/glfw3.h> // A library for creating windows and handling input, used for creating the editor window and managing user interactions.
#include <string>

// ImGui backends for GLFW and DirectX 11, used for rendering the editor's UI.
#include "../external/imgui/backends/imgui_impl_glfw.h"
#include "../external/imgui/backends/imgui_impl_dx11.h"

// Engine headers for logging, rendering, and window management, used for creating the renderer, managing the application window, and logging messages and errors.
#include "Engine.h" // The main header for the engine, which includes all the necessary components and systems for rendering, logging, and window management.
#include "Engine/core/render/d3d11/DXRender.h" // The DirectX 11 renderer class, responsible for initializing DirectX, creating the swap chain and render target views, and handling rendering operations.
#include "Engine/graphics/Window.h" // The Window class, responsible for creating and managing the GLFW window and its associated graphics API context.
#include "imgui.h" // The main header for ImGui, a popular immediate mode GUI library used for creating the editor's user interface.

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // A header-only library for loading images, used for loading textures and icons in the editor.

#include "Editor.h" // The header file for the editor, which includes the definition of the UIState struct and the UI panel classes.

Window m_window; //The Application window Variable, responsible for creating and managing the GLFW window and its associated graphics API context.
DXRender m_renderer; // The DirectX 11 renderer Variable, responsible for initializing DirectX, creating the swap chain and render target views, and handling rendering operations.
Editor::UIState m_state; // A struct that holds the state of the editor's UI, including which panels are visible and the clear color for the scene.

// Callback for when the window is resized. Resizes the renderer's backbuffer to match the new window size.
void FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    deform::Logger::Log("Display size updated: " + std::to_string(width) + "x" + std::to_string(height));
    deform::Logger::Log("Framebuffer size callback triggered: " + std::to_string(width) + "x" + std::to_string(height));
    m_renderer.Resize(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
};

// Clean up resources and shut down the application.
void Shutdown() {
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

// Update the editor panels based on the current UI state.
void UpdatePanels() {
    Editor::UI::MainBar::UpdateMainBar(m_state);

    if (m_state.showHierarchy)
        Editor::UI::Hierarchy::UpdateHierarchy();
    if (m_state.showInspector)
        Editor::UI::Inspector::UpdateInspector();
    if (m_state.showConsole)
        Editor::UI::Console::UpdateConsole();
    if (m_state.showScene)
    {
        ImGui::Begin("Scene", &m_state.showScene);
        ImVec2 size = ImGui::GetContentRegionAvail();
        if (m_renderer.m_sceneSRV)
            ImGui::Image((ImTextureID)m_renderer.m_sceneSRV, size);
        else
            ImGui::TextUnformatted("(Scene texture not available)");
        ImGui::End();
    }
};

// Update the full-screen dockspace for the editor panels.
void UpdateDockHost() {
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGuiWindowFlags hostWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("DockSpace Host", nullptr, hostWindowFlags);
    ImGui::PopStyleVar(2);
    ImGui::DockSpace(ImGui::GetID("DockSpace Host"), ImVec2(m_window.GetWidth(), m_window.GetHeight()), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
};

// Create the renderer and initialize it with the window's handle. Logs an error and shuts down if initialization fails.
bool CreateRenderer() {
    bool d3d11 = m_renderer.Initialize(m_window.GetWindow());
    if (!d3d11)
    {
        deform::Logger::FatalError("Renderer initialization failed.");
        Shutdown();
        return false;
    }

    return d3d11;
};

// Create the editor window with the specified title and graphics API. Logs an error and shuts down if creation fails.
bool CreateEditorWindow(const char *WindowTitle, WindowGraphicsAPI api = WindowGraphicsAPI::DX11) {
    bool CreatedWindow = m_window.Create(WindowTitle, "Window_Icon.png", 1920, 1080, api);

    if (!CreatedWindow)
    {
        deform::Logger::FatalError("Window failed");
        return false;
    }

    deform::Logger::Log("Window " + std::string(WindowTitle) + " created successfully.");
    return CreatedWindow;
}

// Initialize ImGui with the appropriate backends for the windowing system and graphics API. Logs errors and shuts down if initialization fails.
bool InitializeImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_DockingEnable;

    bool ImGuiGlfwInitialized = ImGui_ImplGlfw_InitForOther(m_window.GetWindow(), true);
    bool ImguiDX11Initialized = ImGui_ImplDX11_Init(m_renderer.GetDevice(), m_renderer.GetContext());
    if (!ImGuiGlfwInitialized)
    {
        deform::Logger::FatalError("ImGui GLFW backend initialization failed.");
        Shutdown();
        return false;
    }
    if (!ImguiDX11Initialized)
    {
        deform::Logger::FatalError("ImGui DX11 backend initialization failed.");
        Shutdown();
        return false;
    }
    deform::Logger::Log("ImGui GLFW backend initialized successfully.");
    deform::Logger::Log("ImGui DX11 backend initialized successfully.");

    return true;
}

// Main application loop. Polls for events, updates the UI panels, renders the scene and UI, and presents the backbuffer. Cleans up resources on exit.
int Run() {

    deform::Logger::Log("Entering main application loop.");
    deform::Logger::Log("ImGui Display Size: " + std::to_string(ImGui::GetIO().DisplaySize.x) + "x" + std::to_string(ImGui::GetIO().DisplaySize.y));

    while (!m_window.ShouldClose())
    {
        m_window.PollEvents();

        m_renderer.BeginFrame(m_state.clearColor);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGui::Begin("Window", nullptr);
        ImGui::Text("Hello, world!");
        ImGui::End();
        
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        m_renderer.Present();
    }

    Shutdown();
    return 0;
}

// Initialize the application by creating the editor window, initializing the renderer, setting up the framebuffer resize callback, and initializing ImGui. Logs errors and shuts down if any step fails.
int Initialize() {

    bool CreatedWindow = CreateEditorWindow("Deform Editor", WindowGraphicsAPI::DX11);
    bool RendererInitialized = CreateRenderer();
    bool ImGuiInitialized = InitializeImGui();

    if (!CreatedWindow || !RendererInitialized || !ImGuiInitialized)
    {
        deform::Logger::FatalError("Application initialization failed.");
        return -1;
    }

    glfwSetFramebufferSizeCallback(m_window.GetWindow(), FramebufferSizeCallback);
    glfwMaximizeWindow(m_window.GetWindow());

    // Set the initial DisplaySize for ImGui
    ImGui::GetIO().DisplaySize = ImVec2(static_cast<float>(m_window.GetWidth()), static_cast<float>(m_window.GetHeight()));


    return Run();
}

//Entry point of the application. Calls the main application loop and returns its exit code.
int main() {
    int Initialized = Initialize();
    return Initialized;
}
