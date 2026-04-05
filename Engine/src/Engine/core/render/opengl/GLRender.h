#pragma once

#include <GLFW/glfw3.h>
#include <Engine/EngineAPI.h>

class DEFORM_API GLRender
{
public:
    bool Initialize(GLFWwindow* window);
    void Shutdown();
    void BeginFrame(const float clearColor[4]) const;
    void Present() const;
    bool IsInitialized() const;

private:
    GLFWwindow* m_window = nullptr;
};
