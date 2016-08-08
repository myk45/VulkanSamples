#include "vkutils.h"


#define VK_GUI_LOG(msg) MessageBox(NULL, (LPCWSTR)L#msg, (LPCWSTR)L#msg, MB_OK);

void vkStuff()
{
    mVkInstance inst;
    mVkDevice dev;
    bool ret = inst.createInstance();

    if (ret) 
    {
        VK_GUI_LOG("Created Instance successfully");

        ret = dev.enumeratePhysicalDevices(inst);
        if (inst.getInstance() == VK_NULL_HANDLE)
        {
            VK_LOG("Error creating obtaining physical device!");
            exit(0);
        }

        VK_GUI_LOG("Device enumeration done successfully");
    }

    if (ret)
    {
        ret = dev.createDevice();
        if (ret)
        {
            VK_GUI_LOG("Vulkan device created");
        }
    }
}
