#pragma once

#include <Engine/EngineAPI.h>

struct GLFWwindow;

class DEFORM_API IRenderer
{
public:
    virtual ~IRenderer() = default;

    // Lifecycle
    virtual bool Initialize(GLFWwindow* window) = 0;
    virtual void Shutdown() = 0;
    virtual bool IsInitialized() const = 0;

    // Rendering
    virtual void BeginFrame(const float clearColor[4]) = 0;
    virtual void Present() = 0;
    virtual void Resize(unsigned int width, unsigned int height) = 0;

    // Accessors
    virtual GLFWwindow* GetWindow() const = 0;
};