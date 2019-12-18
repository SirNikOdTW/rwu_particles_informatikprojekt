#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "particlesystem.h"

char *printVector(vector3f *v);
void printParticle(particle *v);
void printEmitter(emitter *e);
void initRandomParticles(emitter *e);
void error_callback(int error, const char* description);

int main()
{
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(error_callback);

    GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        glMatrixMode(GL_MODELVIEW);

        glLoadIdentity();
        glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);

        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(-0.6f, -0.4f, 0.f);
        glColor3f(0.f, 1.f, 0.f);
        glVertex3f(0.6f, -0.4f, 0.f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.6f, 0.f);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //END
    glfwDestroyWindow(window);
    glfwTerminate();

    /*vector3f *epos = initVector3f(0, 0, 0);
    emitter *e = initEmitter(epos, 10);

    initRandomParticles(e);

    printEmitter(e);

    for (int i = 0; i < e->pamount; i++)
    {
        free((e->particles)[i]);
    }

    free(epos);
    free(e);*/

    return 0;
}

void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

void initRandomParticles(emitter *e)
{
    for (int i = 0; i < e->pamount; i++)
    {
        vector3f *pos = initVector3f(e->position->x, e->position->y, e->position->z);
        vector3f *dir = initVector3f((float) rand() / RAND_MAX, (float) rand() / RAND_MAX, (float) rand() / RAND_MAX);
        (e->particles)[i] = initParticle(pos, dir);
    }
}

char *printVector(vector3f *v)
{
    char *c = malloc(100);
    sprintf(c, "(%f, %f, %f)", v->x, v->y, v->z);
    return c;
}

void printParticle(particle *v)
{
    printf("   Particle {\n");
    printf("      position = %s", printVector(v->position));
    printf("\n      direction = %s", printVector(v->direction));
    printf("\n   }");
}

void printEmitter(emitter *e)
{
    printf("Emitter {\n");

    for (int i = 0; i < e->pamount; i++)
    {
        printParticle((e->particles)[i]);
        printf("\n");
    }

    printf("\n}");
}
