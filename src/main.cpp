#include "win32/window/Window.h"
#include "core/render/render.h"
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    Window window;
    window.Create(hInstance, 800, 600);
    HWND hwnd = window.GetHWND();

    Render render;
    if (!render.Initialize(hwnd))
    {
        return -1; // Initialization failed
    }

    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);

    // Main message loop would go here
    MSG msg = {};
while (msg.message != WM_QUIT) {
    // Check for messages without blocking
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            // Optional: break early if needed
            break;  // or just let it fall through
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // No messages? → Render a frame
    float clearColor[4] = { 0.1f, 0.4f, 0.8f, 1.0f };
    render.m_context->ClearRenderTargetView(render.m_rtv, clearColor);

    render.m_swapChain->Present(1, 0);  // Keep VSync on for now (low GPU usage)

    // Optional tiny sleep to reduce CPU spin when minimized / idle
    // Sleep(1);  // or use a better timer-based limiter later
}

    return 0;
}