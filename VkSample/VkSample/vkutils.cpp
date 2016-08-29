#include "vkutils.h"


static const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

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

bool mVkDevice::enumeratePhysicalDevices(const mVkInstance& inst, const glfwWindowHelper& glfwWindow)
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

    bool ret = getPhysicalDeviceProperties(inst, glfwWindow);

    return (err == VK_SUCCESS) && (ret);
}

bool mVkDevice::getPhysicalDeviceProperties(const mVkInstance& inst, const glfwWindowHelper& glfwWindow)
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
            VkBool32 supportsPresent;
            vkGetPhysicalDeviceSurfaceSupportKHR(_gpu, i, glfwWindow.getSurface(), &supportsPresent);

            if (supportsPresent &&
                (queueFamilyProperty[i].queueCount > 0) &&
                (queueFamilyProperty[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) 
            {
                _queueFamilyIndex = i;
                _presentQueueFamilyIndex = i;
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
        devCreateInfo.enabledExtensionCount   = deviceExtensions.size();
        devCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

        VkPhysicalDeviceFeatures features = {};
        features.shaderClipDistance    = VK_TRUE;
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
    vkGetDeviceQueue(_device, _presentQueueFamilyIndex, 0, &_presentQueue);
    
    // @todo: Add error check here!
    return true;
}

mVkSwapChain::mVkSwapChain()
{

}

mVkSwapChain::~mVkSwapChain()
{

}

bool mVkSwapChain::createSwapChain(const mVkDevice& gpu, const glfwWindowHelper& glfwWindow)
{
    std::vector<VkPresentModeKHR>   presentModes;
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu.getPhysicalDevice(), glfwWindow.getSurface(), &capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.getPhysicalDevice(), glfwWindow.getSurface(), &formatCount, nullptr);

        assert(formatCount);
        formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.getPhysicalDevice(), glfwWindow.getSurface(), &formatCount, formats.data());

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu.getPhysicalDevice(), glfwWindow.getSurface(), &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(gpu.getPhysicalDevice(), glfwWindow.getSurface(), &presentModeCount, presentModes.data());
        }
    }

    // Choose the surface format. Default is the first one available.
    VkSurfaceFormatKHR surfaceFormat = formats[0];
    {
        for (const auto& availableFormat : formats) 
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
            {
                surfaceFormat = availableFormat;
                break;
            }
        }
    }

    // Choose the presentation mode. FIFO or mailbox.
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    {
        for (const auto& availablePresentMode : presentModes) 
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
            {
                presentMode = availablePresentMode;
                break;
            }
        }
    }

    VkExtent2D extent;
    {
        VkExtent2D actualExtent = { WIDTH, HEIGHT };
        extent = actualExtent;
    }

    uint32_t imageCount = capabilities.minImageCount + 1;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface          = glfwWindow.getSurface();
    createInfo.minImageCount    = imageCount;
    createInfo.imageFormat      = surfaceFormat.format;
    createInfo.imageColorSpace  = surfaceFormat.colorSpace;
    createInfo.imageExtent      = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = { gpu.getQueueFamilyIndex(), gpu.getPresentFamilyIndex() };

    if (gpu.getQueueFamilyIndex() != gpu.getPresentFamilyIndex()) 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform   = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode    = presentMode;
    createInfo.clipped        = VK_TRUE;
    createInfo.oldSwapchain   = VK_NULL_HANDLE;

    VkResult res = vkCreateSwapchainKHR(gpu.getDevice(), &createInfo, nullptr, &_swapChain);
    assert(res == VK_SUCCESS);

    vkGetSwapchainImagesKHR(gpu.getDevice(), _swapChain, &imageCount, nullptr);
    _swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(gpu.getDevice(), _swapChain, &imageCount, _swapChainImages.data());

    _colorFormat     = surfaceFormat.format;
    _swapChainExtent = extent;

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

    // One command buffer per frame buffer. It's hardcoded now. We can get it from the swapchain interface.
    // TODO: add swapchain interface code here.
    _drawCmdBufs.resize(3);
    for (int i = 0; i < 3; i++) {
        ret = vkAllocateCommandBuffers(gpu.getDevice(), &commandBufferAllocationInfo, &_drawCmdBufs[i]);
        assert(ret == VK_SUCCESS);
    }

    return ret == VK_SUCCESS;
}

bool mVkSwapChain::createSwapChainImageView(const mVkDevice& gpu)
{
    VkResult ret = VK_SUCCESS;
    _swapChainImageViews.resize(_swapChainImages.size());

    for (uint32_t i = 0; i < _swapChainImages.size(); i++) 
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image                           = _swapChainImages[i];
        createInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format                          = _colorFormat;
        createInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel   = 0;
        createInfo.subresourceRange.levelCount     = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount     = 1;

        ret = vkCreateImageView(gpu.getDevice(), &createInfo, nullptr, &_swapChainImageViews[i]);
        assert(ret == VK_SUCCESS);
    }

    return ret == VK_SUCCESS;
}

static std::vector<char> readFile(const std::string& filename) 
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        VK_LOG("failed to open file!");
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
        VK_LOG("failed to create shader module!");
    }
}

mVkInstance::~mVkInstance()
{

}

bool mVkGraphicsPipeline::createGraphicsPipeline(const mVkDevice& gpu, const mVkRenderPass& rp)
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
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = (float)WIDTH;
    viewport.height   = (float)HEIGHT;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkExtent2D rect;
    rect.width        = WIDTH;
    rect.height       = HEIGHT;
    VkRect2D scissor  = {};
    scissor.offset    = { 0, 0 };
    scissor.extent    = rect;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports    = &viewport;
    viewportState.scissorCount  = 1;
    viewportState.pScissors     = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType                    = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable         = VK_FALSE;
    rasterizer.rasterizerDiscardEnable  = VK_FALSE;
    rasterizer.polygonMode              = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth                = 1.0f;
    rasterizer.cullMode                 = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace                = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable          = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable  = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable    = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable     = VK_FALSE;
    colorBlending.logicOp           = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount   = 1;
    colorBlending.pAttachments      = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(gpu.getDevice(), &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) 
    {
        VK_LOG("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType                  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount             = 2;
    pipelineInfo.pStages                = shaderStages;
    pipelineInfo.pVertexInputState      = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState    = &inputAssembly;
    pipelineInfo.pViewportState         = &viewportState;
    pipelineInfo.pRasterizationState    = &rasterizer;
    pipelineInfo.pMultisampleState      = &multisampling;
    pipelineInfo.pColorBlendState       = &colorBlending;
    pipelineInfo.layout                 = _pipelineLayout;
    pipelineInfo.renderPass             = rp.getRenderPass();
    pipelineInfo.subpass                = 0;
    pipelineInfo.basePipelineHandle     = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(gpu.getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS) 
    {
        VK_LOG("failed to create graphics pipeline!");
    }

    return true;
}

bool mVkRenderPass::createRenderPass(const mVkDevice& gpu, const mVkSwapChain& swapChain)
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format         = swapChain.getColorFormat();
    colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment  = 0;
    colorAttachmentRef.layout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subPass   = {};
    subPass.pipelineBindPoint      = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subPass.colorAttachmentCount   = 1;
    subPass.pColorAttachments      = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments    = &colorAttachment;
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subPass;

    if (vkCreateRenderPass(gpu.getDevice() , &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS) {
        VK_LOG("failed to create render pass!");
    }

    return true;
}

bool mVkSwapChain::createFramebuffers(const mVkDevice& gpu, const mVkRenderPass& rp)
{
    VkResult ret = VK_SUCCESS;
    _swapChainFramebuffers.resize(_swapChainImageViews.size());

    // One Frame buffer per swap chain image.
    for (size_t i = 0; i < _swapChainImageViews.size(); i++) 
    {
        VkImageView attachments[] = {
            _swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = rp.getRenderPass();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments    = attachments;
        framebufferInfo.width           = WIDTH;
        framebufferInfo.height          = HEIGHT;
        framebufferInfo.layers          = 1;

        ret = vkCreateFramebuffer(gpu.getDevice(), &framebufferInfo, nullptr, &_swapChainFramebuffers[i]);
        assert(ret == VK_SUCCESS);
    }

    return ret == VK_SUCCESS;
}

bool recordCommands(const mVkCommandPool& cmdPool, mVkSwapChain swapChain, 
                    const mVkGraphicsPipeline& pipe, const mVkRenderPass& rp)
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    for (unsigned int i = 0; i < cmdPool.getDrawCmdBuf().size(); i++)
    {
        vkBeginCommandBuffer(cmdPool.getDrawCmdBuf()[i], &beginInfo);

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass        = rp.getRenderPass();
        renderPassInfo.framebuffer       = swapChain.getFrameBuffers()[i];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = { WIDTH, HEIGHT };

        VkClearValue clearColor = { 0.2f, 0.9f, 0.2f, 1.0f };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        // Issue commands
        {
            vkCmdBeginRenderPass(cmdPool.getDrawCmdBuf()[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(cmdPool.getDrawCmdBuf()[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipe.getGraphicsPipeline());
            vkCmdDraw(cmdPool.getDrawCmdBuf()[i], 5, 2, 0, 0);
            vkCmdEndRenderPass(cmdPool.getDrawCmdBuf()[i]);
        }

        if (vkEndCommandBuffer(cmdPool.getDrawCmdBuf()[i]) != VK_SUCCESS) 
        {
            VK_LOG("failed to record command buffer!");
        }
    }
    
    return true;
}

bool drawFrame(const mVkCommandPool& cmdPool, const mVkDevice& gpu, mVkSwapChain swapChain, const mVkSemaphore& sem)
{
    uint32_t imageIndex;
    vkAcquireNextImageKHR(gpu.getDevice(), swapChain.getSwapChain(), 
                          15000000, sem.getImageAvailableSem(), 
                          VK_NULL_HANDLE, &imageIndex);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { sem.getImageAvailableSem() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdPool.getDrawCmdBuf()[imageIndex];

    VkSemaphore signalSemaphores[] = { sem.getRenderFinishedSem() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(gpu.getDeviceQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) 
    {
        VK_LOG("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapChain.getSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(gpu.getPresentQueue(), &presentInfo);
    
    return true;
}

bool mVkSemaphore::createSemaphore(const mVkDevice& gpu)
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(gpu.getDevice(), &semaphoreInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(gpu.getDevice(), &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS)
    {
        VK_LOG("failed to create semaphores!");
    }

    return true;
}