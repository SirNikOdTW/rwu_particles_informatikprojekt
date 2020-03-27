#include "initVulkan.h"
#include <string.h>

void shutdownGLFW(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void createInstance(Compute *compute)
{
    GLuint glfwExtensionsSize;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsSize);

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
    instanceCreateInfo.enabledLayerCount = 0;
    instanceCreateInfo.ppEnabledLayerNames = NULL;
    instanceCreateInfo.enabledExtensionCount = glfwExtensionsSize;
    instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;

    ASSERT_VK(vkCreateInstance(&instanceCreateInfo, NULL, &(compute->instance)))
}

void findPhysicalDevice(Compute *compute)
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
            && PARTICLE_SIZE * PARTICLE_AMOUNT <= deviceProperties.limits.maxStorageBufferRange)
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
}

uint32_t getQueueFamilyIndex(Compute *compute, VkQueueFlagBits queueFlagBits)
{
    uint32_t queueFamiliesSize;
    vkGetPhysicalDeviceQueueFamilyProperties(compute->physicalDevice, &queueFamiliesSize, NULL);
    VkQueueFamilyProperties queueFamilies[queueFamiliesSize];
    vkGetPhysicalDeviceQueueFamilyProperties(compute->physicalDevice, &queueFamiliesSize, queueFamilies);

    for (uint32_t i = 0; i < queueFamiliesSize; ++i)
    {
        if (queueFamilies[i].queueCount > 0
            && queueFamilies[i].queueFlags & queueFlagBits)
        {
            return i;
        }
    }

    return -1;
}

void createDevice(Compute *compute)
{
    compute->queueFamilyIndex = getQueueFamilyIndex(compute, VK_QUEUE_COMPUTE_BIT);

    if (compute->queueFamilyIndex < 0)
    {
        printf("Fatal: Could not find a queue family with capability of computing!");
        assert(compute->queueFamilyIndex >= 0);
    }

    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.queueFamilyIndex = compute->queueFamilyIndex;
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = NULL;

    VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

    ASSERT_VK(vkCreateDevice(compute->physicalDevice, &deviceCreateInfo, NULL, &(compute->device)))

    vkGetDeviceQueue(compute->device, compute->queueFamilyIndex, 0, &(compute->queue));
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

    return -1;
}

void createComputeBuffer(Compute *compute, uint32_t bufferSize, VkBufferUsageFlags usageFlags, VkBuffer *buffer,
                         VkDeviceMemory *bufferMemory, VkMemoryPropertyFlags memoryPropertyFlags)
{

    VkBufferCreateInfo particleBufferCreateInfo = {};
    particleBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    particleBufferCreateInfo.size = bufferSize;
    particleBufferCreateInfo.usage = usageFlags;
    particleBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    ASSERT_VK(vkCreateBuffer(compute->device, &particleBufferCreateInfo, NULL, buffer))

    VkMemoryRequirements particleBufferMemoryRequirements;
    vkGetBufferMemoryRequirements(compute->device, *buffer, &particleBufferMemoryRequirements);

    VkMemoryAllocateInfo particleMemoryAllocateInfo = {};
    particleMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    particleMemoryAllocateInfo.allocationSize = particleBufferMemoryRequirements.size;
    particleMemoryAllocateInfo.memoryTypeIndex = findMemoryType(compute,
                                                                particleBufferMemoryRequirements.memoryTypeBits,
                                                                memoryPropertyFlags);

    ASSERT_VK(vkAllocateMemory(compute->device, &particleMemoryAllocateInfo, NULL, bufferMemory))

    ASSERT_VK(vkBindBufferMemory(compute->device, *buffer, *bufferMemory, 0))
}

void createComputeBuffers(Compute *compute)
{
    // Particle Buffer
    createComputeBuffer(compute, compute->particleBufferSize,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                        &(compute->particleBuffer),
                        &(compute->particleBufferMemory), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // dt Uniform Buffer
    createComputeBuffer(compute, compute->dtUniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        &(compute->dtUniformBuffer),
                        &(compute->dtUniformBufferMemory),
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // staticIn Uniform Buffer
    createComputeBuffer(compute, compute->staticInUniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        &(compute->staticInUniformBuffer),
                        &(compute->staticInUniformBufferMemory), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

}

void createComputeDescriptorSetLayout(Compute *compute, uint32_t binding, VkDescriptorType descriptorType, VkDescriptorSetLayout *descriptorSetLayout)
{
    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
    descriptorSetLayoutBinding.binding = binding;
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
    createComputeDescriptorSetLayout(compute, 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                     &(compute->particleBufferDescriptorSetLayout));

    // dt Uniform Buffer
    createComputeDescriptorSetLayout(compute, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                     &(compute->dtUniformBufferDescriptorSetLayout));

    // staticIn Uniform Buffer
    createComputeDescriptorSetLayout(compute, 2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
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

void createComputePipeline(Compute *compute)
{
    long computeShaderSourceSize;
    char *computeShaderSource = readFile("./vulkan/comp.spv", "rb", &computeShaderSourceSize);

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.pCode = (uint32_t *) computeShaderSource;
    shaderModuleCreateInfo.codeSize = computeShaderSourceSize;

    ASSERT_VK(vkCreateShaderModule(compute->device, &shaderModuleCreateInfo, NULL, &(compute->shaderModule)))

    free(computeShaderSource);

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

    ASSERT_VK(vkCreatePipelineLayout(compute->device, &pipelineLayoutCreateInfo, NULL, &(compute->pipelineLayout)));

    VkComputePipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stage = pipelineShaderStageCreateInfo;
    pipelineCreateInfo.layout = compute->pipelineLayout;

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
    uint32_t queueFamilyIndex = getQueueFamilyIndex(compute, VK_QUEUE_TRANSFER_BIT);

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

void createComputeCommandBuffer(Compute *compute)
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

void runComputeCommandBuffer(Compute *compute)
{
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &(compute->commandBuffer);

    VkFence fence;
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;

    ASSERT_VK(vkCreateFence(compute->device, &fenceCreateInfo, NULL, &fence))

    ASSERT_VK(vkQueueSubmit(compute->queue, 1, &submitInfo, fence))

    ASSERT_VK(vkWaitForFences(compute->device, 1, &fence, VK_TRUE, UINT64_MAX));

    vkDestroyFence(compute->device, fence, NULL);
}

void shutdownComputeVulkan(Compute *compute)
{
    vkFreeMemory(compute->device, compute->particleBufferMemory, NULL);
    vkFreeMemory(compute->device, compute->dtUniformBufferMemory, NULL);
    vkFreeMemory(compute->device, compute->staticInUniformBufferMemory, NULL);

    vkDestroyBuffer(compute->device, compute->particleBuffer, NULL);
    vkDestroyBuffer(compute->device, compute->dtUniformBuffer, NULL);
    vkDestroyBuffer(compute->device, compute->staticInUniformBuffer, NULL);

    vkDestroyShaderModule(compute->device, compute->shaderModule, NULL);

    vkDestroyDescriptorPool(compute->device, compute->particleBufferDescriptorPool, NULL);
    vkDestroyDescriptorPool(compute->device, compute->dtUniformBufferDescriptorPool, NULL);
    vkDestroyDescriptorPool(compute->device, compute->staticInUniformBufferDescriptorPool, NULL);

    vkDestroyDescriptorSetLayout(compute->device, compute->particleBufferDescriptorSetLayout, NULL);
    vkDestroyDescriptorSetLayout(compute->device, compute->dtUniformBufferDescriptorSetLayout, NULL);
    vkDestroyDescriptorSetLayout(compute->device, compute->staticInUniformBufferDescriptorSetLayout, NULL);

    vkDestroyPipelineLayout(compute->device, compute->pipelineLayout, NULL);

    vkDestroyPipeline(compute->device, compute->pipeline, NULL);

    vkDestroyCommandPool(compute->device, compute->commandPool, NULL);

    vkDestroyDevice(compute->device, NULL);

    vkDestroyInstance(compute->instance, NULL);
}
