#include <stdio.h>

#include "initVulkan.h"
#include "particlesystem.h"

#define PARTICLE_AMOUNT 0

void drawFrame();

typedef struct compute {
    VkInstance instance;
    VkDebugReportCallbackEXT debugReportCallback;

    VkPhysicalDevice physicalDevice;
    VkDevice device;

    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
    VkShaderModule shaderModule;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
    VkDescriptorSetLayout descriptorSetLayout;

    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    uint32_t bufferSize;

    const char **enabledLayer;

    VkQueue queue;
    uint32_t queueFamilyIndex;
} Compute;


int main()
{
    VkInstance vkInstance;
    VkDevice device;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    VkImageView *imageViews = NULL;
    uint32_t imageViewsSize;
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;
    VkPipeline pipeline;
    VkFramebuffer *framebuffers;
    VkCommandPool commandPool;
    VkCommandBuffer *commandBuffers;
    VkSemaphore imageAvailable;
    VkSemaphore renderingFinished;

    /************* INIT *************/
    // GLFW
    ASSERT_GLFW_SUCCESS(glfwInit())
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Informatikprojekt - Vulkan", NULL, NULL);

    // Init Vulkan
    ASSERT_SUCCESS(initVulkan(&vkInstance, &device, &surface, window, &swapChain, &imageViews, &imageViewsSize))

    /************* PARTICLE SYSTEM *************/
    vector3f *epos1 = initVector3f(0, 0, 0);
    emitter *e1 = initEmitter(epos1, PARTICLE_AMOUNT);
    particle_system *ps = initParticleSystem(1);
    (ps->emitters)[0] = e1;
    initRandomParticles(e1);

    /************* SHADER *************/
    // Shader Modules
    long computeShaderSourceSize, vertexShaderSourceSize, fragmentShaderSourceSize;
    char *computeShaderSource = readFile("./vulkan/comp.spv", "rb", &computeShaderSourceSize);
    char *vertexShaderSource = readFile("./vulkan/vert.spv", "rb", &vertexShaderSourceSize);
    char *fragmentShaderSource = readFile("./vulkan/frag.spv", "rb", &fragmentShaderSourceSize);

    VkShaderModule computeShaderModule;
    createShaderModule(device, &computeShaderModule, computeShaderSource, computeShaderSourceSize);
    VkShaderModule vertexShaderModule;
    createShaderModule(device, &vertexShaderModule, vertexShaderSource, vertexShaderSourceSize);
    VkShaderModule fragmentShaderModule;
    createShaderModule(device, &fragmentShaderModule, fragmentShaderSource, fragmentShaderSourceSize);

    // Shader stages
    VkPipelineShaderStageCreateInfo computeShaderStageInfo;
    createShaderStageInfo(&computeShaderStageInfo, VK_SHADER_STAGE_COMPUTE_BIT, computeShaderModule, "main");
    VkPipelineShaderStageCreateInfo vertexShaderStageInfo;
    createShaderStageInfo(&vertexShaderStageInfo, VK_SHADER_STAGE_VERTEX_BIT, vertexShaderModule, "main");
    VkPipelineShaderStageCreateInfo fragmentShaderStageInfo;
    createShaderStageInfo(&fragmentShaderStageInfo, VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShaderModule, "main");

    VkPipelineShaderStageCreateInfo shaderStages[3] = {computeShaderStageInfo, vertexShaderStageInfo,
                                                       fragmentShaderStageInfo};

    /************* PIPELINE *************/
    // Vertex input
    VkPipelineVertexInputStateCreateInfo vertexInputStateInfo;
    createPipelineVertexInputStateInfo(&vertexInputStateInfo, NULL, 0);

    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo;
    createInputAssemblyStateInfo(&inputAssemblyStateInfo, VK_PRIMITIVE_TOPOLOGY_POINT_LIST);

    // Viewport
    VkPipelineViewportStateCreateInfo viewportStateInfo;
    createViewportStateInfo(&viewportStateInfo, WIDTH, HEIGHT);

    // Rasterization
    VkPipelineRasterizationStateCreateInfo rasterizationStateInfo;
    createRasterizationStateInfo(&rasterizationStateInfo, VK_POLYGON_MODE_POINT);

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampleStateInfo;
    createMultisampleStateInfo(&multisampleStateInfo);

    // Blending
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
    createColorBlendAttachmentStateInfo(&colorBlendAttachmentState);
    VkPipelineColorBlendStateCreateInfo colorBlendStateInfo;
    createColorBlendStateInfo(&colorBlendStateInfo, &colorBlendAttachmentState, 1);

    // Layout
    VkPipelineLayoutCreateInfo layoutInfo;
    createLayoutInfo(&layoutInfo, NULL, 0);
    ASSERT_VK_SUCCESS(vkCreatePipelineLayout(device, &layoutInfo, NULL, &pipelineLayout))

    // Attachments
    VkAttachmentDescription attachmentDescription;
    createAttachmentDescription(&attachmentDescription);

    VkAttachmentReference attachmentReference;
    createAttachmentReference(&attachmentReference, 0);

    // Subpasses
    VkSubpassDescription subpassDescription;
    createSubpassDescription(&subpassDescription, VK_PIPELINE_BIND_POINT_GRAPHICS, &attachmentReference);

    // Renderpass
    VkRenderPassCreateInfo renderPassInfo;
    createRenderPassInfo(&renderPassInfo, &attachmentDescription, &subpassDescription);
    ASSERT_VK_SUCCESS(vkCreateRenderPass(device, &renderPassInfo, NULL, &renderPass))

    // Graphics pipeline
    VkGraphicsPipelineCreateInfo graphicsPipelineInfo;
    createGraphicsPipelineInfo(&graphicsPipelineInfo, shaderStages, &vertexInputStateInfo, &inputAssemblyStateInfo,
                               &viewportStateInfo, &rasterizationStateInfo, &multisampleStateInfo, &colorBlendStateInfo,
                               &pipelineLayout, &renderPass);
    ASSERT_VK_SUCCESS(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineInfo, NULL, &pipeline))

    // Framebuffers
    framebuffers = malloc(imageViewsSize * sizeof(VkFramebuffer));
    for (int i = 0; i < imageViewsSize; ++i)
    {
        VkFramebufferCreateInfo framebufferInfo;
        createFramebufferInfo(&framebufferInfo, &renderPass, &imageViews[i]);

        ASSERT_VK_SUCCESS(vkCreateFramebuffer(device, &framebufferInfo, NULL, &framebuffers[i]))
    }

    // Command pool
    VkCommandPoolCreateInfo commandPoolInfo;
    createCommandPoolInfo(&commandPoolInfo, 0);
    ASSERT_VK_SUCCESS(vkCreateCommandPool(device, &commandPoolInfo, NULL, &commandPool))

    // Allocate info
    VkCommandBufferAllocateInfo commandBufferAllocateInfo;
    createCommandBufferAllocateInfo(&commandBufferAllocateInfo, &commandPool, imageViewsSize);

    // Command buffers
    commandBuffers = malloc(imageViewsSize * sizeof(VkCommandBuffer));
    ASSERT_VK_SUCCESS(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers))

    // Begin Info command buffers
    VkCommandBufferBeginInfo commandBufferBeginInfo;
    createCommandBufferBeginInfo(&commandBufferBeginInfo);

    for (int i = 0; i < imageViewsSize; ++i)
    {
        ASSERT_VK_SUCCESS(vkBeginCommandBuffer(commandBuffers[i], &commandBufferBeginInfo))

        VkRenderPassBeginInfo renderPassBeginInfo;
        createRenderPassBeginInfo(&renderPassBeginInfo, &renderPass, &framebuffers[i]);

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffers[i]);

        ASSERT_VK_SUCCESS(vkEndCommandBuffer(commandBuffers[i]))
    }

    // Semaphores
    VkSemaphoreCreateInfo semaphoreInfo;
    createSemaphoreInfo(&semaphoreInfo);

    ASSERT_VK_SUCCESS(vkCreateSemaphore(device, &semaphoreInfo, NULL, &imageAvailable))
    ASSERT_VK_SUCCESS(vkCreateSemaphore(device, &semaphoreInfo, NULL, &renderingFinished))

    /************* RENDER LOOP *************/
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        drawFrame();
    }

    // Shutdown Vulkan
    VkShaderModule modules[3] = {computeShaderModule, vertexShaderModule, fragmentShaderModule};
    VkSemaphore semaphores[2] = {imageAvailable, renderingFinished};
    shutdownVulkan(&vkInstance, &device, &surface, &swapChain, imageViews, imageViewsSize, modules, 3, &pipelineLayout,
                   1,
                   &renderPass, 1, &pipeline, 1, framebuffers, &commandPool, commandBuffers, semaphores, 2);

    // Shutdown GLFW
    shutdownGLFW(window);

    return SUCCESS;
}

void drawFrame(VkDevice *device, VkSwapchainKHR *swapchainKhr, VkSemaphore *imageAvailable, VkSemaphore *renderingFinished)
{
    uint32_t imageIndex;
    vkAcquireNextImageKHR(*device, *swapchainKhr, UINT64_MAX, *imageAvailable, VK_NULL_HANDLE, &imageIndex);
}



