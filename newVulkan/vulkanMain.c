#include <stdio.h>

#include "initVulkan.h"

int main()
{
    /************* PARTICLE SYSTEM *************/
    vector3f *epos1 = initVector3f(0, 0, 0);
    emitter *e1 = initEmitter(epos1, PARTICLE_AMOUNT);
    particle_system *ps = initParticleSystem(1);
    (ps->emitters)[0] = e1;
    initRandomParticles(e1);
    float *particles = serializeParticlesystem(ps);

    Dt dt = { 0.5f };
    StaticIn staticIn = {
            e1->position->x,
            e1->position->y,
            e1->position->z,
            PARTICLE_AMOUNT
    };

    freeParticleSystem(ps);

    /************* INIT GLFW *************/
    ASSERT_GLFW_SUCCESS(glfwInit());
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Informatikprojekt - Vulkan", NULL, NULL);

    /************* INIT VULKAN *************/
    Compute compute = {
            .particleBufferSize = PARTICLE_SIZE * PARTICLE_AMOUNT,
            .staticInUniformBufferSize = sizeof(StaticIn),
            .dtUniformBufferSize = sizeof(Dt)
    };
    Graphics graphics = {};

    // General
    createInstance(&compute, &graphics);
    findPhysicalDevice(&compute, &graphics);
    createDevice(&compute, &graphics);
    createParticleBuffer(&compute, &graphics);

    // Compute
    createComputeBuffers(&compute);
    createComputeDescriptorSetLayouts(&compute);
    createComputeDescriptorSets(&compute);
    createComputePipeline(&compute);
    fillComputeBuffers(&compute, particles, &dt, &staticIn);

    createSemaphore(compute.device, &(compute.semaphore));
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &compute.semaphore;
    ASSERT_VK(vkQueueSubmit(compute.queue, 1, &submitInfo, VK_NULL_HANDLE));
    ASSERT_VK(vkQueueWaitIdle(compute.queue));

    createComputeCommandBuffer(&compute, &graphics);

    // Graphics
    graphics.particleBuffer = compute.particleBuffer;
    graphics.particleBufferSize = compute.particleBufferSize;
    createGraphicsSurface(&graphics, window);
    createSwapchain(&graphics);
    createGraphicsPipeline(&graphics);
    createFramebuffer(&graphics);
    createGraphicsCommandBuffers(&graphics);
    createSemaphore(graphics.device, &(graphics.presentComplete));
    createSemaphore(graphics.device, &(graphics.renderComplete));
    createSemaphore(graphics.device, &(graphics.semaphore));

    /************* RENDER LOOP *************/
    // Graphics preparation
    VkPipelineStageFlags graphicsWaitStageMasks[] = {
            VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    VkSemaphore graphicsWaitSemaphores[] = { compute.semaphore, graphics.presentComplete };
    VkSemaphore graphicsSignalSemaphores[] = { graphics.semaphore, graphics.renderComplete };

    VkSubmitInfo graphicsSubmitInfo = {};
    graphicsSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    graphicsSubmitInfo.commandBufferCount = 1;
    graphicsSubmitInfo.waitSemaphoreCount = 2;
    graphicsSubmitInfo.pWaitSemaphores = graphicsWaitSemaphores;
    graphicsSubmitInfo.pWaitDstStageMask = graphicsWaitStageMasks;
    graphicsSubmitInfo.signalSemaphoreCount = 2;
    graphicsSubmitInfo.pSignalSemaphores = graphicsSignalSemaphores;

    VkPresentInfoKHR graphicsPresentInfo = {};
    graphicsPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    graphicsPresentInfo.waitSemaphoreCount = 1;
    graphicsPresentInfo.pWaitSemaphores = &(graphics.renderComplete);
    graphicsPresentInfo.swapchainCount = 1;
    graphicsPresentInfo.pSwapchains = &(graphics.swapChain);


    // Compute preparation
    VkPipelineStageFlags computeWaitStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

    VkSubmitInfo computeSubmitInfo = {};
    computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    computeSubmitInfo.commandBufferCount = 1;
    computeSubmitInfo.pCommandBuffers = &(compute.commandBuffer);
    computeSubmitInfo.waitSemaphoreCount = 1;
    computeSubmitInfo.pWaitSemaphores = &graphics.semaphore;
    computeSubmitInfo.pWaitDstStageMask = &computeWaitStageMask;
    computeSubmitInfo.signalSemaphoreCount = 1;
    computeSubmitInfo.pSignalSemaphores = &compute.semaphore;

    // Loop
    double time, tLast = 0;
    Dt tFrame = {};
    uint32_t imageIndex;
    while (!glfwWindowShouldClose(window))
    {
        time = glfwGetTime();
        tFrame.dt = (float) (time - tLast);
        tLast = time;

        /*** RENDER ***/
        ASSERT_VK(vkAcquireNextImageKHR(graphics.device, graphics.swapChain, UINT64_MAX, graphics.presentComplete, VK_NULL_HANDLE, &imageIndex))

        graphicsSubmitInfo.pCommandBuffers = &(graphics.commandBuffers[imageIndex]);
        ASSERT_VK(vkQueueSubmit(graphics.queue, 1, &graphicsSubmitInfo, VK_NULL_HANDLE))

        graphicsPresentInfo.pImageIndices = &imageIndex;
        ASSERT_VK(vkQueuePresentKHR(graphics.queue, &graphicsPresentInfo))

        /*** UPDATE ***/
        // Update dt
        mapBufferMemory(&compute, compute.dtUniformBufferMemory, &tFrame, sizeof(Dt));
        ASSERT_VK(vkQueueSubmit(compute.queue, 1, &computeSubmitInfo, VK_NULL_HANDLE))

        glfwPollEvents();
    }

    shutdownGraphicsVulkan(&graphics);
    shutdownComputeVulkan(&compute);
    shutdownGLFW(window);

    return SUCCESS;
}

