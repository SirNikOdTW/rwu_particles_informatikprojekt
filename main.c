#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#include "particlesystem.h"
#include "initOpenGL.h"

char *printVector(vector3f *v);
void printParticle(particle *v);
void printEmitter(emitter *e);
void initRandomParticles(emitter *e);
void error_callback(int error, const char* description);

void calcPos(particle *p, float dt);
void calcCol(particle *p);

int main()
{
    // Init OpenGL and GLFW
    initGLFW();
    setErrorCallbackGL();

    int width = 800, height = 800;
    GLFWwindow *window = createGLFWWindow(width, height, "Informatikprojekt - OpenGL");

    setCurrentContextGL(window);
    setFramebufferSizeCallbackGL(window);

    // glad
    initGlad();

    vector3f *epos = initVector3f(0, 0, 0);
    emitter *e = initEmitter(epos, 1000);
    particle_system *ps = initParticleSystem(1);
    (ps->emitters)[0] = e;

    initRandomParticles(e);

    double time, tFrame, tLast = 0;
    unsigned int vbo;
    float vert[3] = {0.1f, 0.2f, 0.3f};

    while (!glfwWindowShouldClose(window))
    {
        time = glfwGetTime();
        tFrame = time - tLast;
        tLast = time;

        glClear(GL_COLOR_BUFFER_BIT);
        glfwGetFramebufferSize(window, &width, &height);

        initVertexBuffer(&vbo, vert);

        updateParticles((float) tFrame, ps, calcPos, calcCol);

        particle *p;
        vector3f *pos;
        for (int i = 0; i < e->pamount; i++)
        {
            p = (e->particles)[i];
            glColor3f(p->color->x, p->color->y, p->color->z);
            pos = p->position;

            glBegin(GL_POINTS);
            glVertex3f(pos->x, pos->y, pos->z);
            glEnd();

            /*glBegin(GL_QUADS);
            glVertex3f(pos->x, pos->y, pos->z);
            glVertex3f(pos->x+.01, pos->y, pos->z);
            glVertex3f(pos->x+.01, pos->y-.01, pos->z);
            glVertex3f(pos->x, pos->y-.01, pos->z);
            glEnd();*/
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //END
    terminateGLFW(window);

    free(epos);
    freeParticleSystem(ps);

    return 0;
}

void calcPos(particle *p, float dt)
{
    p->position->x += p->direction->x * dt;
    p->position->y += p->direction->y * dt;
    p->position->z += p->direction->z * dt;
}

void calcCol(particle *p)
{
    p->color->x = 1;
    p->color->y = 1;
    p->color->z = 1;
}

/*************************************************************************************************************/
void initRandomParticles(emitter *e)
{
    for (int i = 0; i < e->pamount; i++)
    {
        vector3f *pos = initVector3f(e->position->x, e->position->y, e->position->z);
        vector3f *dir = initVector3f(((float) (rand()%2 ? -1 : 1) * rand()) / RAND_MAX,
                                     ((float) (rand()%2 ? -1 : 1) * rand()) / RAND_MAX,
                                     ((float) (rand()%2 ? -1 : 1) * rand()) / RAND_MAX);
        vector3f *color = initVector3f(1, 1, 1);
        (e->particles)[i] = initParticle(pos, dir, color, rand() / 100.0f);
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
