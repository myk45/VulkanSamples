#include "assignmentbase.h"

#define LOG(msg) printf("\n[Info] %s", msg);

bool AssignmentBase::init()
{
    bool ret = _inst.createInstance();
    assert(ret);
    LOG("Created Instance successfully");

    ret = _glfwHelper.createWindow(800, 600);
    assert(ret);
    LOG("Window created successfully");

    ret = _glfwHelper.createWindowSurface(_inst);
    assert(ret);
    LOG("Window surface created successfully");

    ret = _dev.enumeratePhysicalDevices(_inst, _glfwHelper);
    assert(ret);
    LOG("Device enumeration done successfully");

    ret = _dev.createDevice();
    assert(ret);
    LOG("Vulkan device created");

    ret = _dev.createDeviceQueue();
    assert(ret);
    LOG("Device queue created successfully");

    ret = _swapChain.createSwapChain(_dev, _glfwHelper);
    assert(ret);
    LOG("Win32 swapchain created successfully");

    ret = _swapChain.createSwapChainImageView(_dev);
    assert(ret);
    LOG("Win32 swapchain image view created successfully");

    ret = _cmdPool.createCommandPool(_dev);
    assert(ret);
    LOG("Command pool created successfully");

    ret = _cmdPool.createCommandBuffer(_dev);
    assert(ret);
    LOG("2 command buffers created successfully");

    ret = _renderPass.createRenderPass(_dev, _swapChain);
    assert(ret);
    LOG("Render pass created successfully");

    ret = _graphicsPipeline.createGraphicsPipeline(_dev, _renderPass);
    assert(ret);
    LOG("Graphics pipeline created successfully");

    ret = _swapChain.createFramebuffers(_dev, _renderPass);
    assert(ret);
    LOG("Framebuffers created successfully");

    ret = _semaphore.createSemaphore(_dev);
    assert(ret);
    LOG("Semaphores created successfully");

    ret = recordCommands(_cmdPool, _swapChain, _graphicsPipeline, _renderPass);
    assert(ret);

    return ret;
}

bool AssignmentBase::run()
{
    return DemoBase::run();
}
