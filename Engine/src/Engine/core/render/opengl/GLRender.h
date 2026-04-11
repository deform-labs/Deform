#pragma once

#include <GLFW/glfw3.h>
#include <Engine/EngineAPI.h>
#include <Engine/core/render/IRenderer.h>

class DEFORM_API GLRender : public IRenderer
{
public:
    // IRenderer interface
    /// Initializes the OpenGL renderer with the provided GLFW window.
    bool Initialize(GLFWwindow* window) override;
    /// Releases OpenGL resources and cleans up the renderer.
    void Shutdown() override;
    /// Begins a new frame by making the OpenGL context current and clearing the buffer.
    void BeginFrame(const float clearColor[4]) override;
    /// Presents the rendered frame to the display by swapping the front and back buffers.
    void Present() override;
    /// Checks if the renderer has been successfully initialized.
    bool IsInitialized() const override;
    /// Resizes the OpenGL viewport to match new window dimensions.
    void Resize(unsigned int width, unsigned int height) override;
    /// Returns the GLFW window handle.
    GLFWwindow* GetWindow() const override { return m_window; }

private:
    GLFWwindow* m_window = nullptr;
};
