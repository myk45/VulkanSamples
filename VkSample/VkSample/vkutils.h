#ifndef __VK_UTILS__
#define __VK_UTILS__

#include "stdafx.h"

// STD lib.
#include <cstdio>
#include <cstdlib>
#include <cassert>

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

private:
    bool getPhysicalDeviceProperties(const mVkInstance& inst);

    unsigned int        _gpuCount;
    VkPhysicalDevice    _gpu; // Let's assume 1 GPU.
    VkDevice            _device;
    VkQueue             _deviceQueue;
    uint32_t            _queueFamilyCount;
    uint32_t            _queueFamilyIndex;
    VkSurfaceKHR        _surface;
};


class mkSwapChain
{
public:
    mkSwapChain();
    ~mkSwapChain();

    bool createSwapChain(const mVkDevice& gpu);
private:
};

#endif