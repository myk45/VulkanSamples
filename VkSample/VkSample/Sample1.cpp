#include "vkutils.h"


#define VK_GUI_LOG(msg) MessageBox(NULL, (LPCWSTR)L#msg, (LPCWSTR)L#msg, MB_OK);

void vkStuff()
{
    mVkInstance inst;
    mVkDevice dev;
    mVkSwapChain swapChain;
    mVkCommandPool cmdPool;
    mVkImageView   swapChainImageView;

    bool ret = inst.createInstance();
    assert(ret);
    VK_GUI_LOG("Created Instance successfully");

    ret = dev.enumeratePhysicalDevices(inst);
    assert(ret);
    VK_GUI_LOG("Device enumeration done successfully");

    ret = dev.createDevice();
    assert(ret);
    VK_GUI_LOG("Vulkan device created");
    
    ret = dev.createDeviceQueue();
    assert(ret);
    VK_GUI_LOG("Device queue created successfully");

    ret = swapChain.createSwapChain(dev);
    assert(ret);
    VK_GUI_LOG("Win32 swapchain created successfully");

    ret = cmdPool.createCommandPool(dev);
    assert(ret);
    VK_GUI_LOG("Command pool created successfully");

    ret = cmdPool.createCommandBuffer(dev);
    assert(ret);
    VK_GUI_LOG("2 command buffers created successfully");

    ret = swapChainImageView.createSwapChainImageView(swapChain, dev);
    assert(ret);
    VK_GUI_LOG("Swap chain image view created successfully");

    ret = createRenderPass(dev);
    assert(ret);
    VK_GUI_LOG("Render pass created successfully");

    ret = createGraphicsPipeline(dev);
    assert(ret);
    VK_GUI_LOG("Graphics pipeline created successfully");

    ret = createFramebuffers(dev);
    assert(ret);
    VK_GUI_LOG("Framebuffers created successfully");

    ret = createSemaphores(dev);
    assert(ret);
    VK_GUI_LOG("Semaphores created successfully");

    ret = draw(cmdPool);
    assert(ret);
    //VK_GUI_LOG("Issue comman created successfully");

    ret = drawFrame(cmdPool, dev, swapChain);
    assert(ret);
    //VK_GUI_LOG("Framebuffers created successfully");
}

