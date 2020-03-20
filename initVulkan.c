#include "initVulkan.h"

int initVulkan(VkInstance *vkInstance, VkDevice *device, VkSurfaceKHR *surface, GLFWwindow *window,
               VkSwapchainKHR *swapChain, VkImageView **imageViews, uint32_t *amountImages)
{
    // VkApplicationInfo
    VkApplicationInfo appInfo;
    createAppInfo(&appInfo);

    // VkInstanceCreateInfo
    uint32_t  amountOfLayers;
    vkEnumerateInstanceLayerProperties(&amountOfLayers, NULL);
    VkLayerProperties layers[amountOfLayers];
    vkEnumerateInstanceLayerProperties(&amountOfLayers, layers);

    VkInstanceCreateInfo instanceInfo;
    createInstanceInfo(&appInfo, &instanceInfo);

    // Vulkan Instance
    ASSERT_VK_SUCCESS(vkCreateInstance(&instanceInfo, NULL, vkInstance))

    // Get physical device
    uint32_t amountOfPhysicalDevices = 0;
    ASSERT_VK_SUCCESS(vkEnumeratePhysicalDevices(*vkInstance, &amountOfPhysicalDevices, NULL)) // Let fill amount first automatically
    VkPhysicalDevice physicalDevices[amountOfPhysicalDevices]; // create array for physical devices
    ASSERT_VK_SUCCESS(vkEnumeratePhysicalDevices(*vkInstance, &amountOfPhysicalDevices, physicalDevices)) // Call again with array

    // Create Window Surface
    ASSERT_VK_SUCCESS(glfwCreateWindowSurface(*vkInstance, window, NULL, surface))

    printStats(&physicalDevices[0], surface);

    // Queue info
    VkDeviceQueueCreateInfo queueInfo;
    createQueueInfo(&queueInfo);

    // Device info
    VkPhysicalDeviceFeatures usedFeatures = {};
    VkDeviceCreateInfo deviceInfo;
    createDeviceInfo(&queueInfo, &deviceInfo, &usedFeatures);

    // Logical device
    ASSERT_VK_SUCCESS(vkCreateDevice(physicalDevices[0], &deviceInfo, NULL, device))

    // Queue
    VkQueue queue;
    vkGetDeviceQueue(*device, 0, 0, &queue);

    // Swap chain support
    VkBool32 swapChainSupport;
    ASSERT_VK_SUCCESS(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[0], 0, *surface, &swapChainSupport))
    if (!swapChainSupport)
    {
        printf("Swap chain not supported!");
        return FAILURE;
    }

    // Swap chain info
    VkSwapchainCreateInfoKHR swapChainCreateInfo;
    createSwapChainInfo(&swapChainCreateInfo, surface);

    // Swap chain
    ASSERT_VK_SUCCESS(vkCreateSwapchainKHR(*device, &swapChainCreateInfo, NULL, swapChain))

    // Swap chain images
    vkGetSwapchainImagesKHR(*device, *swapChain, amountImages, NULL);
    VkImage swapChainImages[*amountImages];
    ASSERT_VK_SUCCESS(vkGetSwapchainImagesKHR(*device, *swapChain, amountImages, swapChainImages))

    // Image view
    *imageViews = malloc(*amountImages * sizeof(VkImageView));
    VkImageViewCreateInfo imageViewInfo;
    for (int i = 0; i < *amountImages; i++)
    {
        createImageViewInfo(&imageViewInfo, swapChainImages, i);
        ASSERT_VK_SUCCESS(vkCreateImageView(*device, &imageViewInfo, NULL, &*imageViews[i]))
    }

    return SUCCESS;
}

void createAppInfo(VkApplicationInfo *appInfo)
{
    appInfo->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo->pNext = NULL;
    appInfo->pApplicationName = APP_NAME;
    appInfo->applicationVersion = APP_VERSION;
    appInfo->pEngineName = ENGINE_NAME;
    appInfo->engineVersion = ENGINE_VERSION;
    appInfo->apiVersion = VK_API_VERSION_1_1;
}

void createInstanceInfo(VkApplicationInfo *appInfo, VkInstanceCreateInfo *instanceInfo)
{
    GLuint amountOfGLFWExtensions;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&amountOfGLFWExtensions);

    instanceInfo->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo->pNext = NULL;
    instanceInfo->flags = 0;
    instanceInfo->pApplicationInfo = appInfo;
    instanceInfo->enabledLayerCount = 0;
    instanceInfo->ppEnabledLayerNames = NULL;
    instanceInfo->enabledExtensionCount = amountOfGLFWExtensions;
    instanceInfo->ppEnabledExtensionNames = glfwExtensions;
}

void createQueueInfo(VkDeviceQueueCreateInfo *queueInfo)
{
    queueInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo->pNext = NULL;
    queueInfo->flags = 0;
    queueInfo->queueFamilyIndex = 0;
    queueInfo->queueCount = 1;
    queueInfo->pQueuePriorities = NULL;
}

void createDeviceInfo(VkDeviceQueueCreateInfo *queueInfo, VkDeviceCreateInfo *deviceInfo, VkPhysicalDeviceFeatures *features)
{
    const char *deviceExtensions[1] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    deviceInfo->sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo->pNext = NULL;
    deviceInfo->flags = 0;
    deviceInfo->queueCreateInfoCount = 1;
    deviceInfo->pQueueCreateInfos = queueInfo;
    deviceInfo->enabledLayerCount = 0;
    deviceInfo->ppEnabledLayerNames = NULL;
    deviceInfo->enabledExtensionCount = 1;
    deviceInfo->ppEnabledExtensionNames = deviceExtensions;
    deviceInfo->pEnabledFeatures = features;
}

void createSwapChainInfo(VkSwapchainCreateInfoKHR *swapChainCreateInfo, VkSurfaceKHR *surface)
{
    VkExtent2D imageExtent = { WIDTH, HEIGHT };

    swapChainCreateInfo->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo->pNext = NULL;
    swapChainCreateInfo->flags = 0;
    swapChainCreateInfo->surface = *surface;
    swapChainCreateInfo->minImageCount = 1;
    swapChainCreateInfo->imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    swapChainCreateInfo->imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapChainCreateInfo->imageExtent = imageExtent;
    swapChainCreateInfo->imageArrayLayers = 1;
    swapChainCreateInfo->imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo->imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo->queueFamilyIndexCount = 0;
    swapChainCreateInfo->pQueueFamilyIndices = NULL;
    swapChainCreateInfo->preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapChainCreateInfo->compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo->presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapChainCreateInfo->clipped = VK_FALSE;
    swapChainCreateInfo->oldSwapchain = VK_NULL_HANDLE;
}

void createImageViewInfo(VkImageViewCreateInfo *imageViewInfo, VkImage *swapChainImages, int index)
{
    VkComponentMapping componentMapping = {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
    };
    VkImageSubresourceRange subresourceRange = {
            VK_IMAGE_ASPECT_COLOR_BIT,0, 1, 0, 1
    };

    imageViewInfo->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo->pNext = NULL;
    imageViewInfo->flags = 0;
    imageViewInfo->image = swapChainImages[index];
    imageViewInfo->viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo->format = VK_FORMAT_B8G8R8A8_UNORM;
    imageViewInfo->components = componentMapping;
    imageViewInfo->subresourceRange = subresourceRange;
}

void createAttachmentDescription(VkAttachmentDescription *attachmentDescription)
{
    attachmentDescription->flags = 0;
    attachmentDescription->format = VK_FORMAT_B8G8R8A8_UNORM;
    attachmentDescription->samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription->loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}

void createAttachmentReference(VkAttachmentReference *attachmentReference, uint32_t attachment)
{
    attachmentReference->attachment = attachment;
    attachmentReference->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

void createSubpassDescription(VkSubpassDescription *subpassDescription, VkPipelineBindPoint bindPoint,
                              VkAttachmentReference *attachmentReference)
{
    subpassDescription->flags = 0;
    subpassDescription->pipelineBindPoint = bindPoint;
    subpassDescription->inputAttachmentCount = 0;
    subpassDescription->pInputAttachments = NULL;
    subpassDescription->colorAttachmentCount = 1;
    subpassDescription->pColorAttachments = attachmentReference;
    subpassDescription->pResolveAttachments = NULL;
    subpassDescription->pDepthStencilAttachment = NULL;
    subpassDescription->preserveAttachmentCount = 0;
    subpassDescription->pPreserveAttachments = NULL;
}

void createRenderPassInfo(VkRenderPassCreateInfo *renderPassInfo, VkAttachmentDescription *attachmentDescriptions, VkSubpassDescription *subpassDescriptions)
{
    renderPassInfo->sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo->pNext = NULL;
    renderPassInfo->flags = 0;
    renderPassInfo->attachmentCount = 1;
    renderPassInfo->pAttachments = attachmentDescriptions;
    renderPassInfo->subpassCount = 1;
    renderPassInfo->pSubpasses = subpassDescriptions;
    renderPassInfo->dependencyCount = 0;
    renderPassInfo->pDependencies = NULL;
}

void createGraphicsPipelineInfo(VkGraphicsPipelineCreateInfo *graphicsPipelineInfo,
                                VkPipelineShaderStageCreateInfo *shaderStages,
                                VkPipelineVertexInputStateCreateInfo *vertexInputState,
                                VkPipelineInputAssemblyStateCreateInfo *inputAssemblyState,
                                VkPipelineViewportStateCreateInfo *viewportState,
                                VkPipelineRasterizationStateCreateInfo *rasterizationState,
                                VkPipelineMultisampleStateCreateInfo *multisampleState,
                                VkPipelineColorBlendStateCreateInfo *colorBlendState, VkPipelineLayout *pipelineLayout,
                                VkRenderPass *renderPass)
{
    graphicsPipelineInfo->sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineInfo->pNext = NULL;
    graphicsPipelineInfo->flags = 0;
    graphicsPipelineInfo->stageCount = 3;
    graphicsPipelineInfo->pStages = shaderStages;
    graphicsPipelineInfo->pVertexInputState = vertexInputState;
    graphicsPipelineInfo->pInputAssemblyState = inputAssemblyState;
    graphicsPipelineInfo->pTessellationState = NULL;
    graphicsPipelineInfo->pViewportState = viewportState;
    graphicsPipelineInfo->pRasterizationState = rasterizationState;
    graphicsPipelineInfo->pMultisampleState = multisampleState;
    graphicsPipelineInfo->pDepthStencilState = NULL;
    graphicsPipelineInfo->pColorBlendState = colorBlendState;
    graphicsPipelineInfo->pDynamicState = NULL;
    graphicsPipelineInfo->layout = *pipelineLayout;
    graphicsPipelineInfo->renderPass = *renderPass;
    graphicsPipelineInfo->subpass = 0;
    graphicsPipelineInfo->basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineInfo->basePipelineIndex = -1;
}

void createFramebufferInfo(VkFramebufferCreateInfo *framebufferInfo, VkRenderPass *renderPass, VkImageView *imageView)
{
    framebufferInfo->sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo->pNext = NULL;
    framebufferInfo->flags = 0;
    framebufferInfo->renderPass = *renderPass;
    framebufferInfo->attachmentCount = 1;
    framebufferInfo->pAttachments = imageView;
    framebufferInfo->width = WIDTH;
    framebufferInfo->height = HEIGHT;
    framebufferInfo->layers = 1;
}

void createCommandPoolInfo(VkCommandPoolCreateInfo *commandPoolInfo, uint32_t queueFamilyIndex)
{
    commandPoolInfo->sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo->pNext = NULL;
    commandPoolInfo->flags = 0;
    commandPoolInfo->queueFamilyIndex = queueFamilyIndex;
}

void createCommandBufferAllocateInfo(VkCommandBufferAllocateInfo *commandBufferAllocateInfo)
{

}

void shutdownVulkan(VkInstance *vkInstance, VkDevice *device, VkSurfaceKHR *surface, VkSwapchainKHR *swapChain,
                    VkImageView *imageViews, uint32_t imageViewsSize, VkShaderModule *modules,
                    uint32_t shaderModulesSize, VkPipelineLayout *pipelineLayouts, uint32_t pipelineLayoutsSize,
                    VkRenderPass *renderPasses, uint32_t renderPassesSize, VkPipeline *pipelines,
                    uint32_t pipelinesSize, VkFramebuffer *framebuffers, VkCommandPool *commandPool)
{
    vkDeviceWaitIdle(*device);

    vkDestroyCommandPool(*device, *commandPool, NULL);

    for (int i = 0; i < imageViewsSize; ++i)
    {
        vkDestroyFramebuffer(*device, framebuffers[i], NULL);
    }

    for (int i = 0; i < pipelinesSize; ++i)
    {
        vkDestroyPipeline(*device, pipelines[i], NULL);
    }

    for (int i = 0; i < renderPassesSize; ++i)
    {
        vkDestroyRenderPass(*device, renderPasses[i], NULL);
    }

    for (int i = 0; i < pipelineLayoutsSize; ++i)
    {
        vkDestroyPipelineLayout(*device, pipelineLayouts[i], NULL);
    }

    for (int i = 0; i < shaderModulesSize; ++i)
    {
        vkDestroyShaderModule(*device, modules[i], NULL);
    }

    for (int i = 0; i < imageViewsSize; i++)
    {
        vkDestroyImageView(*device, imageViews[i], NULL);
    }

    vkDestroySwapchainKHR(*device, *swapChain, NULL);
    vkDestroySurfaceKHR(*vkInstance, *surface, NULL);
    vkDestroyDevice(*device, NULL);
    vkDestroyInstance(*vkInstance, NULL);
}

void shutdownGLFW(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

int createShaderModule(VkDevice device, VkShaderModule *shaderModule, const char *shaderSource, long sourceSize)
{
    VkShaderModuleCreateInfo shaderModuleInfo;
    shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleInfo.pNext = NULL;
    shaderModuleInfo.flags = 0;
    shaderModuleInfo.codeSize = sourceSize;
    shaderModuleInfo.pCode = (uint32_t *) shaderSource;

    ASSERT_VK_SUCCESS(vkCreateShaderModule(device, &shaderModuleInfo, NULL, shaderModule))

    return SUCCESS;
}

void createShaderStageInfo(VkPipelineShaderStageCreateInfo *shaderStageInfo, VkShaderStageFlagBits shaderStageBit, VkShaderModule shaderModule, const char *entryPointName)
{
    shaderStageInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo->pNext = NULL;
    shaderStageInfo->flags = 0;
    shaderStageInfo->stage = shaderStageBit;
    shaderStageInfo->module = shaderModule;
    shaderStageInfo->pName = entryPointName;
    shaderStageInfo->pSpecializationInfo = NULL;
}

void createPipelineVertexInputStateInfo(VkPipelineVertexInputStateCreateInfo *vertexInputStateInfo, VkVertexInputAttributeDescription *attributes, uint32_t atrributesSize)
{
    vertexInputStateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateInfo->pNext = NULL;
    vertexInputStateInfo->flags = 0;
    vertexInputStateInfo->vertexBindingDescriptionCount = 0;
    vertexInputStateInfo->pVertexBindingDescriptions = NULL;
    vertexInputStateInfo->vertexAttributeDescriptionCount = atrributesSize;
    vertexInputStateInfo->pVertexAttributeDescriptions = attributes;
}

void createInputAssemblyStateInfo(VkPipelineInputAssemblyStateCreateInfo *inputAssemblyStateInfo, VkPrimitiveTopology topology)
{
    inputAssemblyStateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateInfo->pNext = NULL;
    inputAssemblyStateInfo->flags = 0;
    inputAssemblyStateInfo->topology = topology;
    inputAssemblyStateInfo->primitiveRestartEnable = VK_FALSE;
}

void createViewportStateInfo(VkPipelineViewportStateCreateInfo *viewportStateInfo, float width, float height)
{
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = { {0, 0}, {width, height} };

    viewportStateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateInfo->pNext = NULL;
    viewportStateInfo->flags = 0;
    viewportStateInfo->viewportCount = 1;
    viewportStateInfo->pViewports = &viewport;
    viewportStateInfo->scissorCount = 1;
    viewportStateInfo->pScissors = &scissor;
}

void createRasterizationStateInfo(VkPipelineRasterizationStateCreateInfo *rasterizationStateInfo, VkPolygonMode polygonMode)
{
    rasterizationStateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateInfo->pNext = NULL;
    rasterizationStateInfo->flags = 0;
    rasterizationStateInfo->depthClampEnable = VK_FALSE;
    rasterizationStateInfo->rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateInfo->polygonMode = polygonMode;
    rasterizationStateInfo->cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateInfo->frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationStateInfo->depthBiasEnable = VK_FALSE;
    rasterizationStateInfo->depthBiasConstantFactor = 0.0f;
    rasterizationStateInfo->depthBiasClamp = 0.0f;
    rasterizationStateInfo->depthBiasSlopeFactor = 0.0f;
    rasterizationStateInfo->lineWidth = 1.0f;
}

void createMultisampleStateInfo(VkPipelineMultisampleStateCreateInfo *multisampleStateInfo)
{
    multisampleStateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateInfo->pNext = NULL;
    multisampleStateInfo->flags = 0;
    multisampleStateInfo->rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleStateInfo->sampleShadingEnable = VK_FALSE;
    multisampleStateInfo->minSampleShading = 1.0f;
    multisampleStateInfo->pSampleMask = NULL;
    multisampleStateInfo->alphaToCoverageEnable = VK_FALSE;
    multisampleStateInfo->alphaToOneEnable = VK_FALSE;
}

void createColorBlendAttachmentStateInfo(VkPipelineColorBlendAttachmentState  *colorBlendAttachmentState)
{
    colorBlendAttachmentState->blendEnable = VK_FALSE;
    colorBlendAttachmentState->srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachmentState->dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachmentState->colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState->alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
}

void createColorBlendStateInfo(VkPipelineColorBlendStateCreateInfo  *colorBlendStateInfo, VkPipelineColorBlendAttachmentState  *blendAttachments, uint32_t blendAttachmentsSize)
{
    colorBlendStateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateInfo->pNext = NULL;
    colorBlendStateInfo->flags = 0;
    colorBlendStateInfo->logicOpEnable = VK_FALSE;
    colorBlendStateInfo->logicOp = VK_LOGIC_OP_NO_OP;
    colorBlendStateInfo->attachmentCount = blendAttachmentsSize;
    colorBlendStateInfo->pAttachments = blendAttachments;
    colorBlendStateInfo->blendConstants[0] = 0.0f;
    colorBlendStateInfo->blendConstants[1] = 0.0f;
    colorBlendStateInfo->blendConstants[2] = 0.0f;
    colorBlendStateInfo->blendConstants[3] = 0.0f;
}

void createLayoutInfo(VkPipelineLayoutCreateInfo  *layoutInfo, VkDescriptorSetLayout *setLayouts, uint32_t setLayoutSize)
{
    layoutInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo->pNext = NULL;
    layoutInfo->flags = 0;
    layoutInfo->setLayoutCount = setLayoutSize;
    layoutInfo->pSetLayouts = setLayouts;
    layoutInfo->pushConstantRangeCount = 0;
    layoutInfo->pPushConstantRanges = NULL;
}

void printStats(VkPhysicalDevice *physicalDevice, VkSurfaceKHR *surface)
{
    // Device Properties
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(*physicalDevice, &properties);

    printf("--------- DEVICE PROPERTIES ---------\n");
    uint32_t apiVersion = properties.apiVersion;
    printf("Name:                   %s\n", properties.deviceName);
    printf("API Version:            %d.%d.%d\n", VK_VERSION_MAJOR(apiVersion), VK_VERSION_MINOR(apiVersion), VK_VERSION_PATCH(apiVersion));

    // Features
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(*physicalDevice, &features);

    printf("\n--------- FEATURES ---------\n");
    printf("Geometry Shader:        %s\n", (BOOL_LITERAL(features.geometryShader)));
    printf("Tessellation Shader:    %s\n", (BOOL_LITERAL(features.tessellationShader)));

    // Memory Properties
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(*physicalDevice, &memoryProperties);

    printf("\n--------- MEMORY PROPERTIES ---------\n");
    printf("Heapsize:               %llu Byte / %f GiB\n", memoryProperties.memoryHeaps->size,
           (HUMAN_READABLE(memoryProperties.memoryHeaps->size)));

    // Queue Properties
    uint32_t amountQueueFamilies = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &amountQueueFamilies, NULL);
    VkQueueFamilyProperties familyProperties[amountQueueFamilies];
    vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &amountQueueFamilies, familyProperties);

    printf("\n--------- QUEUE PROPERTIES ---------\n");
    printf("Queue Families Amount:  %d\n", amountQueueFamilies);

    for (int i = 0; i < amountQueueFamilies; i++)
    {
        printf("-- Queue Family #%d --\n", i);
        printf("   Graphics bit:       %s\n", (BOOL_LITERAL((familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)));
        printf("   Compute bit:        %s\n", (BOOL_LITERAL((familyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)));
        printf("   Transfer bit:       %s\n", (BOOL_LITERAL((familyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)));
        printf("   Sparse Binding bit: %s\n", (BOOL_LITERAL((familyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0)));
        printf("   Queue Count:        %d\n", familyProperties[i].queueCount);
    }

    // Surface Capabilities
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*physicalDevice, *surface, &surfaceCapabilities);
    printf("\n--------- SURFACE CAPABILITIES ---------\n");
    printf("    Min Image Count:           %d\n", surfaceCapabilities.minImageCount);
    printf("    Max Image Count:           %d\n", surfaceCapabilities.maxImageCount);
    printf("    Current Extent:           (%d; %d)\n", surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);
    printf("    Min Image Extent:         (%d; %d)\n", surfaceCapabilities.minImageExtent.width, surfaceCapabilities.minImageExtent.height);
    printf("    Max Image Extent:         (%d; %d)\n", surfaceCapabilities.maxImageExtent.width, surfaceCapabilities.maxImageExtent.height);
    printf("    Max Image Array Layers:    %d\n", surfaceCapabilities.maxImageArrayLayers);
    printf("    Supported Transforms:      %d\n", surfaceCapabilities.supportedTransforms);
    printf("    Current Transform:         %d\n", surfaceCapabilities.currentTransform);
    printf("    Supported Composite Alpha: %d\n", surfaceCapabilities.supportedCompositeAlpha);
    printf("    Supported Usage Flags:     %d\n", surfaceCapabilities.supportedUsageFlags);

    // Surface Formats
    uint32_t amountFormats;
    vkGetPhysicalDeviceSurfaceFormatsKHR(*physicalDevice, *surface, &amountFormats, NULL);
    VkSurfaceFormatKHR formats[amountFormats];
    vkGetPhysicalDeviceSurfaceFormatsKHR(*physicalDevice, *surface, &amountFormats, formats);

    printf("\n--------- SURFACE FORMATS ---------\n");
    printf("Surface Formats Amount:  %d\n", amountFormats);

    for (int i = 0; i < amountFormats; i++)
    {
        printf("--  Surface Format #%d --\n", i);
        printf("    Format: %d\n", formats[i].format);
    }

    // Presentation Modes
    uint32_t amountPresentModes;
    vkGetPhysicalDeviceSurfacePresentModesKHR(*physicalDevice, *surface, &amountPresentModes, NULL);
    VkPresentModeKHR presentModes[amountPresentModes];
    vkGetPhysicalDeviceSurfacePresentModesKHR(*physicalDevice, *surface, &amountPresentModes, presentModes);

    printf("\n--------- PRESENTATION MODES ---------\n");
    printf("Presentation Modes Amount:  %d\n", amountPresentModes);

    for (int i = 0; i < amountPresentModes; ++i)
    {
        printf("--  Present Mode #%d --\n", i);
        printf("    Mode: %d\n", presentModes[i]);
    }
}