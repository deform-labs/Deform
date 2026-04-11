#pragma once

#include <GLFW/glfw3.h>
#include <Engine/EngineAPI.h>
#include <Engine/core/render/IRenderer.h>

class DEFORM_API GLRender : public IRenderer
{
public:
    // IRenderer interface
    bool Initialize(GLFWwindow* window) override;
    void Shutdown() override;
    void BeginFrame(const float clearColor[4]) override;
    void Present() override;
    bool IsInitialized() const override;
    void Resize(unsigned int width, unsigned int height) override;
    GLFWwindow* GetWindow() const override { return m_window; }

private:
    GLFWwindow* m_window = nullptr;
};
