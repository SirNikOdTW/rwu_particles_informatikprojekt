#include "initVulkan.h"

int initVulkan(VkInstance *vkInstance, VkDevice *device, VkSurfaceKHR *surface, GLFWwindow *window,
               VkSwapchainKHR *swapChain, VkImageView *imageViews, uint32_t *amountImages)
{
    // VkApplicationInfo
    VkApplicationInfo appInfo;
    initAppInfo(&appInfo);

    // VkInstanceCreateInfo
    uint32_t  amountOfLayers;
    vkEnumerateInstanceLayerProperties(&amountOfLayers, NULL);
    VkLayerProperties layers[amountOfLayers];
    vkEnumerateInstanceLayerProperties(&amountOfLayers, layers);

    VkInstanceCreateInfo instanceInfo;
    initCreateInfo(&appInfo, &instanceInfo);

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
    initQueueInfo(&queueInfo);

    // Device info
    VkPhysicalDeviceFeatures usedFeatures = {};
    VkDeviceCreateInfo deviceInfo;
    initDeviceInfo(&queueInfo, &deviceInfo, &usedFeatures);

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
    initSwapChainInfo(&swapChainCreateInfo, surface);

    // Swap chain
    ASSERT_VK_SUCCESS(vkCreateSwapchainKHR(*device, &swapChainCreateInfo, NULL, swapChain))

    // Swap chain images
    vkGetSwapchainImagesKHR(*device, *swapChain, amountImages, NULL);
    VkImage swapChainImages[*amountImages];
    ASSERT_VK_SUCCESS(vkGetSwapchainImagesKHR(*device, *swapChain, amountImages, swapChainImages))

    // Image view
    imageViews = malloc(*amountImages * sizeof(VkImageView));
    VkImageViewCreateInfo imageViewInfo;
    for (int i = 0; i < *amountImages; i++)
    {
        initImageViewInfo(&imageViewInfo, swapChainImages, i);
        ASSERT_VK_SUCCESS(vkCreateImageView(*device, &imageViewInfo, NULL, &imageViews[i]))
    }

    return SUCCESS;
}

void initAppInfo(VkApplicationInfo *appInfo)
{
    appInfo->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo->pNext = NULL;
    appInfo->pApplicationName = APP_NAME;
    appInfo->applicationVersion = APP_VERSION;
    appInfo->pEngineName = ENGINE_NAME;
    appInfo->engineVersion = ENGINE_VERSION;
    appInfo->apiVersion = VK_API_VERSION_1_1;
}

void initCreateInfo(VkApplicationInfo *appInfo, VkInstanceCreateInfo *instanceInfo)
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

void initQueueInfo(VkDeviceQueueCreateInfo *queueInfo)
{
    queueInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo->pNext = NULL;
    queueInfo->flags = 0;
    queueInfo->queueFamilyIndex = 0;
    queueInfo->queueCount = 1;
    queueInfo->pQueuePriorities = NULL;
}

void initDeviceInfo(VkDeviceQueueCreateInfo *queueInfo, VkDeviceCreateInfo *deviceInfo, VkPhysicalDeviceFeatures *features)
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

void initSwapChainInfo(VkSwapchainCreateInfoKHR *swapChainCreateInfo, VkSurfaceKHR *surface)
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

void initImageViewInfo(VkImageViewCreateInfo *imageViewInfo, VkImage *swapChainImages, int index)
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

void shutdownVulkan(VkInstance *vkInstance, VkDevice *device, VkSurfaceKHR *surface, VkSwapchainKHR *swapChain,
                    VkImageView *imageViews, uint32_t amountImages)
{
    vkDeviceWaitIdle(*device);

    for (int i = 0; i < amountImages; i++)
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