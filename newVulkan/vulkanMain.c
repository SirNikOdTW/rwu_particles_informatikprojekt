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

    // Compute
    createComputeBuffers(&compute);
    createComputeDescriptorSetLayouts(&compute);
    createComputeDescriptorSets(&compute);
    createComputePipeline(&compute);
    fillComputeBuffers(&compute, particles, &dt, &staticIn);
    createComputeCommandBuffer(&compute);
    createSemaphore(compute.device, &(compute.semaphore));

    // Graphics
    createGraphicsSurface(&graphics, window);
    createSwapchain(&graphics);
    createGraphicsPipeline(&graphics);
    createGraphicsCommandBuffers(&graphics);
    createSemaphore(graphics.device, &(graphics.semaphore));


    /************* RENDER LOOP *************/
    double time, tLast = 0;
    Dt tFrame = {};
    while (!glfwWindowShouldClose(window))
    {
        time = glfwGetTime();
        tFrame.dt = (float) (time - tLast);
        tLast = time;

        // Update dt
        mapBufferMemory(&compute, compute.dtUniformBufferMemory, &tFrame, sizeof(Dt));

        /*** UPDATE ***/
        runComputeCommandBuffer(&compute);

        /*** RENDER ***/

        glfwPollEvents();
    }

    shutdownComputeVulkan(&compute);
    shutdownGLFW(window);
}

