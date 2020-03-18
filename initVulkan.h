#include <stdio.h>
#include <malloc.h>
#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "utils.h"

#define APP_NAME "Informatikprojekt - Vulkan"
#define APP_VERSION VK_MAKE_VERSION(0, 0, 0)
#define ENGINE_NAME "rwu_particles"
#define ENGINE_VERSION VK_MAKE_VERSION(0, 0, 0)

#define SUCCESS 0
#define FAILURE -1
#define ASSERT_SUCCESS(res)\
    if (res != SUCCESS) { printf("Error-Code: %d", res); return FAILURE; }
#define ASSERT_VK_SUCCESS(res)\
    if (res != VK_SUCCESS) { printf("Error-Code: %d", res); return FAILURE; }
#define ASSERT_GLFW_SUCCESS(res)\
    if (res != GLFW_TRUE) { printf("Error-Code: %d", res); return FAILURE; }
#define BOOL_LITERAL(val)\
    val ? "True" : "False"
#define HUMAN_READABLE(val)\
    val * 9.313226e-10

int initVulkan(VkInstance *vkInstance, VkDevice *device, VkSurfaceKHR *surface, GLFWwindow *window,
               VkSwapchainKHR *swapChain, VkImageView **imageViews, uint32_t *amountImages);
void initAppInfo(VkApplicationInfo *appInfo);
void initCreateInfo(VkApplicationInfo *appInfo, VkInstanceCreateInfo *instanceInfo);
void initQueueInfo(VkDeviceQueueCreateInfo *queueInfo);
void initDeviceInfo(VkDeviceQueueCreateInfo *queueInfo, VkDeviceCreateInfo *deviceInfo, VkPhysicalDeviceFeatures *features);
void initImageViewInfo(VkImageViewCreateInfo *imageViewInfo, VkImage *swapChainImages, int index);
void printStats(VkPhysicalDevice *physicalDevice, VkSurfaceKHR *surface);

void initSwapChainInfo(VkSwapchainCreateInfoKHR *swapChainCreateInfo, VkSurfaceKHR *surface);

void shutdownVulkan(VkInstance *vkInstance, VkDevice *device, VkSurfaceKHR *surface, VkSwapchainKHR *swapChain,
                    VkImageView *imageViews, uint32_t amountImages);

void shutdownGLFW(GLFWwindow *window);
