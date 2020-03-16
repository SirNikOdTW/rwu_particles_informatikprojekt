#include "particlesystem.h"
#include "initOpenGL.h"
#include "def.h"

#define PARTICLE_AMOUNT 1000

void calcPos(particle *p, float dt);
void calcCol(particle *p);

int main()
{
    /************* INIT *************/
    // Init OpenGL and GLFW
    initGLFW();
    setErrorCallbackGL();

    int width = WIDTH, height = HEIGHT;
    GLFWwindow *window = createGLFWWindow(WIDTH, HEIGHT, "Informatikprojekt - OpenGL CPU");

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

    /************* RENDER LOOP *************/
    double time, tFrame, tLast = 0;
    while (!glfwWindowShouldClose(window))
    {
        time = glfwGetTime();
        tFrame = time - tLast;
        tLast = time;

        /*** UPDATE ***/
        updateParticles((float) tFrame, ps, calcPos, calcCol);

        /*** RENDER ***/
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glfwGetFramebufferSize(window, &width, &height);

        emitter *e;
        particle *p;
        vector3f *pos;
        for (int j = 0; j < ps->eamount; j++)
        {
            e = (ps->emitters)[j];
            for (int i = 0; i < e->pamount; i++)
            {
                p = (e->particles)[i];
                pos = p->position;

                glColor3f(p->color->x, p->color->y, p->color->z);
                glBegin(GL_POINTS);
                glVertex3f(pos->x, pos->y, pos->z);
                glEnd();
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //END
    terminateGLFW(window);
    freeParticleSystem(ps);

    return 0;
}

void calcPos(particle *p, float dt)
{
    p->position->x += p->velocity->x * dt;
    p->position->y += p->velocity->y * dt;
    p->position->z += p->velocity->z * dt;
}

void calcCol(particle *p)
{
    p->color->x -= 0.00001f;
    p->color->y -= 0.00001f;
    p->color->z -= 0.00001f;
}
