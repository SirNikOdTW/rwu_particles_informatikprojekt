#include "initVulkan.h"
#include <string.h>

void shutdownGLFW(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void createInstance(Compute *compute, Graphics *graphics)
{
    GLuint enabledLayerSize = 0;
    const char **enabledLayer;
    GLuint extensionsSize;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionsSize);

    #ifndef NDEBUG
        enabledLayerSize = 1;
        enabledLayer = malloc(sizeof(char *));
        enabledLayer[0] = "VK_LAYER_LUNARG_standard_validation";
    #endif

    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = APP_NAME;
    applicationInfo.applicationVersion = APP_VERSION;
    applicationInfo.pEngineName = ENGINE_NAME;
    applicationInfo.engineVersion = ENGINE_VERSION;
    applicationInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.flags = 0;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledLayerCount = enabledLayerSize;
    instanceCreateInfo.ppEnabledLayerNames = enabledLayer;
    instanceCreateInfo.enabledExtensionCount = extensionsSize;
    instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;

    ASSERT_VK(vkCreateInstance(&instanceCreateInfo, NULL, &(compute->instance)))

    graphics->instance = compute->instance;
}

void findPhysicalDevice(Compute *compute, Graphics *graphics)
{
    uint32_t devicesSize;
    vkEnumeratePhysicalDevices(compute->instance, &devicesSize, NULL);

    if (devicesSize <= 0)
    {
        printf("Fatal : No device found with Vulkan support!");
        assert(devicesSize > 0);
    }

    VkPhysicalDevice physicalDevices[devicesSize];
    vkEnumeratePhysicalDevices(compute->instance, &devicesSize, physicalDevices);

    for (int i = 0; i < devicesSize; ++i)
    {
        VkPhysicalDeviceProperties deviceProperties = {};
        vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);

        if (WORKGROUP_SIZE_X <= deviceProperties.limits.maxComputeWorkGroupSize[0]
            && PARTICLE_AMOUNT / WORKGROUP_SIZE_X <= deviceProperties.limits.maxComputeWorkGroupCount[0]
            && (uint32_t) (PARTICLE_SIZE * PARTICLE_AMOUNT) <= deviceProperties.limits.maxStorageBufferRange)
        {
            compute->physicalDevice = physicalDevices[i];
            break;
        }
    }

    if (!compute->physicalDevice)
    {
        printf("Fatal : No device found with capable limits!");
        assert(compute->physicalDevice);
    }

    graphics->physicalDevice = compute->physicalDevice;
}

uint32_t getQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkQueueFlagBits queueFlagBits, int noGraphics)
{
    uint32_t queueFamiliesSize;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesSize, NULL);
    VkQueueFamilyProperties queueFamilies[queueFamiliesSize];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesSize, queueFamilies);

    for (uint32_t i = 0; i < queueFamiliesSize; ++i)
    {
        if (noGraphics && (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
        {
        }
        else if ((queueFamilies[i].queueFlags & queueFlagBits) == queueFlagBits)
        {
            return i;
        }
    }

    return UINT32_MAX;
}

void createDevice(Compute *compute, Graphics *graphics)
{
    compute->queueFamilyIndex = getQueueFamilyIndex(compute->physicalDevice, VK_QUEUE_COMPUTE_BIT, 1);
    if (compute->queueFamilyIndex == UINT32_MAX)
    {
        printf("ERROR: No Queue family found with desired capabilities!\n");
        assert( compute->queueFamilyIndex < UINT32_MAX);
    }

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo computeDeviceQueueCreateInfo = {};
    computeDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    computeDeviceQueueCreateInfo.queueFamilyIndex = compute->queueFamilyIndex;
    computeDeviceQueueCreateInfo.queueCount = 1;
    computeDeviceQueueCreateInfo.pQueuePriorities = &queuePriority;

    graphics->queueFamilyIndex = getQueueFamilyIndex(graphics->physicalDevice, VK_QUEUE_GRAPHICS_BIT, 0);
    if (graphics->queueFamilyIndex == UINT32_MAX)
    {
        printf("ERROR: No Queue family found with desired capabilities!\n");
        assert( compute->queueFamilyIndex < UINT32_MAX);
    }

    VkDeviceQueueCreateInfo graphicsDeviceQueueCreateInfo = {};
    graphicsDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphicsDeviceQueueCreateInfo.queueFamilyIndex = graphics->queueFamilyIndex;
    graphicsDeviceQueueCreateInfo.queueCount = 1;
    graphicsDeviceQueueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceQueueCreateInfo deviceQueueCreateInfos[2] = {
            computeDeviceQueueCreateInfo,
            graphicsDeviceQueueCreateInfo
    };

    VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
    const char *swapchainExtension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    const char **extensions = &swapchainExtension;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 2;
    deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos;
    deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
    deviceCreateInfo.enabledExtensionCount = 1;
    deviceCreateInfo.ppEnabledExtensionNames = extensions;

    ASSERT_VK(vkCreateDevice(compute->physicalDevice, &deviceCreateInfo, NULL, &(compute->device)))

    graphics->device = compute->device;

    vkGetDeviceQueue(compute->device, compute->queueFamilyIndex, 0, &(compute->queue));
    vkGetDeviceQueue(graphics->device, graphics->queueFamilyIndex, 0, &(graphics->queue));
}

uint32_t findMemoryType(Compute *compute, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(compute->physicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((memoryTypeBits & (1u << i))
            && ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
            return i;
    }

    return UINT32_MAX;
}

void createParticleBuffer(Compute *compute, Graphics *graphics)
{
    uint32_t queueFamilyIndices[2] = {
            compute->queueFamilyIndex,
            graphics->queueFamilyIndex
    };

    VkBufferCreateInfo particleBufferCreateInfo = {};
    particleBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    particleBufferCreateInfo.size = compute->particleBufferSize;
    particleBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    particleBufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    particleBufferCreateInfo.queueFamilyIndexCount = 2;
    particleBufferCreateInfo.pQueueFamilyIndices = queueFamilyIndices;

    ASSERT_VK(vkCreateBuffer(compute->device, &particleBufferCreateInfo, NULL, &(compute->particleBuffer)))

    VkMemoryRequirements particleBufferMemoryRequirements;
    vkGetBufferMemoryRequirements(compute->device, compute->particleBuffer, &particleBufferMemoryRequirements);

    VkMemoryAllocateInfo particleMemoryAllocateInfo = {};
    particleMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    particleMemoryAllocateInfo.allocationSize = particleBufferMemoryRequirements.size;
    particleMemoryAllocateInfo.memoryTypeIndex = findMemoryType(compute,
                                                                particleBufferMemoryRequirements.memoryTypeBits,
                                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    ASSERT_VK(vkAllocateMemory(compute->device, &particleMemoryAllocateInfo, NULL, &(compute->particleBufferMemory)))

    ASSERT_VK(vkBindBufferMemory(compute->device, compute->particleBuffer, compute->particleBufferMemory, 0))
}

void createComputeBuffer(Compute *compute, uint32_t bufferSize, VkBufferUsageFlags usageFlags, VkBuffer *buffer,
                         VkDeviceMemory *bufferMemory, VkMemoryPropertyFlags memoryPropertyFlags)
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = bufferSize;
    bufferCreateInfo.usage = usageFlags;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    ASSERT_VK(vkCreateBuffer(compute->device, &bufferCreateInfo, NULL, buffer))

    VkMemoryRequirements bufferMemoryRequirements;
    vkGetBufferMemoryRequirements(compute->device, *buffer, &bufferMemoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = bufferMemoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = findMemoryType(compute,
                                                        bufferMemoryRequirements.memoryTypeBits,
                                                        memoryPropertyFlags);

    ASSERT_VK(vkAllocateMemory(compute->device, &memoryAllocateInfo, NULL, bufferMemory))

    ASSERT_VK(vkBindBufferMemory(compute->device, *buffer, *bufferMemory, 0))
}

void createComputeBuffers(Compute *compute)
{
    // dt Uniform Buffer
    createComputeBuffer(compute, compute->dtUniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        &(compute->dtUniformBuffer),
                        &(compute->dtUniformBufferMemory),
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // staticIn Uniform Buffer
    createComputeBuffer(compute, compute->staticInUniformBufferSize,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        &(compute->staticInUniformBuffer),
                        &(compute->staticInUniformBufferMemory), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

}

void createComputeDescriptorSetLayout(Compute *compute, VkDescriptorType descriptorType, VkDescriptorSetLayout *descriptorSetLayout)
{
    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
    descriptorSetLayoutBinding.binding = 0;
    descriptorSetLayoutBinding.descriptorType = descriptorType;
    descriptorSetLayoutBinding.descriptorCount = 1;
    descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = 1;
    descriptorSetLayoutCreateInfo.pBindings = &descriptorSetLayoutBinding;

    ASSERT_VK(vkCreateDescriptorSetLayout(compute->device, &descriptorSetLayoutCreateInfo, NULL, descriptorSetLayout))
}

void createComputeDescriptorSetLayouts(Compute *compute)
{
    // Particle Buffer
    createComputeDescriptorSetLayout(compute, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                     &(compute->particleBufferDescriptorSetLayout));

    // dt Uniform Buffer
    createComputeDescriptorSetLayout(compute, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                     &(compute->dtUniformBufferDescriptorSetLayout));

    // staticIn Uniform Buffer
    createComputeDescriptorSetLayout(compute, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                     &(compute->staticInUniformBufferDescriptorSetLayout));
}

void createComputeDescriptorSet(Compute *compute, VkDescriptorType descriptorType, VkDescriptorPool *descriptorPool,
                                VkDescriptorSetLayout *descriptorSetLayout, VkDescriptorSet *descriptorSet,
                                VkBuffer buffer, uint32_t bufferSize)
{
    VkDescriptorPoolSize descriptorPoolSize = {};
    descriptorPoolSize.type = descriptorType;
    descriptorPoolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.maxSets = 1;
    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;

    ASSERT_VK(vkCreateDescriptorPool(compute->device, &descriptorPoolCreateInfo, NULL, descriptorPool))

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = *descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayout;

    ASSERT_VK(vkAllocateDescriptorSets(compute->device, &descriptorSetAllocateInfo, descriptorSet))

    VkDescriptorBufferInfo descriptorBufferInfo = {};
    descriptorBufferInfo.buffer = buffer;
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.range = bufferSize;

    VkWriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = *descriptorSet;
    writeDescriptorSet.dstBinding = 0;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType = descriptorType;
    writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;

    vkUpdateDescriptorSets(compute->device, 1, &writeDescriptorSet, 0, NULL);
}

void createComputeDescriptorSets(Compute *compute)
{
    // Particle Buffer
    createComputeDescriptorSet(compute, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &(compute->particleBufferDescriptorPool),
                               &(compute->particleBufferDescriptorSetLayout), &(compute->particleBufferDescriptorSet),
                               compute->particleBuffer, compute->particleBufferSize);

    // dt Uniform Buffer
    createComputeDescriptorSet(compute, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &(compute->dtUniformBufferDescriptorPool),
                               &(compute->dtUniformBufferDescriptorSetLayout), &(compute->dtUniformBufferDescriptorSet),
                               compute->dtUniformBuffer, compute->dtUniformBufferSize);

    // staticIn Uniform Buffer
    createComputeDescriptorSet(compute, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                               &(compute->staticInUniformBufferDescriptorPool),
                               &(compute->staticInUniformBufferDescriptorSetLayout),
                               &(compute->staticInUniformBufferDescriptorSet),
                               compute->staticInUniformBuffer, compute->staticInUniformBufferSize);
}

void createShaderModule(VkDevice device, char *filename, VkShaderModule *shaderModule)
{
    long shaderSourceSize;
    char *shaderSource = readFile(filename, "rb", &shaderSourceSize);

    VkShaderModuleCreateInfo shaderModuleInfo;
    shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleInfo.pNext = NULL;
    shaderModuleInfo.flags = 0;
    shaderModuleInfo.codeSize = shaderSourceSize;
    shaderModuleInfo.pCode = (uint32_t *) shaderSource;

    ASSERT_VK(vkCreateShaderModule(device, &shaderModuleInfo, NULL, shaderModule))

    free(shaderSource);
}

void createComputePipeline(Compute *compute)
{
    createShaderModule(compute->device, "./vulkan/comp.spv", &(compute->shaderModule));

    VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo = {};
    pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineShaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipelineShaderStageCreateInfo.module = compute->shaderModule;
    pipelineShaderStageCreateInfo.pName = "main";

    VkDescriptorSetLayout descriptorSetLayouts[] = {
            compute->particleBufferDescriptorSetLayout,
            compute->dtUniformBufferDescriptorSetLayout,
            compute->staticInUniformBufferDescriptorSetLayout
    };
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 3;
    pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts;

    ASSERT_VK(vkCreatePipelineLayout(compute->device, &pipelineLayoutCreateInfo, NULL, &(compute->pipelineLayout)))

    VkComputePipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stage = pipelineShaderStageCreateInfo;
    pipelineCreateInfo.layout = compute->pipelineLayout;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    ASSERT_VK(vkCreateComputePipelines(compute->device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, NULL, &(compute->pipeline)))
}

void mapBufferMemory(Compute *compute, VkDeviceMemory memory, void *inputData, uint32_t dataSize)
{
    void *vkData;
    vkMapMemory(compute->device, memory, 0, dataSize, 0, &vkData);
    memcpy(vkData, inputData, dataSize);
    vkUnmapMemory(compute->device, memory);
}

void copyBuffer(Compute  *compute, VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
    uint32_t queueFamilyIndex = getQueueFamilyIndex(compute->physicalDevice, VK_QUEUE_TRANSFER_BIT, 0);

    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = 0;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

    VkCommandPool stagingCommandPool;
    ASSERT_VK(vkCreateCommandPool(compute->device, &commandPoolCreateInfo, NULL, &stagingCommandPool))

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = stagingCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer stagingCommandBuffer;
    ASSERT_VK(vkAllocateCommandBuffers(compute->device, &allocInfo, &stagingCommandBuffer))

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    ASSERT_VK(vkBeginCommandBuffer(stagingCommandBuffer, &beginInfo))

    VkBufferCopy copyRegion = { .size = size };
    vkCmdCopyBuffer(stagingCommandBuffer, src, dst, 1, &copyRegion);

    ASSERT_VK(vkEndCommandBuffer(stagingCommandBuffer))

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &stagingCommandBuffer;

    VkQueue stagingQueue;
    vkGetDeviceQueue(compute->device, queueFamilyIndex, 0, &stagingQueue);

    vkQueueSubmit(stagingQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(stagingQueue);

    vkFreeCommandBuffers(compute->device, stagingCommandPool, 1, &stagingCommandBuffer);
}

void fillComputeBuffer(Compute *compute, VkBuffer dst, void *data, uint32_t dataSize)
{
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBuggerMemory;
    createComputeBuffer(compute, compute->particleBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &stagingBuffer, &stagingBuggerMemory,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    mapBufferMemory(compute, stagingBuggerMemory, data, dataSize);

    copyBuffer(compute, stagingBuffer, dst, dataSize);
}

void fillComputeBuffers(Compute *compute, float *particles, Dt *dtData, StaticIn *staticInData)
{
    // Particle Buffer
    fillComputeBuffer(compute, compute->particleBuffer, particles, compute->particleBufferSize);

    // dt Buffer
    mapBufferMemory(compute, compute->dtUniformBufferMemory, dtData, compute->dtUniformBufferSize);

    // staticIn Buffer
    fillComputeBuffer(compute, compute->staticInUniformBuffer, staticInData, compute->staticInUniformBufferSize);
}

void createComputeCommandBuffer(Compute *compute, Graphics *graphics)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = 0;
    commandPoolCreateInfo.queueFamilyIndex = compute->queueFamilyIndex;

    ASSERT_VK(vkCreateCommandPool(compute->device, &commandPoolCreateInfo, NULL, &(compute->commandPool)))

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = compute->commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    ASSERT_VK(vkAllocateCommandBuffers(compute->device, &commandBufferAllocateInfo, &(compute->commandBuffer)))

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    ASSERT_VK(vkBeginCommandBuffer(compute->commandBuffer, &beginInfo))

    vkCmdBindPipeline(compute->commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, compute->pipeline);
    VkDescriptorSet descriptorSets[] = {
            compute->particleBufferDescriptorSet,
            compute->dtUniformBufferDescriptorSet,
            compute->staticInUniformBufferDescriptorSet,
    };
    vkCmdBindDescriptorSets(compute->commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, compute->pipelineLayout, 0, 3,
                            descriptorSets, 0, NULL);

    vkCmdDispatch(compute->commandBuffer, WORKGROUP_SIZE_X, WORKGROUP_SIZE_Y, WORKGROUP_SIZE_Z);

    ASSERT_VK(vkEndCommandBuffer(compute->commandBuffer))
}

void createSemaphore(VkDevice device, VkSemaphore *semaphore)
{
    VkSemaphoreCreateInfo semaphoreCreateInfo = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    ASSERT_VK(vkCreateSemaphore(device, &semaphoreCreateInfo, NULL, semaphore));
}

void shutdownComputeVulkan(Compute *compute)
{
    vkDeviceWaitIdle(compute->device);

    vkDestroySemaphore(compute->device, compute->semaphore, NULL);

    vkFreeMemory(compute->device, compute->particleBufferMemory, NULL);
    vkFreeMemory(compute->device, compute->dtUniformBufferMemory, NULL);
    vkFreeMemory(compute->device, compute->staticInUniformBufferMemory, NULL);

    vkDestroyBuffer(compute->device, compute->particleBuffer, NULL);
    vkDestroyBuffer(compute->device, compute->dtUniformBuffer, NULL);
    vkDestroyBuffer(compute->device, compute->staticInUniformBuffer, NULL);

    vkDestroyPipelineLayout(compute->device, compute->pipelineLayout, NULL);

    vkDestroyDescriptorPool(compute->device, compute->particleBufferDescriptorPool, NULL);
    vkDestroyDescriptorPool(compute->device, compute->dtUniformBufferDescriptorPool, NULL);
    vkDestroyDescriptorPool(compute->device, compute->staticInUniformBufferDescriptorPool, NULL);

    vkDestroyDescriptorSetLayout(compute->device, compute->particleBufferDescriptorSetLayout, NULL);
    vkDestroyDescriptorSetLayout(compute->device, compute->dtUniformBufferDescriptorSetLayout, NULL);
    vkDestroyDescriptorSetLayout(compute->device, compute->staticInUniformBufferDescriptorSetLayout, NULL);

    vkDestroyPipeline(compute->device, compute->pipeline, NULL);

    vkDestroyShaderModule(compute->device, compute->shaderModule, NULL);

    vkFreeCommandBuffers(compute->device, compute->commandPool, 1, &(compute->commandBuffer));

    vkDestroyCommandPool(compute->device, compute->commandPool, NULL);

    vkDestroyDevice(compute->device, NULL);

    vkDestroyInstance(compute->instance, NULL);
}

void createGraphicsSurface(Graphics *graphics, GLFWwindow *window)
{
    ASSERT_VK(glfwCreateWindowSurface(graphics->instance, window, NULL, &(graphics->surface)))
}

void createSwapchain(Graphics *graphics)
{
    VkBool32 swapChainSupport;
    ASSERT_VK(vkGetPhysicalDeviceSurfaceSupportKHR(graphics->physicalDevice, 0, graphics->surface, &swapChainSupport))
    if (!swapChainSupport)
    {
        printf("ERROR: Swap chain not supported!");
        assert(!swapChainSupport);
    }

    VkExtent2D imageExtent = { WIDTH, HEIGHT };
    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = graphics->surface;
    swapChainCreateInfo.minImageCount = 1;
    swapChainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    swapChainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapChainCreateInfo.imageExtent = imageExtent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    swapChainCreateInfo.clipped = VK_FALSE;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    ASSERT_VK(vkCreateSwapchainKHR(graphics->device, &swapChainCreateInfo, NULL, &(graphics->swapChain)))

    vkGetSwapchainImagesKHR(graphics->device, graphics->swapChain, &(graphics->imageViewsSize), NULL);
    VkImage swapChainImages[graphics->imageViewsSize];
    ASSERT_VK(vkGetSwapchainImagesKHR(graphics->device, graphics->swapChain, &(graphics->imageViewsSize), swapChainImages))

    graphics->imageViews = malloc(graphics->imageViewsSize * sizeof(VkImageView));

    for (int i = 0; i < graphics->imageViewsSize; i++)
    {
        VkImageViewCreateInfo imageViewInfo = {};
        VkComponentMapping componentMapping = {
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY
        };
        VkImageSubresourceRange subresourceRange = {
                VK_IMAGE_ASPECT_COLOR_BIT,0, 1, 0, 1
        };

        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.image = swapChainImages[i];
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
        imageViewInfo.components = componentMapping;
        imageViewInfo.subresourceRange = subresourceRange;

        ASSERT_VK(vkCreateImageView(graphics->device, &imageViewInfo, NULL, &(graphics->imageViews[i])))
    }
}

void createShaderStageInfo(VkPipelineShaderStageCreateInfo *shaderStageCreateInfo, VkShaderStageFlagBits shaderStageFlagBits,
                      VkShaderModule shaderModule)
{
    shaderStageCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo->stage = shaderStageFlagBits;
    shaderStageCreateInfo->module = shaderModule;
    shaderStageCreateInfo->pName = "main";
}

void createGraphicsPipeline(Graphics *graphics)
{
    VkVertexInputBindingDescription vertexInputBindingDescription;
    vertexInputBindingDescription.binding = 0;
    vertexInputBindingDescription.stride = PARTICLE_SIZE;
    vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription positionVertexInputAttributeDescription;
    positionVertexInputAttributeDescription.binding = 0;
    positionVertexInputAttributeDescription.location = 0;
    positionVertexInputAttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
    positionVertexInputAttributeDescription.offset = 0;

    VkVertexInputAttributeDescription colInVertexInputAttributeDescription;
    colInVertexInputAttributeDescription.binding = 0;
    colInVertexInputAttributeDescription.location = 1;
    colInVertexInputAttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
    colInVertexInputAttributeDescription.offset = 24;

    VkVertexInputAttributeDescription vertexInputAttributeDescription[2] = {
            positionVertexInputAttributeDescription,
            colInVertexInputAttributeDescription
    };

    VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = {};
    vertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
    vertexInputStateInfo.vertexAttributeDescriptionCount = 2;
    vertexInputStateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescription;

    createShaderModule(graphics->device, "./vulkan/vert.spv", &(graphics->vertexShaderModule));
    createShaderModule(graphics->device, "./vulkan/frag.spv", &(graphics->fragmentShaderModule));

    VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {};
    createShaderStageInfo(&vertexShaderStageInfo, VK_SHADER_STAGE_VERTEX_BIT, graphics->vertexShaderModule);
    VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = {};
    createShaderStageInfo(&fragmentShaderStageInfo, VK_SHADER_STAGE_FRAGMENT_BIT, graphics->fragmentShaderModule);

    VkPipelineShaderStageCreateInfo shaderStages[] = {
            vertexShaderStageInfo,
            fragmentShaderStageInfo
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = {};
    inputAssemblyStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    inputAssemblyStateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = WIDTH;
    viewport.height = HEIGHT;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = { {0, 0}, {WIDTH, HEIGHT} };

    VkPipelineViewportStateCreateInfo viewportStateInfo = {};
    viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateInfo.viewportCount = 1;
    viewportStateInfo.pViewports = &viewport;
    viewportStateInfo.scissorCount = 1;
    viewportStateInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationStateInfo = {};
    rasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationStateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationStateInfo.depthBiasClamp = 0.0f;
    rasterizationStateInfo.depthBiasSlopeFactor = 0.0f;
    rasterizationStateInfo.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisampleStateInfo = {};
    multisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleStateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateInfo.minSampleShading = 1.0f;
    multisampleStateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleStateInfo.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    colorBlendAttachmentState.blendEnable = VK_FALSE;
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlendStateInfo = {};
    colorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateInfo.logicOp = VK_LOGIC_OP_NO_OP;
    colorBlendStateInfo.attachmentCount = 1;
    colorBlendStateInfo.pAttachments = &colorBlendAttachmentState;
    colorBlendStateInfo.blendConstants[0] = 0.0f;
    colorBlendStateInfo.blendConstants[1] = 0.0f;
    colorBlendStateInfo.blendConstants[2] = 0.0f;
    colorBlendStateInfo.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    ASSERT_VK(vkCreatePipelineLayout(graphics->device, &layoutInfo, NULL, &(graphics->pipelineLayout)))

    VkAttachmentDescription attachmentDescription = {};
    attachmentDescription.format = VK_FORMAT_B8G8R8A8_UNORM;
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference attachmentReference = {};
    attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &attachmentReference;

    VkSubpassDependency subpassDependency = {};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &attachmentDescription;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &subpassDependency;


    ASSERT_VK(vkCreateRenderPass(graphics->device, &renderPassInfo, NULL, &(graphics->renderPass)))

    VkGraphicsPipelineCreateInfo graphicsPipelineInfo = {};
    graphicsPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineInfo.stageCount = 2;
    graphicsPipelineInfo.pStages = shaderStages;
    graphicsPipelineInfo.pVertexInputState = &vertexInputStateInfo;
    graphicsPipelineInfo.pInputAssemblyState = &inputAssemblyStateInfo;
    graphicsPipelineInfo.pViewportState = &viewportStateInfo;
    graphicsPipelineInfo.pRasterizationState = &rasterizationStateInfo;
    graphicsPipelineInfo.pMultisampleState = &multisampleStateInfo;
    graphicsPipelineInfo.pColorBlendState = &colorBlendStateInfo;
    graphicsPipelineInfo.layout = graphics->pipelineLayout;
    graphicsPipelineInfo.renderPass = graphics->renderPass;
    graphicsPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineInfo.basePipelineIndex = -1;

    ASSERT_VK(vkCreateGraphicsPipelines(graphics->device, VK_NULL_HANDLE, 1, &graphicsPipelineInfo, NULL, &(graphics->pipeline)))
}

void createFramebuffer(Graphics *graphics)
{
    graphics->framebuffers = malloc(graphics->imageViewsSize * sizeof(VkFramebuffer));
    for (int i = 0; i < graphics->imageViewsSize; ++i)
    {
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = graphics->renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = &(graphics->imageViews[i]);
        framebufferInfo.width = WIDTH;
        framebufferInfo.height = HEIGHT;
        framebufferInfo.layers = 1;

        ASSERT_VK(vkCreateFramebuffer(graphics->device, &framebufferInfo, NULL, &(graphics->framebuffers[i])))
    }
}

void createGraphicsCommandBuffers(Graphics *graphics)
{
    VkCommandPoolCreateInfo commandPoolInfo = {};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.queueFamilyIndex = graphics->queueFamilyIndex;

    ASSERT_VK(vkCreateCommandPool(graphics->device, &commandPoolInfo, NULL, &(graphics->commandPool)))

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = graphics->commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = graphics->imageViewsSize;

    graphics->commandBuffers = malloc(graphics->imageViewsSize * sizeof(VkCommandBuffer));

    ASSERT_VK(vkAllocateCommandBuffers(graphics->device, &commandBufferAllocateInfo, graphics->commandBuffers))

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    VkRect2D renderArea = { {0, 0}, {WIDTH, HEIGHT} };
    VkClearValue clearValue = {0, 0, 0, 1};

    for (int i = 0; i < graphics->imageViewsSize; i++)
    {
        ASSERT_VK(vkBeginCommandBuffer(graphics->commandBuffers[i], &commandBufferBeginInfo))

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = graphics->renderPass;
        renderPassBeginInfo.framebuffer = graphics->framebuffers[i];
        renderPassBeginInfo.renderArea = renderArea;
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearValue;

        vkCmdBeginRenderPass(graphics->commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(graphics->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics->pipeline);

        VkDeviceSize offsets[1] = { 0 };
        vkCmdBindVertexBuffers(graphics->commandBuffers[i], 0, 1, &(graphics->particleBuffer), offsets);

        vkCmdDraw(graphics->commandBuffers[i], PARTICLE_AMOUNT, 1, 0, 0);

        vkCmdEndRenderPass(graphics->commandBuffers[i]);

        ASSERT_VK(vkEndCommandBuffer(graphics->commandBuffers[i]))
    }
}

void shutdownGraphicsVulkan(Graphics *graphics)
{
    vkDeviceWaitIdle(graphics->device);

    for (int i = 0; i < graphics->imageViewsSize; ++i)
    {
        vkDestroyFramebuffer(graphics->device, graphics->framebuffers[i], NULL);
    }

    vkDestroyCommandPool(graphics->device, graphics->commandPool, NULL);

    vkDestroySemaphore(graphics->device, graphics->semaphore, NULL);
    vkDestroySemaphore(graphics->device, graphics->renderComplete, NULL);
    vkDestroySemaphore(graphics->device, graphics->presentComplete, NULL);

    vkDestroyPipelineLayout(graphics->device, graphics->pipelineLayout, NULL);

    vkDestroyRenderPass(graphics->device, graphics->renderPass, NULL);

    vkDestroyPipeline(graphics->device, graphics->pipeline, NULL);

    vkDestroyShaderModule(graphics->device, graphics->fragmentShaderModule, NULL);
    vkDestroyShaderModule(graphics->device, graphics->vertexShaderModule, NULL);

    for (int i = 0; i < graphics->imageViewsSize; ++i)
    {
        vkDestroyImageView(graphics->device, graphics->imageViews[i], NULL);
    }

    vkDestroySwapchainKHR(graphics->device, graphics->swapChain, NULL);

    vkDestroySurfaceKHR(graphics->instance, graphics->surface, NULL);
}