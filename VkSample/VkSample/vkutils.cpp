#include "vkutils.h"

mVkInstance::mVkInstance()
{

}

bool mVkInstance::createInstance()
{
    VkApplicationInfo app;
    {
        app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app.pNext = NULL;
        app.pApplicationName = "Sample";
        app.applicationVersion = 0;
        app.pEngineName = "Sample";
        app.engineVersion = 0;
        app.apiVersion = VK_API_VERSION_1_0;
    };

    VkInstanceCreateInfo instInfo;
    {
        instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instInfo.pNext = NULL;
        instInfo.pApplicationInfo = &app;
        instInfo.enabledLayerCount = 0;
        instInfo.ppEnabledLayerNames = NULL;
        instInfo.enabledExtensionCount = 0;
        instInfo.ppEnabledExtensionNames = NULL;
   }

    VkResult err = vkCreateInstance(&instInfo, NULL, &_inst);
    if (err != VK_SUCCESS) 
    {
        VK_LOG("vkCreateInstance error ID %d", err);
    }
    else
    {
        VK_LOG("vkInstance created : [%p]", _inst);
    }

    return err == VK_SUCCESS;
}

mVkInstance::~mVkInstance()
{

}
