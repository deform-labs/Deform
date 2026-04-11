#pragma once

#include <GLFW/glfw3.h>
#include <Engine/EngineAPI.h>
#include <Engine/core/render/IRenderer.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

class DEFORM_API VKRender : public IRenderer
{
public:
    ~VKRender();

    // IRenderer interface
    bool Initialize(GLFWwindow* window) override;
    void Shutdown() override;
    void BeginFrame(const float clearColor[4]) override;
    void Present() override;
    void Resize(unsigned int width, unsigned int height) override;
    bool IsInitialized() const override;
    GLFWwindow* GetWindow() const override { return m_window; }

    // Vulkan specific accessors
    VkInstance GetInstance() const { return m_instance; }
    VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }
    VkDevice GetDevice() const { return m_device; }
    VkQueue GetGraphicsQueue() const { return m_graphicsQueue; }
    VkSurfaceKHR GetSurface() const { return m_surface; }
    VkSwapchainKHR GetSwapchain() const { return m_swapchain; }
    const std::vector<VkImage>& GetSwapchainImages() const { return m_swapchainImages; }
    const std::vector<VkImageView>& GetSwapchainImageViews() const { return m_swapchainImageViews; }
    VkFormat GetSwapchainFormat() const { return m_swapchainFormat; }
    VkExtent2D GetSwapchainExtent() const { return m_swapchainExtent; }

private:
    // Initialization helpers
    bool CreateInstance();
    bool SelectPhysicalDevice();
    bool CreateLogicalDevice();
    bool CreateSurface();
    bool CreateSwapchain();
    bool CreateImageViews();
    bool CreateSyncObjects();

    // Cleanup helpers
    void DestroySwapchain();
    void DestroyImageViews();
    void DestroyLogicalDevice();
    void DestroyInstance();

    // Vulkan objects
    VkInstance m_instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    VkFormat m_swapchainFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D m_swapchainExtent = {0, 0};

    std::vector<VkImage> m_swapchainImages;
    std::vector<VkImageView> m_swapchainImageViews;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;

    uint32_t m_graphicsQueueFamilyIndex = UINT32_MAX;
    uint32_t m_presentQueueFamilyIndex = UINT32_MAX;
    uint32_t m_currentFrame = 0;
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    GLFWwindow* m_window = nullptr;
    bool m_initialized = false;
};
