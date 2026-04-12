//header files

#include "Engine.h" // Include the main engine header, which likely includes core functionality and logging utilities.

#include <GLFW/glfw3.h> // Include the GLFW library for window creation and input handling
#include <string> // Include the string library for handling window titles and logging

#include "Window.h" // Include the header for the Window class, which defines the interface for creating and managing the application window and its associated graphics API context.

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// CONSTRUCTOR AND OVERLOADS
Window::Window() : m_Window(nullptr), m_Width(0), m_Height(0), m_Created(false) {}

// Create a new window with the specified title, icon, dimensions, and graphics API. Returns true if the window was created successfully, false otherwise.
bool Window::Create(const char* name, const char* iconPath, int width, int height, WindowGraphicsAPI api)
{
    if (m_Created)
        return false;

    m_Width = width;
    m_Height = height;

    if (!glfwInit())
        return false;

    // Set window hints and renderer name BEFORE creating the window
    if (api == WindowGraphicsAPI::OpenGL)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        RendererName = "OpenGL";
    }
    else if (api == WindowGraphicsAPI::DX11)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        RendererName = "DirectX 11";
    }
    else if (api == WindowGraphicsAPI::VULKAN)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        RendererName = "Vulkan";
    }

    // Build the full window title with renderer name before creating the window
    std::string fullTitle = std::string(name) + " - " + RendererName;
    m_Window = glfwCreateWindow(width, height, fullTitle.c_str(), nullptr, nullptr);
    m_Created = (m_Window != nullptr);

    int w, h, channels;
    unsigned char *pixels = stbi_load(iconPath, &w, &h, &channels, 4); // force RGBA
    if (!pixels)
    {
        deform::Logger::Log("Failed to load window icon.");
    };

    GLFWimage icon;
    icon.width = w;
    icon.height = h;
    icon.pixels = pixels;

    glfwSetWindowIcon(m_Window, 1, &icon);
    stbi_image_free(pixels);

    return m_Created;
}

// Overload of Create that doesnt take an icon path, uses a default icon instead
bool Window::Create(const char* name,  int width, int height, WindowGraphicsAPI api)
{
    if (m_Created)
        return false;

    m_Width = width;
    m_Height = height;

    if (!glfwInit())
        return false;

    // Set window hints and renderer name BEFORE creating the window
    if (api == WindowGraphicsAPI::OpenGL)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        RendererName = "OpenGL";
    }
    else if (api == WindowGraphicsAPI::DX11)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        RendererName = "DirectX 11";
    }
    else if (api == WindowGraphicsAPI::VULKAN)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        RendererName = "Vulkan";
    }

    // Build the full window title with renderer name before creating the window
    std::string fullTitle = std::string(name) + " - " + RendererName;
    m_Window = glfwCreateWindow(width, height, fullTitle.c_str(), nullptr, nullptr);
    m_Created = (m_Window != nullptr);

    return m_Created;
}

// Destroy the window and clean up resources
void Window::Destroy()
{
    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }

    m_Created = false;
    m_Width = 0;
    m_Height = 0;
}

// Poll for window events (e.g., input, window close)
void Window::PollEvents() const
{
    glfwPollEvents();
}

// Check if the window should close (e.g., if the user has clicked the close button)
bool Window::ShouldClose() const
{
    return m_Window == nullptr || glfwWindowShouldClose(m_Window);
}

// Move constructor
Window::~Window()
{
    Destroy();
}

// Get the GLFW window handle for use in rendering and event handling
GLFWwindow* Window::GetWindow() const
{
    return m_Window;
}

// Get the size of the framebuffer (in pixels) for the window
void Window::GetFramebufferSize(int& width, int& height) const
{
    width = 0;
    height = 0;

    if (m_Window)
    {
        glfwGetFramebufferSize(m_Window, &width, &height);
    }
}

// Get the current width of the window
int Window::GetWidth() const
{
    return m_Width;
}

// Get the current height of the window
int Window::GetHeight() const
{
    return m_Height;
}
