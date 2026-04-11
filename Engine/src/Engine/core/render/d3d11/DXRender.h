#pragma once

#include <GLFW/glfw3.h>
#include <Engine/EngineAPI.h>
#include <Engine/core/render/IRenderer.h>
#include <d3d11.h>
#include <dxgi.h>

class DEFORM_API DXRender : public IRenderer
{
public:
    IDXGISwapChain* m_swapChain = nullptr;
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
    ID3D11RenderTargetView* m_rtv = nullptr;

    ID3D11Texture2D* m_sceneTexture = nullptr;
    ID3D11RenderTargetView* m_sceneRTV = nullptr;
    ID3D11ShaderResourceView* m_sceneSRV = nullptr;

    ID3D11VertexShader* m_skyVSBlob = nullptr;

    /// Initializes the DirectX 11 renderer with the provided GLFW window.
    bool Initialize(GLFWwindow* hwnd) override;
    /// Releases all DirectX 11 resources and cleans up the renderer.
    void Shutdown() override;
    /// Resizes the renderer's swap chain and render targets to match new window dimensions.
    void Resize(unsigned int width, unsigned int height) override;
    /// Checks if the renderer has been successfully initialized.
    bool IsInitialized() const override;
    /// Begins a new frame by preparing the back buffer for rendering.
    void BeginFrame(const float clearColor[4]) override;
    /// Presents the rendered frame to the display.
    void Present() override;
    /// Returns the GLFW window handle.
    GLFWwindow* GetWindow() const override { return m_window; }

    /// Prepares the back buffer render target for rendering.
    void BeginBackbufferPass(const float clearColor[4]) const;

    /// Returns the DirectX 11 device.
    ID3D11Device* GetDevice() const { return m_device; }
    /// Returns the immediate context for the DirectX 11 device.
    ID3D11DeviceContext* GetContext() const { return m_context; }
    /// Returns the back buffer render target view.
    ID3D11RenderTargetView* GetBackbufferRenderTargetView() const { return m_rtv; }
    /// Returns the scene off-screen render target view.
    ID3D11RenderTargetView* GetSceneRenderTargetView() const { return m_sceneRTV; }
    /// Returns the scene texture shader resource view for ImGui rendering.
    ID3D11ShaderResourceView* GetSceneTextureView() const { return m_sceneSRV; }
    /// Returns the swap chain interface.
    IDXGISwapChain* GetSwapChain() const { return m_swapChain; }

private:
    GLFWwindow* m_window = nullptr;
};
