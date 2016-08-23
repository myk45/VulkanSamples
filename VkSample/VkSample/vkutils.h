#ifndef __VK_UTILS__
#define __VK_UTILS__

// STD lib.
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <functional>
#include <limits>

// STL
#include <vector>

// GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Vulkan
#include <vulkan/vulkan.h>

#define VK_LOG printf

static const int WIDTH  = 800;
static const int HEIGHT = 600;

/* All utility wrapper classses will start with an 'm' prefix */
class mVkInstance 
{
public:
    // Constructor does nothing.
    mVkInstance();

    // Instance is created here. Note that Vulkan usually needs a lot of initializations
    // to be done. We paranthesize the value list to make code more readable.
    bool createInstance();

    inline const VkInstance& getInstance() const
    {
        return _inst;
    }

    ~mVkInstance();

private:
    VkInstance          _inst;
};

class glfwWindowHelper
{
public:
    glfwWindowHelper()  {}
    ~glfwWindowHelper() {}

    bool createWindow(int width, int height)
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        _window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
        assert(_window);

        return _window != nullptr;
    }

    bool createWindowSurface(const mVkInstance& inst)
    {
        if (glfwCreateWindowSurface(inst.getInstance(), _window, nullptr, &_surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }

        return true;
    }

    inline /*const*/ GLFWwindow* getWindowHandle() const
    {
        return _window;
    }

    inline const VkSurfaceKHR& getSurface() const
    {
        return _surface;
    }

private:
    GLFWwindow*         _window;
    VkSurfaceKHR        _surface;
};


class mVkDevice
{
public:
    mVkDevice()  {}
    ~mVkDevice() {}

    // This also gets the list of queues supported and gets the index of the Graphics queue
    bool enumeratePhysicalDevices(const mVkInstance& inst, const glfwWindowHelper& glfwWindow);

    bool createDevice();
    bool createDeviceQueue();

    inline const VkPhysicalDevice& getPhysicalDevice() const
    {
        return _gpu;
    }

    inline const VkDevice& getDevice() const
    {
        return _device;
    }

    inline const VkQueue& getDeviceQueue() const
    {
        return _deviceQueue;
    }

    inline uint32_t getQueueFamilyIndex() const
    {
        return _queueFamilyIndex;
    }

    inline uint32_t getPresentFamilyIndex() const
    {
        return _presentQueueFamilyIndex;
    }

    inline const VkQueue& getPresentQueue() const
    {
        return _presentQueue;
    }

private:
    bool getPhysicalDeviceProperties(const mVkInstance& inst, const glfwWindowHelper& glfwWindow);

    unsigned int        _gpuCount;
    VkPhysicalDevice    _gpu; // Let's assume 1 GPU.
    VkDevice            _device;
    VkQueue             _deviceQueue;
    VkQueue             _presentQueue;
    uint32_t            _queueFamilyCount;
    uint32_t            _queueFamilyIndex;
    uint32_t            _presentQueueFamilyIndex;
};

class mVkSwapChain
{
public:
    mVkSwapChain();
    ~mVkSwapChain();

    bool createSwapChain(const mVkDevice& gpu, const glfwWindowHelper& glfwWindow);
    bool createSwapChainImageView(const mVkDevice& gpu);
    bool createFramebuffers(const mVkDevice& gpu);

    inline const VkSwapchainKHR& getSwapChain() const
    {
        return _swapChain;
    }

    inline const VkFormat& getColorFormat() const
    {
        return _colorFormat;
    }

    inline const std::vector<VkFramebuffer>& getFrameBuffers() const
    {
        return _swapChainFramebuffers;
    }

private:
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
            return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        }

        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != 150000) {
            return capabilities.currentExtent;
        }
        else {
            VkExtent2D actualExtent = { WIDTH, HEIGHT };

            actualExtent.width = WIDTH;// max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = HEIGHT;// max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    SwapChainSupportDetails querySwapChainSupport(const mVkDevice& gpu, const glfwWindowHelper& glfwWindow)
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu.getPhysicalDevice(), glfwWindow.getSurface(), &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.getPhysicalDevice(), glfwWindow.getSurface(), &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.getPhysicalDevice(), glfwWindow.getSurface(), &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu.getPhysicalDevice(), glfwWindow.getSurface(), &presentModeCount, nullptr);

        if (presentModeCount != 0) 
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(gpu.getPhysicalDevice(), glfwWindow.getSurface(), &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    uint32_t                   _width;
    uint32_t                   _height;
    VkSwapchainKHR             _swapChain;
    VkFormat                   _colorFormat;
    VkExtent2D                 _swapChainExtent;
    std::vector<VkImage>       _swapChainImages;
    std::vector<VkImageView>   _swapChainImageViews;
    std::vector<VkFramebuffer> _swapChainFramebuffers;
};


// This class has a list of command buffers.
// The command buffer management is this class' responsibility.
class mVkCommandPool 
{
public:
    mVkCommandPool()   {}
    ~mVkCommandPool()  {}

    bool createCommandPool(const mVkDevice& gpu);
    bool createCommandBuffer(const mVkDevice& gpu);

    inline const std::vector<VkCommandBuffer>& getDrawCmdBuf() const
    {
        return _drawCmdBufs;
    }

private:
    bool createInternalCommandBuffer(const mVkDevice& gpu);

    VkCommandPool                    _commandPool;
    VkCommandBuffer                  _setupCmdBuf;
    std::vector<VkCommandBuffer>     _drawCmdBufs;
};

class mVkGraphicsPipeline
{
public:
    mVkGraphicsPipeline()  {}
    ~mVkGraphicsPipeline() {}

    bool createGraphicsPipeline(const mVkDevice& gpu);

    inline const VkPipeline& getGraphicsPipeline() const
    {
        return _graphicsPipeline;
    }

private:
    VkPipeline       _graphicsPipeline;
    VkPipelineLayout _pipelineLayout;
};

class mVkSemaphore
{
public:
    mVkSemaphore()  {}
    ~mVkSemaphore() {}

    bool createSemaphore(const mVkDevice& gpu);

    inline const VkSemaphore& getImageAvailableSem() const
    {
        return _imageAvailableSemaphore;
    }

    inline const VkSemaphore& getRenderFinishedSem() const
    {
        return _renderFinishedSemaphore;
    }

private:
    VkSemaphore         _imageAvailableSemaphore;
    VkSemaphore         _renderFinishedSemaphore;
};

// TEMP CODE!
bool createRenderPass(const mVkDevice& gpu);
bool draw(const mVkCommandPool& cmdPool, mVkSwapChain swapChain, const mVkGraphicsPipeline& pipe);
bool drawFrame(const mVkCommandPool& cmdPool, const mVkDevice& gpu, mVkSwapChain swapChain, const mVkSemaphore& sem);

#endif