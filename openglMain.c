#include <stdlib.h>
#include "particlesystem.h"
#include "initOpenGL.h"
#include "utils.h"

#define PARTICLE_AMOUNT 1000000

int main()
{
    /************* INIT *************/
    // Init OpenGL and GLFW
    initGLFW();
    setErrorCallbackGL();

    int width = WIDTH, height = HEIGHT;
    GLFWwindow *window = createGLFWWindow(WIDTH, HEIGHT, "Informatikprojekt - OpenGL");

    setCurrentContextGL(window);
    setFramebufferSizeCallbackGL(window);

    // glad
    initGlad();

    /************* PARTICLE SYSTEM *************/
    vector3f *epos1 = initVector3f(0, 0, 0);
    emitter *e1 = initEmitter(epos1, PARTICLE_AMOUNT);
    particle_system *ps = initParticleSystem(1);
    (ps->emitters)[0] = e1;
    initRandomParticles(e1);

    /************* SHADER *************/
    const GLchar *computeShaderSource = readFile("ComputeShader.glsl");
    const GLchar *vertexShaderSource = readFile("VertexShader.glsl");
    const GLchar *fragmentShaderSource = readFile("FragmentShader.glsl");

    GLuint computeShader = compileShader(computeShaderSource, GL_COMPUTE_SHADER);
    GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    GLuint computeShaders[] = { computeShader };
    GLuint computeShaderProgram = linkShaders(computeShaders, 1);
    glUseProgram(computeShaderProgram);
    GLint dtUniformLocation = glGetUniformLocation(computeShaderProgram, "dt");
    GLint resetPosUniformLocation = glGetUniformLocation(computeShaderProgram, "resetPos");
    glUniform3f(resetPosUniformLocation, e1->position->x, e1->position->y, e1->position->z);
    GLint maxParticlesUniformLocation = glGetUniformLocation(computeShaderProgram, "maxParticles");
    glUniform1ui(maxParticlesUniformLocation, PARTICLE_AMOUNT);

    GLuint renderShaders[] = { vertexShader, fragmentShader };
    GLuint renderShaderProgram = linkShaders(renderShaders, 2);

    float *particles = serializeParticlesystem(ps);
    GLsizeiptr sizeOfParticle = 3 * sizeof(vector3f) + sizeof(float);

    GLuint particleBuffer;
    glGenBuffers(1, &particleBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, PARTICLE_AMOUNT * sizeOfParticle, particles, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfParticle, (GLvoid *)0);
    // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeOfParticle, (GLvoid *)24); // 32
    glBindVertexArray(0);

    /************* RENDER LOOP *************/
    double time, tFrame, tLast = 0;
    while (!glfwWindowShouldClose(window))
    {
        time = glfwGetTime();
        tFrame = time - tLast;
        tLast = time;

        /*** UPDATE ***/
        glUseProgram(computeShaderProgram);
        glUniform1f(dtUniformLocation, tFrame);
        glDispatchCompute(PARTICLE_AMOUNT / 256, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

        /*** RENDER ***/
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glfwGetFramebufferSize(window, &width, &height);
        glBindVertexArray(vertexArray);
        glUseProgram(renderShaderProgram);
        glDrawArrays(GL_POINTS, 0, PARTICLE_AMOUNT);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //END
    deleteShaders(renderShaders, 2);
    deleteShaders(computeShaders, 1);

    terminateGLFW(window);
    freeParticleSystem(ps);

    return 0;
}