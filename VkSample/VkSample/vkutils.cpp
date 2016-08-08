#include "vkutils.h"

mVkInstance::mVkInstance()
{

}

bool mVkInstance::createInstance()
{
    VkApplicationInfo app;
    {
        app.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app.pNext               = NULL;
        app.pApplicationName    = "Sample";
        app.applicationVersion  = VK_MAKE_VERSION(1, 0, 0);
        app.pEngineName         = "Sample";
        app.engineVersion       = VK_MAKE_VERSION(1, 0, 0);
        app.apiVersion          = VK_API_VERSION_1_0;
    };

    VkInstanceCreateInfo instInfo;
    {
        instInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instInfo.pNext                   = NULL;
        instInfo.pApplicationInfo        = &app;
        instInfo.enabledLayerCount       = 0;
        instInfo.ppEnabledLayerNames     = NULL;
        instInfo.enabledExtensionCount   = 0;
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

bool mVkDevice::enumeratePhysicalDevices(const mVkInstance& inst)
{
    VkResult err = vkEnumeratePhysicalDevices(inst.getInstance(), &_gpuCount, NULL);
    if (err != VK_SUCCESS)
    {
        VK_LOG("vkEnumeratePhysicalDevices error ID %d", err);
    }
    else
    {
        std::vector<VkPhysicalDevice> physicalDevices(_gpuCount);
        err = vkEnumeratePhysicalDevices(inst.getInstance(), &_gpuCount, &physicalDevices[0]);

        _gpu = physicalDevices[0];
    }

    bool ret = getPhysicalDeviceProperties();

    return (err == VK_SUCCESS) && (ret);
}

bool mVkDevice::getPhysicalDeviceProperties()
{
    VkPhysicalDeviceProperties devProperties;
    VkPhysicalDeviceFeatures   devFeatures;

    vkGetPhysicalDeviceProperties(_gpu, &devProperties);
    vkGetPhysicalDeviceFeatures(_gpu, &devFeatures);

    // Reports properties of the queues of the specified physical device.
    vkGetPhysicalDeviceQueueFamilyProperties(_gpu, &_queueFamilyCount, nullptr);
    if (_queueFamilyCount == 0) 
    {
        VK_LOG("Error, no queues present!");
    } 
    else
    {
        std::vector<VkQueueFamilyProperties> queueFamilyProperty(_queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(_gpu, &_queueFamilyCount, &queueFamilyProperty[0]);
        
        for (uint32_t i = 0; i < _queueFamilyCount; ++i) 
        {
            if ((queueFamilyProperty[i].queueCount > 0) &&
                (queueFamilyProperty[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) 
            {
                _queueFamilyIndex = i;
                break;
            }
        }
    }

    return true;
}

bool mVkDevice::createDevice()
{
    std::vector<float> queuePriorities = { 1.0f };
    VkDeviceQueueCreateInfo queueCreateInfo;
    {
        queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext            = NULL;
        queueCreateInfo.flags            = 0;
        queueCreateInfo.queueFamilyIndex = _queueFamilyIndex;
        queueCreateInfo.queueCount       = _queueFamilyCount;
        queueCreateInfo.pQueuePriorities = &queuePriorities[0];
    }

    VkDeviceCreateInfo devCreateInfo;
    {
        devCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        devCreateInfo.pNext                   = NULL;
        devCreateInfo.flags                   = 0;
        devCreateInfo.queueCreateInfoCount    = 1;
        devCreateInfo.pQueueCreateInfos       = &queueCreateInfo;
        devCreateInfo.enabledLayerCount       = 0;
        devCreateInfo.ppEnabledLayerNames     = NULL;
        devCreateInfo.enabledExtensionCount   = 0;
        devCreateInfo.ppEnabledExtensionNames = NULL;
        devCreateInfo.pEnabledFeatures        = NULL;
    }

    if (vkCreateDevice(_gpu, &devCreateInfo, nullptr, &_device) != VK_SUCCESS) {
        VK_LOG("Could not create Vulkan device!");
        return false;
    }

    return true;
}

mVkInstance::~mVkInstance()
{

}
