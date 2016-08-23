#include "vkutils.h"


#define LOG(msg) printf("\n%s", msg);

void vkStuff()
{
    mVkInstance inst;
    mVkDevice dev;
    mVkSwapChain swapChain;
    mVkCommandPool cmdPool;
    mVkImageView   swapChainImageView;

    bool ret = inst.createInstance();
    assert(ret);
    LOG("Created Instance successfully");

    ret = dev.enumeratePhysicalDevices(inst);
    assert(ret);
    LOG("Device enumeration done successfully");

    ret = dev.createDevice();
    assert(ret);
    LOG("Vulkan device created");
    
    ret = dev.createDeviceQueue();
    assert(ret);
    LOG("Device queue created successfully");

    ret = swapChain.createSwapChain(dev);
    assert(ret);
    LOG("Win32 swapchain created successfully");

    ret = cmdPool.createCommandPool(dev);
    assert(ret);
    LOG("Command pool created successfully");

    ret = cmdPool.createCommandBuffer(dev);
    assert(ret);
    LOG("2 command buffers created successfully");

    ret = swapChainImageView.createSwapChainImageView(swapChain, dev);
    assert(ret);
    LOG("Swap chain image view created successfully");

    ret = createRenderPass(dev);
    assert(ret);
    LOG("Render pass created successfully");

    ret = createGraphicsPipeline(dev);
    assert(ret);
    LOG("Graphics pipeline created successfully");

    ret = createFramebuffers(dev);
    assert(ret);
    LOG("Framebuffers created successfully");

    ret = createSemaphores(dev);
    assert(ret);
    LOG("Semaphores created successfully");

    ret = draw(cmdPool);
    assert(ret);
    //LOG("Issue comman created successfully");

    ret = drawFrame(cmdPool, dev, swapChain);
    assert(ret);
    //LOG("Framebuffers created successfully");
}

