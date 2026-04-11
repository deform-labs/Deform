#include <GLFW/glfw3.h>

#include "GLRender.h"
#include "../../../../Engine.h"

using namespace deform;

/* 
    Initializes the OpenGL renderer with the provided GLFW window.
    Makes the window's OpenGL context current, enables vsync, and sets up the viewport.
    Returns true if initialization succeeds, false otherwise.
*/
bool GLRender::Initialize(GLFWwindow* window)
{
    if (!window)
    {
        Logger::FatalError("GLRender::Initialize received a null window.");
        return false;
    }

    m_window = window;
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(m_window, &width, &height);
    glViewport(0, 0, width, height);

    Logger::Log("OpenGL initialization successful.");
    return true;
}

/* 
    Releases OpenGL resources and cleans up the renderer.
    Detaches the OpenGL context from the current thread if it was active.
*/
void GLRender::Shutdown()
{
    if (glfwGetCurrentContext() == m_window)
    {
        glfwMakeContextCurrent(nullptr);
    }

    m_window = nullptr;
}

/* 
    Resizes the OpenGL viewport to match new window dimensions.
    Should be called when the window is resized to maintain proper rendering aspect ratio.
*/
void GLRender::Resize(unsigned int width, unsigned int height)
{
    if (!m_window)
        return;

    glfwMakeContextCurrent(m_window);
    glViewport(0, 0, width, height);
}

/* 
    Begins a new frame by making the OpenGL context current and clearing the buffer.
    Sets the viewport and clears the color buffer with the specified clear color.
*/
void GLRender::BeginFrame(const float clearColor[4])
{
    if (!m_window)
        return;

    glfwMakeContextCurrent(m_window);

    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(m_window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}

/* 
    Presents the rendered frame to the display by swapping the front and back buffers.
*/
void GLRender::Present()
{
    if (m_window)
    {
        glfwSwapBuffers(m_window);
    }
}

/* 
    Checks if the renderer has been successfully initialized.
    Returns true if the renderer is ready for rendering, false otherwise.
*/
bool GLRender::IsInitialized() const
{
    return m_window != nullptr;
}
