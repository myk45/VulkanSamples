#include "vkutils.h"

VkFormat hackFormat;

mVkInstance::mVkInstance()
{

}

bool mVkInstance::createInstance()
{
    const char* extensionNames[] = {
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME
    };

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
        instInfo.enabledExtensionCount   = 2;
        instInfo.ppEnabledExtensionNames = extensionNames;
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

    bool ret = getPhysicalDeviceProperties(inst);

    return (err == VK_SUCCESS) && (ret);
}

bool mVkDevice::getPhysicalDeviceProperties(const mVkInstance& inst)
{
    VkPhysicalDeviceProperties devProperties;
    VkPhysicalDeviceFeatures   devFeatures;

    vkGetPhysicalDeviceProperties(_gpu, &devProperties);
    vkGetPhysicalDeviceFeatures(_gpu, &devFeatures);

    // Get the VK surface that we'd render into. This is some windows specific code.
    {
        extern HINSTANCE hInst;
        extern HWND windowHandle;
        VkResult ret = VK_SUCCESS;

        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.hinstance = hInst;
        surfaceCreateInfo.hwnd      = windowHandle;
        surfaceCreateInfo.pNext     = NULL;
        surfaceCreateInfo.flags     = 0;

        void* ptr = vkGetInstanceProcAddr(inst.getInstance(), "vkCreateWin32SurfaceKHR");
        ret =   vkCreateWin32SurfaceKHR(inst.getInstance(), &surfaceCreateInfo, NULL, &_surface);
        assert(ret == VK_SUCCESS);
    }

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
            VkBool32 supportsPresent;
            vkGetPhysicalDeviceSurfaceSupportKHR(_gpu, i, _surface, &supportsPresent);

            if (supportsPresent &&
                (queueFamilyProperty[i].queueCount > 0) &&
                (queueFamilyProperty[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) 
            {
                _queueFamilyIndex = i;
                //context.presentQueueIdx = j;
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
        const char *deviceExtensions[] = { "VK_KHR_swapchain" };
        devCreateInfo.ppEnabledExtensionNames = deviceExtensions;
        devCreateInfo.enabledExtensionCount = 1;

        VkPhysicalDeviceFeatures features = {};
        features.shaderClipDistance = VK_TRUE;
        devCreateInfo.pEnabledFeatures = &features;
    }

    if (vkCreateDevice(_gpu, &devCreateInfo, nullptr, &_device) != VK_SUCCESS) {
        VK_LOG("Could not create Vulkan device!");
        return false;
    }

    return true;
}

bool mVkDevice::createDeviceQueue()
{
    vkGetDeviceQueue(_device, _queueFamilyIndex, 0, &_deviceQueue);
    
    // @todo: Add error check here!
    return true;
}

mVkSwapChain::mVkSwapChain()
{

}

mVkSwapChain::~mVkSwapChain()
{

}


// taken from https://av.dfki.de/~jhenriques/development.html
bool mVkSwapChain::createSwapChain(const mVkDevice& gpu)
{
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.getPhysicalDevice(), gpu.getSurface(), &formatCount, NULL);
    assert(formatCount);
    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.getPhysicalDevice(), gpu.getSurface(), &formatCount, &surfaceFormats[0]);

    if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
        _colorFormat = VK_FORMAT_B8G8R8_UNORM;
    }
    else {
        _colorFormat = surfaceFormats[0].format;
    }
    VkColorSpaceKHR colorSpace;
    colorSpace = surfaceFormats[0].colorSpace;
    hackFormat = _colorFormat;

    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu.getPhysicalDevice(), gpu.getSurface(), &surfaceCapabilities);

    // we are effectively looking for double-buffering:
    // if surfaceCapabilities.maxImageCount == 0 there is actually no limit on the number of images! 
    uint32_t desiredImageCount = 2;
    if (desiredImageCount < surfaceCapabilities.minImageCount) 
    {
        desiredImageCount = surfaceCapabilities.minImageCount;
    }
    else if (surfaceCapabilities.maxImageCount != 0 &&
        desiredImageCount > surfaceCapabilities.maxImageCount) 
    {
        desiredImageCount = surfaceCapabilities.maxImageCount;
    }

    VkExtent2D surfaceResolution = surfaceCapabilities.currentExtent;
    if (surfaceResolution.width == -1) 
    {
        surfaceResolution.width  = 800;
        surfaceResolution.height = 600;
    }
    else 
    {
        _width  = surfaceResolution.width;
        _height = surfaceResolution.height;
    }

    VkSurfaceTransformFlagBitsKHR preTransform = surfaceCapabilities.currentTransform;
    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) 
    {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu.getPhysicalDevice(), gpu.getSurface(), &presentModeCount, NULL);
    VkPresentModeKHR *presentModes = new VkPresentModeKHR[presentModeCount];
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu.getPhysicalDevice(), gpu.getSurface(), &presentModeCount, presentModes);

    VkPresentModeKHR presentationMode = VK_PRESENT_MODE_FIFO_KHR;   // always supported.
    for (uint32_t i = 0; i < presentModeCount; ++i) 
    {
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentationMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
    }
    delete[] presentModes;

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = gpu.getSurface();
    swapChainCreateInfo.minImageCount = desiredImageCount;
    swapChainCreateInfo.imageFormat = _colorFormat;
    swapChainCreateInfo.imageColorSpace = colorSpace;
    swapChainCreateInfo.imageExtent = surfaceResolution;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;   // <--
    swapChainCreateInfo.preTransform = preTransform;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode = presentationMode;
    swapChainCreateInfo.clipped = true;     // If we want clipping outside the extents
    // (remember our device features?)

    VkResult res = vkCreateSwapchainKHR(gpu.getDevice(), &swapChainCreateInfo, NULL, &_swapChain);

    return res == VK_SUCCESS;
}

bool mVkCommandPool::createCommandPool(const mVkDevice& gpu)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = gpu.getQueueFamilyIndex();

    VkResult ret = vkCreateCommandPool(gpu.getDevice(), &commandPoolCreateInfo, NULL, &_commandPool);

    return ret == VK_SUCCESS;
}

bool mVkCommandPool::createCommandBuffer(const mVkDevice& gpu)
{
    return createInternalCommandBuffer(gpu);
}

bool mVkCommandPool::createInternalCommandBuffer(const mVkDevice& gpu)
{
    VkResult ret = VK_SUCCESS;

    VkCommandBufferAllocateInfo commandBufferAllocationInfo = {};
    commandBufferAllocationInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocationInfo.commandPool        = _commandPool;
    commandBufferAllocationInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocationInfo.commandBufferCount = 1;

    ret = vkAllocateCommandBuffers(gpu.getDevice(), &commandBufferAllocationInfo, &_setupCmdBuf);
    assert(ret == VK_SUCCESS);

    ret = vkAllocateCommandBuffers(gpu.getDevice(), &commandBufferAllocationInfo, &_drawCmdBuf);
    assert(ret == VK_SUCCESS);

    return ret == VK_SUCCESS;
}

bool mVkImageView::createSwapChainImageView(const mVkSwapChain& swapChain, const mVkDevice& gpu)
{
    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(gpu.getDevice(), swapChain.getSwapChain(), &imageCount, NULL);
    assert(imageCount);

    std::vector<VkImage> presentImages(imageCount);
    VkResult ret = vkGetSwapchainImagesKHR(gpu.getDevice(), swapChain.getSwapChain(), &imageCount, &presentImages[0]);

    std::vector<VkImageView> imageViews(imageCount);

    for (uint32_t i = 0; i < presentImages.size(); i++) {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = presentImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChain.getColorFormat();
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(gpu.getDevice(), &createInfo, NULL, &imageViews[i]) != VK_SUCCESS) {
            return false;
        }
    }

    return ret == VK_SUCCESS;
}

static std::vector<char> readFile(const std::string& filename) 
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

static void createShaderModule(const std::vector<char>& code, VkShaderModule& shaderModule, const mVkDevice& gpu)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = (uint32_t*)code.data();

    if (vkCreateShaderModule(gpu.getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
}

mVkInstance::~mVkInstance()
{

}

///-------------------------------------------------------------
VkPipelineLayout pipelineLayout;
VkRenderPass     renderPass;
bool createGraphicsPipeline(const mVkDevice& gpu)
{
    std::vector<char> vertShaderCode = readFile("shaders/vert.spv");
    std::vector<char> fragShaderCode = readFile("shaders/frag.spv");

    VkShaderModule vert;
    VkShaderModule frag;
    createShaderModule(vertShaderCode, vert, gpu);
    createShaderModule(fragShaderCode, frag, gpu);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vert;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = frag;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)800;
    viewport.height = (float)600;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkExtent2D rect;
    rect.width = 800;
    rect.height = 600;
    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = rect;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(gpu.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    return true;
}

bool createRenderPass(const mVkDevice& gpu)
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = hackFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subPass = {};
    subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subPass.colorAttachmentCount = 1;
    subPass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subPass;

    if (vkCreateRenderPass(gpu.getDevice() , &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }

    return true;
}