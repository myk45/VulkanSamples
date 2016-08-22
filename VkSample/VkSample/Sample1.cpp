#include "vkutils.h"


#define VK_GUI_LOG(msg) MessageBox(NULL, (LPCWSTR)L#msg, (LPCWSTR)L#msg, MB_OK);

void vkStuff()
{
    mVkInstance inst;
    mVkDevice dev;
    mkSwapChain swapChain;

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

}
