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

GLuint compileShader(const GLchar *shaderSource, GLenum shaderType)
{
    GLuint shader;
    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    int  compileSuccess;
    char info[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);
    if (!compileSuccess)
    {
        glGetShaderInfoLog(shader, 512, NULL, info);
        printf("ERROR::SHADER::%d::COMPILATION_FAILED:\n%s", shaderType, info);
    }

    return shader;
}

GLuint linkShaders(GLuint *shaders, GLsizei count)
{
    GLuint program;
    program = glCreateProgram();

    for (int i = 0; i < count; i++)
    {
        glAttachShader(program, shaders[i]);
    }

    glLinkProgram(program);

    int successLink;
    char infoLogLink[512];
    glGetProgramiv(program, GL_LINK_STATUS, &successLink);
    if (!successLink)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLogLink);
        printf("ERROR::SHADER::LINKING_FAILED\n%s", infoLogLink);
    }

    return program;
}

void deleteShaders(GLuint *shaders, GLsizei count)
{
    for (int i = 0; i < count; i++)
    {
        glDeleteShader(shaders[i]);
    }
}