#include <stdio.h>

#include "initVulkan.h"

int main()
{
    VkInstance vkInstance;
    VkDevice device;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    VkImageView *imageViews;
    uint32_t amountImages;

    // GLFW
    ASSERT_GLFW_SUCCESS(glfwInit())
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Informatikprojekt - Vulkan", NULL, NULL);

    // Init Vulkan
    ASSERT_SUCCESS(initVulkan(&vkInstance, &device, &surface, window, &swapChain, imageViews, &amountImages))

    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    // Stop Vulkan
    shutdownVulkan(&vkInstance, &device, &surface, &swapChain, imageViews, amountImages);

    // Stop GLFW
    shutdownGLFW(window);

    return SUCCESS;
}
