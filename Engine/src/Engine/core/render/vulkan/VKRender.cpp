#include "VKRender.h"
#include "../../../../Engine.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <algorithm>
#include <set>

using namespace deform;

/* 
    Destructor for the Vulkan renderer.
    Ensures all Vulkan resources are properly cleaned up on destruction.
*/
VKRender::~VKRender()
{
    if (m_initialized)
    {
        Shutdown();
    }
}

/* 
    Initializes the Vulkan renderer with the provided GLFW window.
    Sets up the Vulkan instance, selects a physical device, creates a logical device,
    configures the surface and swapchain, and creates synchronization primitives.
    Returns true if initialization succeeds, false otherwise.
*/
bool VKRender::Initialize(GLFWwindow* window)
{
    if (!window)
    {
        Logger::FatalError("VKRender::Initialize received a null window.");
        return false;
    }

    m_window = window;

    if (!CreateInstance())
    {
        Logger::FatalError("Failed to create Vulkan instance.");
        return false;
    }

    if (!CreateSurface())
    {
        Logger::FatalError("Failed to create Vulkan surface.");
        DestroyInstance();
        return false;
    }

    if (!SelectPhysicalDevice())
    {
        Logger::FatalError("Failed to select physical device.");
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        DestroyInstance();
        return false;
    }

    if (!CreateLogicalDevice())
    {
        Logger::FatalError("Failed to create logical device.");
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        DestroyInstance();
        return false;
    }

    if (!CreateSwapchain())
    {
        Logger::FatalError("Failed to create swapchain.");
        DestroyLogicalDevice();
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        DestroyInstance();
        return false;
    }

    if (!CreateImageViews())
    {
        Logger::FatalError("Failed to create image views.");
        DestroySwapchain();
        DestroyLogicalDevice();
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        DestroyInstance();
        return false;
    }

    if (!CreateSyncObjects())
    {
        Logger::FatalError("Failed to create synchronization objects.");
        DestroyImageViews();
        DestroySwapchain();
        DestroyLogicalDevice();
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        DestroyInstance();
        return false;
    }

    m_initialized = true;
    Logger::Log("Vulkan initialization successful.");
    return true;
}

/* 
    Releases all Vulkan resources and cleans up the renderer.
    Waits for the device to become idle, destroys swapchain and synchronization objects,
    then destroys the logical device, surface, and instance.
*/
void VKRender::Shutdown()
{
    if (!m_initialized)
        return;

    if (m_device != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(m_device);
    }

    // Destroy sync objects
    for (auto semaphore : m_imageAvailableSemaphores)
    {
        if (semaphore != VK_NULL_HANDLE)
            vkDestroySemaphore(m_device, semaphore, nullptr);
    }
    for (auto semaphore : m_renderFinishedSemaphores)
    {
        if (semaphore != VK_NULL_HANDLE)
            vkDestroySemaphore(m_device, semaphore, nullptr);
    }
    for (auto fence : m_inFlightFences)
    {
        if (fence != VK_NULL_HANDLE)
            vkDestroyFence(m_device, fence, nullptr);
    }

    DestroyImageViews();
    DestroySwapchain();
    DestroyLogicalDevice();

    if (m_surface != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    DestroyInstance();

    m_window = nullptr;
    m_initialized = false;
}

/* 
    Begins a new frame by acquiring the next swapchain image for rendering.
    Waits for the current frame's in-flight fence to be signaled and acquires 
    the next image from the swapchain to prepare for rendering.
*/
void VKRender::BeginFrame(const float clearColor[4])
{
    if (!m_initialized)
        return;

    vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(
        m_device,
        m_swapchain,
        UINT64_MAX,
        m_imageAvailableSemaphores[m_currentFrame],
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(m_window, &width, &height);
        Resize(width, height);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        Logger::FatalError("Failed to acquire next image from swapchain.");
        return;
    }

    vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);
}

/* 
    Presents the rendered frame to the display via the swapchain.
    Submits the presentation request to the graphics queue and handles swapchain recreation
    if necessary due to out-of-date or suboptimal conditions.
*/
void VKRender::Present()
{
    if (!m_initialized)
        return;

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_renderFinishedSemaphores[m_currentFrame];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_swapchain;
    presentInfo.pImageIndices = &m_currentFrame;

    VkResult result = vkQueuePresentKHR(m_graphicsQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(m_window, &width, &height);
        Resize(width, height);
    }
    else if (result != VK_SUCCESS)
    {
        Logger::FatalError("Failed to present swapchain image.");
        return;
    }

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

/* 
    Resizes the swapchain and rendering resources to match new window dimensions.
    Recreates the swapchain with the new dimensions to maintain proper rendering surfaces.
    Resizes the swapchain and rendering resources to match new window dimensions.
    Recreates the swapchain with the new dimensions to maintain proper rendering surfaces.
*/
void VKRender::Resize(unsigned int width, unsigned int height)
{
    if (!m_initialized || width == 0 || height == 0)
        return;

    vkDeviceWaitIdle(m_device);
    DestroyImageViews();
    DestroySwapchain();
    CreateSwapchain();
    CreateImageViews();
}

/* 
    Checks if the renderer has been successfully initialized.
    Returns true if the renderer is ready for rendering, false otherwise.
*/
bool VKRender::IsInitialized() const
{
    return m_initialized;
}

/* 
    Creates the Vulkan instance with required extensions for GLFW.
    The instance is needed to interact with the Vulkan loader and ICD.
*/
bool VKRender::CreateInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Deform Engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Deform Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

/* 
    Selects a suitable physical device (GPU) from available devices.
    Queries for graphics and presentation queue families to ensure the device supports required operations.
    Selects a suitable physical device (GPU) from available devices.
    Queries for graphics and presentation queue families to ensure the device supports required operations.
*/
bool VKRender::SelectPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        return false;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    m_physicalDevice = devices[0];

    // Find queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilyCount; ++i)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            m_graphicsQueueFamilyIndex = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, i, m_surface, &presentSupport);

        if (presentSupport)
        {
            m_presentQueueFamilyIndex = i;
        }

        if (m_graphicsQueueFamilyIndex != UINT32_MAX && m_presentQueueFamilyIndex != UINT32_MAX)
        {
            break;
        }
    }

    return m_graphicsQueueFamilyIndex != UINT32_MAX && m_presentQueueFamilyIndex != UINT32_MAX;
}

/* 
    Creates a logical device for interacting with the selected physical device.
    Sets up command queues for graphics and presentation operations.
    Creates a logical device for interacting with the selected physical device.
    Sets up command queues for graphics and presentation operations.
*/
bool VKRender::CreateLogicalDevice()
{
    std::set<uint32_t> uniqueQueueFamilies = {m_graphicsQueueFamilyIndex, m_presentQueueFamilyIndex};
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    const char* deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 1;
    createInfo.ppEnabledExtensionNames = deviceExtensions;

    if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
    {
        return false;
    }

    vkGetDeviceQueue(m_device, m_graphicsQueueFamilyIndex, 0, &m_graphicsQueue);

    return true;
/// Creates a Vulkan surface from the GLFW window.
/// The surface is needed to determine which images can be presented to the window.
}

/* 
    Creates a Vulkan surface from the GLFW window.
    The surface is needed to determine which images can be presented to the window.
*/
bool VKRender::CreateSurface()
{
    if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

/* 
    Creates the swapchain for presenting rendered images to the window.

    Selects appropriate surface format and presentation mode, and creates swapchain images.
    Populates the swapchain extent based on surface capabilities.

    Creates the swapchain for presenting rendered images to the window.
    Selects appropriate surface format and presentation mode, and creates swapchain images.
    Populates the swapchain extent based on surface capabilities.
*/
bool VKRender::CreateSwapchain()
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &formatCount, nullptr);

    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &formatCount, formats.data());

    VkSurfaceFormatKHR selectedFormat = formats[0];
    for (const auto& format : formats)
    {
        if (format.format == VK_FORMAT_R8G8B8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            selectedFormat = format;
            break;
        }
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, nullptr);

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, presentModes.data());

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto& mode : presentModes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            presentMode = mode;
            break;
        }
    }

    m_swapchainExtent = capabilities.currentExtent;
    if (capabilities.currentExtent.width == UINT32_MAX)
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(m_window, &width, &height);
        m_swapchainExtent.width = std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        m_swapchainExtent.height = std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }

    uint32_t imageCount = (std::max)(capabilities.minImageCount, 2u);
    if (capabilities.maxImageCount > 0)
    {
        imageCount = (std::min)(imageCount, capabilities.maxImageCount);
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = selectedFormat.format;
    createInfo.imageColorSpace = selectedFormat.colorSpace;
    createInfo.imageExtent = m_swapchainExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    uint32_t queueFamilyIndices[] = {m_graphicsQueueFamilyIndex, m_presentQueueFamilyIndex};

    if (m_graphicsQueueFamilyIndex != m_presentQueueFamilyIndex)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    m_swapchainFormat = selectedFormat.format;

    if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
    {
        return false;
    }

    uint32_t swapchainImageCount = 0;
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainImageCount, nullptr);

    m_swapchainImages.resize(swapchainImageCount);
/// Creates image views for all swapchain images.
/// Image views are needed to access and use the swapchain images in rendering operations.
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainImageCount, m_swapchainImages.data());

    return true;
}

/* 
    Creates image views for all swapchain images.
    Image views are needed to access and use the swapchain images in rendering operations.
*/
bool VKRender::CreateImageViews()
{
    m_swapchainImageViews.resize(m_swapchainImages.size());

    for (size_t i = 0; i < m_swapchainImages.size(); ++i)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_swapchainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_swapchainFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_device, &createInfo, nullptr, &m_swapchainImageViews[i]) != VK_SUCCESS)
        {
            return false;
        }
/// Creates synchronization primitives (semaphores and fences) for frame synchronization.
/// Ensures proper ordering of GPU operations and synchronization with the CPU.
    }

    return true;
}

/* 
    Creates synchronization primitives (semaphores and fences) for frame synchronization.
    Ensures proper ordering of GPU operations and synchronization with the CPU.
*/
bool VKRender::CreateSyncObjects()
{
    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
        {
            return false;
/// Destroys the swapchain and releases associated resources.
        }
    }

    return true;
}

 
// Destroys the swapchain and releases associated resources.
void VKRender::DestroySwapchain()
{
    if (m_swapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
        m_swapchain = VK_NULL_HANDLE;
    }
    m_swapchainImages.clear();
}

// Destroys all swapchain image views.
void VKRender::DestroyImageViews()
{
    for (auto imageView : m_swapchainImageViews)
    {
        if (imageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(m_device, imageView, nullptr);
        }
    }
    m_swapchainImageViews.clear();
}

// Destroys the logical device and releases its resources.
void VKRender::DestroyLogicalDevice()
{
    if (m_device != VK_NULL_HANDLE)
    {
        vkDestroyDevice(m_device, nullptr);
        m_device = VK_NULL_HANDLE;
    }
}

// Destroys the Vulkan instance and releases all associated resources.
void VKRender::DestroyInstance()
{
    if (m_instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}
