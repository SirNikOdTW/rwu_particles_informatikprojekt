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

#define PARTICLE_AMOUNT 10
#define PARTICLE_SIZE (3 * sizeof(vector3f) + sizeof(float))
#define WORKGROUP_SIZE_X 256
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

    VkDescriptorPool particleBufferDescriptorPool;
    VkDescriptorSet particleBufferDescriptorSet;
    VkDescriptorSetLayout particleBufferDescriptorSetLayout;
    VkBuffer particleBuffer;
    VkDeviceMemory particleBufferMemory;
    uint32_t particleBufferSize;

    VkDescriptorPool dtUniformBufferDescriptorPool;
    VkDescriptorSet dtUniformBufferDescriptorSet;
    VkDescriptorSetLayout dtUniformBufferDescriptorSetLayout;
    VkBuffer dtUniformBuffer;
    VkDeviceMemory dtUniformBufferMemory;
    uint32_t dtUniformBufferSize;

    VkDescriptorPool staticInUniformBufferDescriptorPool;
    VkDescriptorSet staticInUniformBufferDescriptorSet;
    VkDescriptorSetLayout staticInUniformBufferDescriptorSetLayout;
    VkBuffer staticInUniformBuffer;
    VkDeviceMemory staticInUniformBufferMemory;
    uint32_t staticInUniformBufferSize;

    VkQueue queue;
    uint32_t queueFamilyIndex;
} Compute;

typedef struct graphics {
    VkInstance instance;

    VkPhysicalDevice physicalDevice;
    VkDevice device;

    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
    VkShaderModule shaderModule;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    VkQueue queue;
    uint32_t queueFamilyIndex;
} Graphics;

typedef struct dt {
    float dt;
} Dt;

typedef struct staticIn {
    float x, y, z;
    unsigned int maxParticles;
} StaticIn;

// Shutdown
void shutdownGLFW(GLFWwindow *window);
void shutdownComputeVulkan(Compute *compute);

void createInstance(Compute *compute);
void findPhysicalDevice(Compute *compute);
void createDevice(Compute *compute);

// Compute
void createComputeBuffers(Compute *compute);
void createComputeDescriptorSetLayouts(Compute *compute);
void createComputeDescriptorSets(Compute *compute);
void createComputePipeline(Compute *compute);
void fillComputeBuffers(Compute *compute, float *particles, Dt *dtData, StaticIn *staticInData);
void createComputeCommandBuffer(Compute *compute);
void runComputeCommandBuffer(Compute *compute);

// ELse
void mapBufferMemory(Compute *compute, VkDeviceMemory memory, void *inputData, uint32_t dataSize);
