#include "vkutils.h"

#define LOG(msg) printf("\n[Info] %s", msg);

mVkInstance         inst;
mVkDevice           dev;
mVkSwapChain        swapChain;
mVkCommandPool      cmdPool;
mVkSemaphore        semaphore;
mVkGraphicsPipeline graphicsPipeline;
mVkRenderPass       renderPass;
glfwWindowHelper    glfwHelper;

void initVk()
{
    bool ret = inst.createInstance();
    assert(ret);
    LOG("Created Instance successfully");

    ret = glfwHelper.createWindow(800, 600);
    assert(ret);
    LOG("Window created successfully");

    ret = glfwHelper.createWindowSurface(inst);
    assert(ret);
    LOG("Window surface created successfully");

    ret = dev.enumeratePhysicalDevices(inst, glfwHelper);
    assert(ret);
    LOG("Device enumeration done successfully");

    ret = dev.createDevice();
    assert(ret);
    LOG("Vulkan device created");
    
    ret = dev.createDeviceQueue();
    assert(ret);
    LOG("Device queue created successfully");

    ret = swapChain.createSwapChain(dev, glfwHelper);
    assert(ret);
    LOG("Win32 swapchain created successfully");

    ret = swapChain.createSwapChainImageView(dev);
    assert(ret);
    LOG("Win32 swapchain image view created successfully");

    ret = cmdPool.createCommandPool(dev);
    assert(ret);
    LOG("Command pool created successfully");

    ret = cmdPool.createCommandBuffer(dev);
    assert(ret);
    LOG("2 command buffers created successfully");

    ret = renderPass.createRenderPass(dev);
    assert(ret);
    LOG("Render pass created successfully");

    ret = graphicsPipeline.createGraphicsPipeline(dev, renderPass);
    assert(ret);
    LOG("Graphics pipeline created successfully");

    ret = swapChain.createFramebuffers(dev, renderPass);
    assert(ret);
    LOG("Framebuffers created successfully");

    ret = semaphore.createSemaphore(dev);
    assert(ret);
    LOG("Semaphores created successfully");

    ret = recordCommands(cmdPool, swapChain, graphicsPipeline, renderPass);
    assert(ret);
}

void drawStuff()
{
    bool ret = drawFrame(cmdPool, dev, swapChain, semaphore);
    assert(ret);
}

