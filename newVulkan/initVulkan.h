#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "../particlesystem.h"
#include "../utils.h"

#define APP_NAME "Informatikprojekt - Vulkan"
#define APP_VERSION VK_MAKE_VERSION(0, 0, 0)
#define ENGINE_NAME "rwu_particles"
#define ENGINE_VERSION VK_MAKE_VERSION(0, 0, 0)

#define PARTICLE_AMOUNT 100
#define PARTICLE_SIZE (3 * sizeof(vector3f) + sizeof(float))
#define WORKGROUP_SIZE_X 1024
#define WORKGROUP_SIZE_Y 1
#define WORKGROUP_SIZE_Z 1

#define SUCCESS 0
#define FAILURE -1
#define ASSERT_VK(f) { \
    VkResult res = (f); \
    if (res != VK_SUCCESS) { \
        printf("Fatal : VkResult is %d in %s at line %d\n", res,  __FILE__, __LINE__); \
        assert(res == VK_SUCCESS); \
    } \
}
#define ASSERT_GLFW_SUCCESS(res) { if (res != GLFW_TRUE) { printf("Error-Code: %d", res); return FAILURE; } }

typedef struct compute {
    VkInstance instance;

    VkPhysicalDevice physicalDevice;
    VkDevice device;

    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
    VkShaderModule shaderModule;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    VkDescriptorSetLayout particleBufferDescriptorSetLayout;
    VkDescriptorPool particleBufferDescriptorPool;
    VkDescriptorSet particleBufferDescriptorSet;
    VkBuffer particleBuffer;
    VkDeviceMemory particleBufferMemory;
    uint32_t particleBufferSize;

    VkDescriptorSetLayout dtUniformBufferDescriptorSetLayout;
    VkDescriptorPool dtUniformBufferDescriptorPool;
    VkDescriptorSet dtUniformBufferDescriptorSet;
    VkBuffer dtUniformBuffer;
    VkDeviceMemory dtUniformBufferMemory;
    uint32_t dtUniformBufferSize;

    VkDescriptorSetLayout staticInUniformBufferDescriptorSetLayout;
    VkDescriptorPool staticInUniformBufferDescriptorPool;
    VkDescriptorSet staticInUniformBufferDescriptorSet;
    VkBuffer staticInUniformBuffer;
    VkDeviceMemory staticInUniformBufferMemory;
    uint32_t staticInUniformBufferSize;

    VkQueue queue;
    uint32_t queueFamilyIndex;
    
    VkSemaphore semaphore;
} Compute;

typedef struct graphics {
    VkInstance instance;

    VkPhysicalDevice physicalDevice;
    VkDevice device;

    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;

    VkImageView *imageViews;
    uint32_t imageViewsSize;

    VkRenderPass renderPass;
    VkFramebuffer *framebuffers;
    
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
    VkShaderModule vertexShaderModule;
    VkShaderModule fragmentShaderModule;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    VkBuffer particleBuffer;
    uint32_t particleBufferSize;

    VkQueue queue;
    uint32_t queueFamilyIndex;
    
    VkSemaphore semaphore;
} Graphics;

typedef struct dt {
    float dt;
} Dt;

typedef struct staticIn {
    float x;
    float y;
    float z;
    unsigned int maxParticles;
} StaticIn;

// Shutdown
void shutdownGLFW(GLFWwindow *window);
void shutdownComputeVulkan(Compute *compute);

void createInstance(Compute *compute, Graphics *graphics);
void findPhysicalDevice(Compute *compute, Graphics *graphics);
void createDevice(Compute *compute, Graphics *graphics);

// Compute
void createComputeBuffers(Compute *compute);
void createComputeDescriptorSetLayouts(Compute *compute);
void createComputeDescriptorSets(Compute *compute);
void createComputePipeline(Compute *compute);
void fillComputeBuffers(Compute *compute, float *particles, Dt *dtData, StaticIn *staticInData);
void createComputeCommandBuffer(Compute *compute);
void runComputeCommandBuffer(Compute *compute);

// Graphics 
void createGraphicsSurface(Graphics *graphics, GLFWwindow *window);
void createSwapchain(Graphics *graphics);
void createGraphicsPipeline(Graphics *graphics);
void createGraphicsCommandBuffers(Graphics *graphics);

// ELse
void mapBufferMemory(Compute *compute, VkDeviceMemory memory, void *inputData, uint32_t dataSize);
void createSemaphore(VkDevice device, VkSemaphore *semaphore);
