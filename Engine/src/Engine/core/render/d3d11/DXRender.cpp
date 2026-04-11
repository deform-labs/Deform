#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d11.h>
#include <dxgi.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "DXRender.h"
#include "Engine.h"

#include "../IRenderer.h"

using namespace deform;

namespace
{
    template <typename T>
    void ReleaseCOM(T *&resource)
    {
        if (resource)
        {
            resource->Release();
            resource = nullptr;
        }
    }
}

/*
    initializes the DirectX 11 renderer with the provided GLFW window.
    sets up the direct3d device, swap chain, render target views, and viewport.
    also creates an off-screen scene texture for rendering to imgui.
    returns true if initialization succeeds, false otherwise.
*/

bool DXRender::Initialize(GLFWwindow *glfwWindow)
{
    if (!glfwWindow)
    {
        Logger::FatalError("DXRender::Initialize received a null window.");
        return false;
    }

    m_window = glfwWindow;

    HWND hwnd = glfwGetWin32Window(glfwWindow);
    if (!hwnd)
    {
        Logger::FatalError("Failed to retrieve the native Win32 window handle.");
        return false;
    }

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &m_swapChain,
        &m_device,
        &featureLevel,
        &m_context);

    if (FAILED(hr))
    {
        Logger::FatalError("Failed to create the Direct3D 11 device and swap chain.");
        return false;
    }

    ID3D11Texture2D *backBuffer = nullptr;
    hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&backBuffer));
    if (FAILED(hr) || !backBuffer)
    {
        Logger::FatalError("Failed to get back buffer from swap chain.");
        return false;
    }

    D3D11_TEXTURE2D_DESC backBufferDesc = {};
    backBuffer->GetDesc(&backBufferDesc);
    UINT sceneWidth = 1280;
    UINT sceneHeight = 720;

    hr = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_rtv);
    backBuffer->Release();
    if (FAILED(hr) || !m_rtv)
    {
        Logger::FatalError("Failed to create render target view for back buffer.");
        return false;
    }

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<FLOAT>(sceneWidth);
    viewport.Height = static_cast<FLOAT>(sceneHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    m_context->RSSetViewports(1, &viewport);

    D3D11_TEXTURE2D_DESC sceneTextureDesc = {};
    sceneTextureDesc.Width = sceneWidth;
    sceneTextureDesc.Height = sceneHeight;
    sceneTextureDesc.MipLevels = 1;
    sceneTextureDesc.ArraySize = 1;
    sceneTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sceneTextureDesc.SampleDesc.Count = 1;
    sceneTextureDesc.SampleDesc.Quality = 0;
    sceneTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    sceneTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    hr = m_device->CreateTexture2D(&sceneTextureDesc, nullptr, &m_sceneTexture);
    if (FAILED(hr) || !m_sceneTexture)
    {
        Logger::FatalError("Failed to create scene texture.");
        return false;
    }

    hr = m_device->CreateRenderTargetView(m_sceneTexture, nullptr, &m_sceneRTV);
    if (FAILED(hr) || !m_sceneRTV)
    {
        Logger::FatalError("Failed to create scene RTV.");
        return false;
    }

    hr = m_device->CreateShaderResourceView(m_sceneTexture, nullptr, &m_sceneSRV);
    if (FAILED(hr) || !m_sceneSRV)
    {
        Logger::FatalError("Failed to create scene SRV.");
        return false;
    }

    Logger::Log("DirectX 11 initialization successful.");
    return true;
}

/*
    Releases all DirectX 11 resources and cleans up the renderer.
    Clears the device context state, then releases the swap chain,
    device, and all render target views in reverse order of creation.
*/
void DXRender::Shutdown()
{
    if (m_context)
    {
        m_context->ClearState();
    }

    ReleaseCOM(m_sceneSRV);
    ReleaseCOM(m_sceneRTV);
    ReleaseCOM(m_sceneTexture);
    ReleaseCOM(m_rtv);
    ReleaseCOM(m_context);
    ReleaseCOM(m_device);
    ReleaseCOM(m_swapChain);
}

/* 
    Resizes the renderer's swap chain and render targets to match new window dimensions.
    Recreates the back buffer render target view and scene texture with new dimensions.
    Called when the window is resized to maintain proper rendering surfaces.
*/
void DXRender::Resize(unsigned int width, unsigned int height)
{
    if (!m_swapChain || !m_device || !m_context || width == 0 || height == 0)
    {
        return;
    }

    ReleaseCOM(m_rtv);
    ReleaseCOM(m_sceneRTV);
    ReleaseCOM(m_sceneSRV);
    ReleaseCOM(m_sceneTexture);

    m_context->OMSetRenderTargets(0, nullptr, nullptr);
    m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

    ID3D11Texture2D *backBuffer = nullptr;
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&backBuffer));
    if (!backBuffer)
    {
        return;
    }

    D3D11_TEXTURE2D_DESC backDesc = {};
    backBuffer->GetDesc(&backDesc);
    m_device->CreateRenderTargetView(backBuffer, nullptr, &m_rtv);
    backBuffer->Release();

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<FLOAT>(backDesc.Width);
    viewport.Height = static_cast<FLOAT>(backDesc.Height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_context->RSSetViewports(1, &viewport);

    D3D11_TEXTURE2D_DESC sceneTextureDesc = {};
    sceneTextureDesc.Width = backDesc.Width;
    sceneTextureDesc.Height = backDesc.Height;
    sceneTextureDesc.MipLevels = 1;
    sceneTextureDesc.ArraySize = 1;
    sceneTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sceneTextureDesc.SampleDesc.Count = 1;
    sceneTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    m_device->CreateTexture2D(&sceneTextureDesc, nullptr, &m_sceneTexture);
    m_device->CreateRenderTargetView(m_sceneTexture, nullptr, &m_sceneRTV);
    m_device->CreateShaderResourceView(m_sceneTexture, nullptr, &m_sceneSRV);
}

/*
    Prepares the back buffer render target for rendering.
    Sets the back buffer as the active render target and clears it with the specified clear color.
    This should be called at the beginning of each frame for rendering to the main window.
*/
void DXRender::BeginBackbufferPass(const float clearColor[4]) const
{
    if (!m_context || !m_rtv)
    {
        return;
    }

    m_context->OMSetRenderTargets(1, &m_rtv, nullptr);
    m_context->ClearRenderTargetView(m_rtv, clearColor);
}

/* 
    Presents the rendered frame to the display.
    Swaps the back buffer with the front buffer to display the rendered content on screen.
    Blocks until the frame is presented (vsync enabled).
*/
void DXRender::Present()
{
    if (m_swapChain)
    {
        m_swapChain->Present(1, 0);
    }
}

/* 
    Begins a new frame by preparing the back buffer for rendering.
    Clears the back buffer with the specified color in preparation for drawing.
*/
void DXRender::BeginFrame(const float clearColor[4])
{
    BeginBackbufferPass(clearColor);
}

/* 
    Checks if the renderer has been successfully initialized.
    Returns true if the renderer is ready for rendering, false otherwise.
*/
bool DXRender::IsInitialized() const
{
    return m_window != nullptr;
}