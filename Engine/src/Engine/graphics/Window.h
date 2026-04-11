#pragma once

#include <GLFW/glfw3.h>
#include <Engine/EngineAPI.h>

enum class WindowGraphicsAPI
{
    DX11,
    VULKAN,
    OpenGL
};

class DEFORM_API Window {
    public:

        // CONSTRUCTOR AND DESTRUCTOR
        Window();

        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        Window(Window&&) noexcept;
        Window& operator=(Window&&) noexcept;

        // Create a new window with the specified title, icon, dimensions, and graphics API. Returns true if the window was created successfully, false otherwise.
        bool Create(const char* name, const char* iconPath, int width, int height, WindowGraphicsAPI api);

        // Create a new window with the specified title, dimensions, and graphics API. Returns true if the window was created successfully, false otherwise.
        bool Create(const char* name, int width, int height, WindowGraphicsAPI api);

        // Destroy the window and clean up resources
        void Destroy();

        // Poll for window events (e.g., input, window close)
        void PollEvents() const;

        // Check if the window should close (e.g., if the user has requested to close the window)
        bool ShouldClose() const;

        // Get the GLFW window handle for use with graphics API initialization and other operations that require direct access to the window.
        GLFWwindow* GetWindow() const;

        // Get the size of the framebuffer (in pixels) for the window, which may differ from the window size due to high-DPI scaling. Outputs the width and height through reference parameters.
        void GetFramebufferSize(int& width, int& height) const;

        // Get the Height of the window in pixels
        int GetHeight() const;
        
        // Get the Width of the window in pixels
        int GetWidth() const;

    private:
        // The GLFW window handle used for rendering and event handling. This is initialized when the window is created and should be destroyed when the window is destroyed.
        GLFWwindow* m_Window = nullptr;

        // The current width of the window in pixels. This is updated whenever the window is resized and can be used to adjust rendering parameters (e.g., viewport size) accordingly.
        int m_Width = 0;

        // The current height of the window in pixels. This is updated whenever the window is resized and can be used to adjust rendering parameters (e.g., viewport size) accordingly.
        int m_Height = 0;

        // Flag indicating whether the window was successfully created. This is set to true after a successful call to Create() and can be used to check if the window is valid before performing operations on it.
        bool m_Created = false;

         // The name of the graphics API being used by the window (e.g., "DirectX 11", "Vulkan", "OpenGL"). This is set during window creation based on the specified graphics API and can be used for display purposes (e.g., in the window title).
        const char* RendererName;
};
