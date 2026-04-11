#pragma once

#include <Engine/EngineAPI.h>

struct GLFWwindow;

class DEFORM_API IRenderer
{
public:
    // Virtual destructor to ensure proper cleanup of derived renderer classes when deleted through a base class pointer.
    virtual ~IRenderer() = default;

    // Initializes the renderer with the given GLFW window. This method should set up the necessary graphics API context and resources for rendering. Returns true if initialization is successful, false otherwise.
    virtual bool Initialize(GLFWwindow* window) = 0;
    // Shuts down the renderer and releases any allocated resources. This method should be called when the renderer is no longer needed to ensure proper cleanup.
    virtual void Shutdown() = 0;
    // Checks if the renderer has been successfully initialized and is ready for rendering operations. Returns true if the renderer is initialized, false otherwise.
    virtual bool IsInitialized() const = 0;

    // Rendering

    // Begins a new frame for rendering. This method should clear the render target using the specified clear color and prepare the renderer for drawing operations. The clearColor parameter is an array of four floats representing the RGBA values for clearing the screen.
    virtual void BeginFrame(const float clearColor[4]) = 0;
    // Ends the current frame and presents the rendered content to the screen. This method should handle any necessary synchronization and buffer swapping to display the rendered frame.
    virtual void Present() = 0;
    // Resizes the renderer's backbuffer and any associated resources to match the new window dimensions. This method should be called when the window is resized to ensure that rendering continues to work correctly with the new size.
    virtual void Resize(unsigned int width, unsigned int height) = 0;

    // Gets the GLFW window handle associated with the renderer. This method allows access to the window for operations that may require direct interaction with the windowing system or graphics API context.
    virtual GLFWwindow* GetWindow() const = 0;
};