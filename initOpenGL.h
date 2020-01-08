
#include "glad/glad.h"

#include <GLFW/glfw3.h>

#ifndef INFORMATIKPROJEKT_INITOPENGL_H
#define INFORMATIKPROJEKT_INITOPENGL_H

void initGlad();

void initGLFW();
void setErrorCallbackGL();
void setFramebufferSizeCallbackGL(GLFWwindow *window);
void setCurrentContextGL(GLFWwindow *window);
GLFWwindow *createGLFWWindow(int width, int height, char *title);
void terminateGLFW(GLFWwindow *window);

void errorCallback(int error, const char* description);
void framebufferSizeCallback(GLFWwindow *window, int width, int height);

void initVertexBuffer(unsigned int *VBO,float *vertices);

#endif //INFORMATIKPROJEKT_INITOPENGL_H
