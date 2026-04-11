#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
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
    /// Initializes the Vulkan renderer with the provided GLFW window.
    bool Initialize(GLFWwindow* window) override;
    /// Releases all Vulkan resources and cleans up the renderer.
    void Shutdown() override;
    /// Begins a new frame by acquiring the next swapchain image for rendering.
    void BeginFrame(const float clearColor[4]) override;
    /// Presents the rendered frame to the display via the swapchain.
    void Present() override;
    /// Resizes the swapchain and rendering resources to match new window dimensions.
    void Resize(unsigned int width, unsigned int height) override;
    /// Checks if the renderer has been successfully initialized.
    bool IsInitialized() const override;
    /// Returns the GLFW window handle.
    GLFWwindow* GetWindow() const override { return m_window; }

    // Vulkan specific accessors
    /// Returns the Vulkan instance.
    VkInstance GetInstance() const { return m_instance; }
    /// Returns the selected physical device (GPU).
    VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }
    /// Returns the logical device.
    VkDevice GetDevice() const { return m_device; }
    /// Returns the graphics queue.
    VkQueue GetGraphicsQueue() const { return m_graphicsQueue; }
    /// Returns the window surface.
    VkSurfaceKHR GetSurface() const { return m_surface; }
    /// Returns the swapchain.
    VkSwapchainKHR GetSwapchain() const { return m_swapchain; }
    /// Returns the list of swapchain images.
    const std::vector<VkImage>& GetSwapchainImages() const { return m_swapchainImages; }
    /// Returns the list of swapchain image views.
    const std::vector<VkImageView>& GetSwapchainImageViews() const { return m_swapchainImageViews; }
    /// Returns the swapchain surface format.
    VkFormat GetSwapchainFormat() const { return m_swapchainFormat; }
    /// Returns the swapchain extent (width and height).
    VkExtent2D GetSwapchainExtent() const { return m_swapchainExtent; }

private:
    // Initialization helpers
    /// Creates the Vulkan instance with required extensions for GLFW.
    bool CreateInstance();
    /// Selects a suitable physical device (GPU) from available devices.
    bool SelectPhysicalDevice();
    /// Creates a logical device for interacting with the selected physical device.
    bool CreateLogicalDevice();
    /// Creates a Vulkan surface from the GLFW window.
    bool CreateSurface();
    /// Creates the swapchain for presenting rendered images to the window.
    bool CreateSwapchain();
    /// Creates image views for all swapchain images.
    bool CreateImageViews();
    /// Creates synchronization primitives (semaphores and fences) for frame synchronization.
    bool CreateSyncObjects();

    // Cleanup helpers
    /// Destroys the swapchain and releases associated resources.
    void DestroySwapchain();
    /// Destroys all swapchain image views.
    void DestroyImageViews();
    /// Destroys the logical device and releases its resources.
    void DestroyLogicalDevice();
    /// Destroys the Vulkan instance and releases all associated resources.
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
