// GLFW header
#include <GLFW/glfw3.h>
// Window class header
#include "graphics\Window.h"

//Rendering headers
#include "core/render/d3d11/DXRender.h"
//include "core/render/vulkan/VKRender.h"
//#include "core/render/opengl/GLRender.h"
int main()
{

    if (!glfwInit())
        return -1;

    {
        Window window; //this is the window right? technically yes but the GLFWwindow* m_Window pointing to a GLFWwindow in this is accessed as window.GetWindow(); which is what we're currently doing.

        bool Created = window.Create(1280, 720);

        DXRender d3d11render;
        bool d3d11Initialized = d3d11render.Initialize(window.GetWindow()); // this automatically transforms it into a hwnd, whats a hwnd, bro its giving italian errros D: i dont know how to read italian

        // Stop if can't create
        if(!Created)
        {
            glfwTerminate();
            return -1;
        }

        if (!window.GetWindow()) 
        {
            glfwTerminate();
            return -1;
        }

        if (!d3d11Initialized)
            return -1;

        glfwMakeContextCurrent(window.GetWindow());

        while (!glfwWindowShouldClose(window.GetWindow()))
        {

            glfwPollEvents();

        }
    }

    glfwTerminate();
    return 0;

}