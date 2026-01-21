// Defining GLFW_EXPOSE_NATIVE_WIN32 through pre-processor statements
#define GLFW_EXPOSE_NATIVE_WIN32
//GLFW headers
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
//DirectX11 headers and library
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include <dxgi.h>

//header
#include "DXRender.h"

bool DXRender::Initialize(GLFWwindow* GLFWhwnd)
{
    HWND hwnd = glfwGetWin32Window(GLFWhwnd);
    
    // Initialize Direct3D 11 here
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Device *device = nullptr;
    ID3D11DeviceContext *context = nullptr;

    IDXGISwapChain *swapChain = nullptr;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,                  // Specify nullptr to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE, // Create a device using the hardware graphics driver
        0,                        // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE
        0,                        // Set debug and Direct2D compatibility flags
        nullptr,                  // List of feature levels this app can support
        0,                        // Number of entries in the previous array
        D3D11_SDK_VERSION,
        &scd,          // Swap chain description
        &swapChain,    // Returns the swap chain created
        &device,       // Returns the Direct3D device created
        &featureLevel, // Returns feature level of device created
        &context       // Returns the device immediate context
    );

    // inside Initialize, after D3D11CreateDeviceAndSwapChain succeeds
    m_swapChain = swapChain;
    m_device = device;
    m_context = context;

    // Create RTV (required for clearing / rendering)
    ID3D11Texture2D *pBackBuffer = nullptr;
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
    m_device->CreateRenderTargetView(pBackBuffer, NULL, &m_rtv);
    pBackBuffer->Release();

    // Optional: set viewport once (you can move this later)
    D3D11_VIEWPORT vp = {};
    vp.Width = 1280; // or get from window size
    vp.Height = 720;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    m_context->RSSetViewports(1, &vp);

    m_context->OMSetRenderTargets(1, &m_rtv, nullptr);

    float clearColor[4] = {0.05f, 0.5f, 1.0f, 0.0f};  //BG color

    // set the BG to the color
    m_context->ClearRenderTargetView(m_rtv, clearColor);

    
    return SUCCEEDED(hr);
}