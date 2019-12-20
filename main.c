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

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "My Title", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    vector3f *epos = initVector3f(0, 0, 0);
    emitter *e = initEmitter(epos, 1000);
    particle_system *ps = initParticleSystem(1);
    (ps->emitters)[0] = e;

    initRandomParticles(e);

    while (!glfwWindowShouldClose(window))
    {
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);

        int b = 0;
        for (int i = 0; i < e->pamount; i++)
        {
            vector3f *p = (e->particles)[i]->position;
            if (p->x > 1 || p->x < -1 || p->y > 1 || p->y < -1 || p->z > 1 || p->z < -1)
            {
                b=1;
                break;
            }
        }

        if (!b)
        {
            updateParticles(4000, ps);
        }

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_POINTS);
        for (int i = 0; i < e->pamount; i++)
        {

            glColor3f((float) rand() / RAND_MAX, (float) rand() / RAND_MAX, (float) rand() / RAND_MAX);
            vector3f *p = (e->particles)[i]->position;
            glVertex3f(p->x, p->y, p->z);
        }
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //END
    glfwDestroyWindow(window);
    glfwTerminate();

    free(epos);
    freeEmitter(e);

    return 0;
}

void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

/*************************/
float rv()
{
    int i = rand()%2 ? -1 : 1;
    return (float) i * rand() / RAND_MAX;
}
/*************************/

void initRandomParticles(emitter *e)
{
    for (int i = 0; i < e->pamount; i++)
    {
        vector3f *pos = initVector3f(e->position->x, e->position->y, e->position->z);
        vector3f *dir = initVector3f(rv(), rv(), rv());
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
