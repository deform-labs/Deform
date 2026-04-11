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

    // IRenderer interface
    bool Initialize(GLFWwindow* hwnd) override;
    void Shutdown() override;
    void Resize(unsigned int width, unsigned int height) override;
    bool IsInitialized() const override;
    void BeginFrame(const float clearColor[4]) override;
    void Present() override;
    GLFWwindow* GetWindow() const override { return m_window; }

    // D3D11 specific
    void BeginBackbufferPass(const float clearColor[4]) const;

    ID3D11Device* GetDevice() const { return m_device; }
    ID3D11DeviceContext* GetContext() const { return m_context; }
    ID3D11RenderTargetView* GetBackbufferRenderTargetView() const { return m_rtv; }
    ID3D11RenderTargetView* GetSceneRenderTargetView() const { return m_sceneRTV; }
    ID3D11ShaderResourceView* GetSceneTextureView() const { return m_sceneSRV; }
    IDXGISwapChain* GetSwapChain() const { return m_swapChain; }

private:
    GLFWwindow* m_window = nullptr;
};
