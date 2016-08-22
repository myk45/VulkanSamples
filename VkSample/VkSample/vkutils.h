#ifndef __VK_UTILS__
#define __VK_UTILS__

#include "stdafx.h"

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

// Vulkan
#include <vulkan/vulkan.h>

#define VK_LOG printf

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

class mVkDevice
{
public:
    mVkDevice()  {}
    ~mVkDevice() {}

    // This also gets the list of queues supported and gets the index of the Graphics queue
    bool enumeratePhysicalDevices(const mVkInstance& inst);

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

    inline const VkSurfaceKHR& getSurface() const
    {
        return _surface;
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
    bool getPhysicalDeviceProperties(const mVkInstance& inst);

    unsigned int        _gpuCount;
    VkPhysicalDevice    _gpu; // Let's assume 1 GPU.
    VkDevice            _device;
    VkQueue             _deviceQueue;
    VkQueue             _presentQueue;
    uint32_t            _queueFamilyCount;
    uint32_t            _queueFamilyIndex;
    uint32_t            _presentQueueFamilyIndex;
    VkSurfaceKHR        _surface;
};


class mVkSwapChain
{
public:
    mVkSwapChain();
    ~mVkSwapChain();

    bool createSwapChain(const mVkDevice& gpu);

    inline const VkSwapchainKHR& getSwapChain() const
    {
        return _swapChain;
    }

    inline const VkFormat& getColorFormat() const
    {
        return _colorFormat;
    }

private:
    uint32_t           _width;
    uint32_t           _height;
    VkSwapchainKHR     _swapChain;
    VkFormat           _colorFormat;
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

    inline const VkCommandBuffer& getDrawCmdBuf() const
    {
        return _drawCmdBuf;
    }

private:
    bool createInternalCommandBuffer(const mVkDevice& gpu);

    VkCommandPool       _commandPool;
    VkCommandBuffer     _setupCmdBuf;
    VkCommandBuffer     _drawCmdBuf;
};

class mVkImageView
{
public:
    mVkImageView()  {}
    ~mVkImageView() {}

    bool createSwapChainImageView(const mVkSwapChain& swapChain, const mVkDevice& gpu);

private:
};


// TEMP CODE!
bool createGraphicsPipeline(const mVkDevice& gpu);
bool createRenderPass(const mVkDevice& gpu);
bool createFramebuffers(const mVkDevice& gpu);
bool createSemaphores(const mVkDevice& gpu);
bool draw(const mVkCommandPool& cmdPool);
bool drawFrame(const mVkCommandPool& cmdPool, const mVkDevice& gpu, mVkSwapChain swapChain);

#endif