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

        _window = glfwCreateWindow(width, height, "Vulkan Samples", nullptr, nullptr);
        assert(_window);

        return _window != nullptr;
    }

    bool createWindowSurface(const mVkInstance& inst)
    {
        return glfwCreateWindowSurface(inst.getInstance(), _window, nullptr, &_surface) == VK_SUCCESS;
    }

    bool cleanUp()
    {
        glfwDestroyWindow(_window);
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

class mVkSwapChain;
class mVkRenderPass
{
public:
    bool createRenderPass(const mVkDevice& gpu, const mVkSwapChain& swapChain);

    inline const VkRenderPass& getRenderPass() const
    {
        return _renderPass;
    }

private:
    VkRenderPass     _renderPass;
};

class mVkSwapChain
{
public:
    mVkSwapChain();
    ~mVkSwapChain();

    bool createSwapChain(const mVkDevice& gpu, const glfwWindowHelper& glfwWindow);
    bool createSwapChainImageView(const mVkDevice& gpu);
    bool createFramebuffers(const mVkDevice& gpu, const mVkRenderPass& rp);

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

    bool createGraphicsPipeline(const mVkDevice& gpu, const mVkRenderPass& rp);

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
bool recordCommands(const mVkCommandPool& cmdPool, mVkSwapChain swapChain, const mVkGraphicsPipeline& pipe, const mVkRenderPass& rp);
bool drawFrame(const mVkCommandPool& cmdPool, const mVkDevice& gpu, mVkSwapChain swapChain, const mVkSemaphore& sem);

#endif