#include <stdio.h>
#include <string.h>

#include "initVulkan.h"
#include "particlesystem.h"

#define PARTICLE_AMOUNT 100

int main()
{
    VkInstance vkInstance;
    VkDevice device;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    VkImageView *imageViews = NULL;
    uint32_t amountImages;
    VkPipelineLayout pipelineLayout;

    /************* INIT *************/
    // GLFW
    ASSERT_GLFW_SUCCESS(glfwInit())
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Informatikprojekt - Vulkan", NULL, NULL);

    // Init Vulkan
    ASSERT_SUCCESS(initVulkan(&vkInstance, &device, &surface, window, &swapChain, &imageViews, &amountImages))

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

    VkPipelineShaderStageCreateInfo shaderStages[3] = { computeShaderStageInfo, vertexShaderStageInfo, fragmentShaderStageInfo };

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

    VkAttachmentDescription attachmentDescription;
    createAttachmentDescription(&attachmentDescription);


    /************* RENDER LOOP *************/
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    // Shutdown Vulkan
    VkShaderModule modules[3] = { computeShaderModule, vertexShaderModule, fragmentShaderModule };
    shutdownVulkan(&vkInstance, &device, &surface, &swapChain, imageViews, amountImages, modules, 3, &pipelineLayout);

    // Shutdown GLFW
    shutdownGLFW(window);

    return SUCCESS;
}


