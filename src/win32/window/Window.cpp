#include "Window.h"
#include <windows.h>

bool Window::Create(HINSTANCE hInstance, int width, int height) {
    // Initialize the window here

    hwnd = CreateWindowExA(
        0,                          // Extended styles
        "STATIC",                   // Predefined class; Unicode assumed 
        "Sample Window",            // Window name
        WS_OVERLAPPEDWINDOW,        // Styles 
        width, height, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,       // Parent window    
        NULL,       // Menu
        hInstance, // Instance handle
        NULL        // Additional application data
    );

    return hwnd != nullptr;
}

HWND Window::GetHWND() const {
    return hwnd;
}