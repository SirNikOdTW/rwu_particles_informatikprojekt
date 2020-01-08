#include <stdlib.h>
#include <stdio.h>

#include "initOpenGL.h"

void initGlad()
{
    if(!gladLoadGL()) {
        printf("Glad could not be loaded!\n");
        exit(EXIT_FAILURE);
    }
}

void initGLFW()
{
    if (!glfwInit())
    {
        printf("GLFW could not be initialized!\n");
        exit(EXIT_FAILURE);
    }
}

void setErrorCallbackGL()
{
    glfwSetErrorCallback(errorCallback);
}

void setFramebufferSizeCallbackGL(GLFWwindow *window)
{
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
}

void setCurrentContextGL(GLFWwindow *window)
{
    glfwMakeContextCurrent(window);
}

GLFWwindow *createGLFWWindow(int width, int height, char *title)
{
    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    return window;
}

void terminateGLFW(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void errorCallback(int error, const char* description)
{
    fputs(description, stderr);
}
void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void initVertexBuffer(unsigned int *VBO, float *vertices)
{
    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

