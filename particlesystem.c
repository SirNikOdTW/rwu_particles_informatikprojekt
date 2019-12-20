#include <malloc.h>
#include <GLFW/glfw3.h>

#include "particlesystem.h"

/*
 * Initializes a particle
 */
particle *initParticle(vector3f *pos, vector3f *dir)
{
    particle *p = malloc(sizeof(particle));
    p->position = pos;
    p->direction = dir;
    return p;
}

/*
 * Initializes an emitter
 */
emitter *initEmitter(vector3f *pos, int pamount)
{
    emitter *e = malloc(sizeof(emitter));
    e->position = pos;
    e->particles = malloc(sizeof(particle *) * pamount);
    e->pamount = pamount;
    return e;
}

/*
 * Initializes a particle system
 */
particle_system *initParticleSystem(int eamount)
{
    particle_system *ps = malloc(sizeof(particle_system));
    ps->emitters = malloc(sizeof(emitter *) * eamount);
    ps->eamount = eamount;
    return ps;
}

/*
 * Updates particles
 */
int updateParticles(float dt, particle_system *ps)
{
    emitter *e;
    float t = 1 / dt;
    for (int i = 0; i < ps->eamount; i++)
    {
        e = (ps->emitters)[i];
        for (int j = 0; j < e->pamount; j++)
        {
            vector3f *p = (e->particles)[j]->position;
            vector3f *d = (e->particles)[j]->direction;
            p->x += d->x * t;
            p->y += d->y * t;
            p->z += d->z * t;
        }
    }
}

/*
 * Draws particles
 */
int drawParticles(particle_system *particleSystem)
{

}

/*
 * Initializes a vector
 * For that it allocates memory that must be freed later
 */
vector3f *initVector3f(float x, float y, float z)
{
    vector3f *vector = malloc(sizeof(vector3f));
    vector->x = x;
    vector->y = y;
    vector->z = z;
    return vector;
}

/*
 * Frees a given emitter and all corresponding particles
 */
void freeEmitter(emitter *e)
{
    for (int j = 0; j < e->pamount; j++)
    {
        free((e->particles)[j]);
    }

    free(e);
}

/*
 * Frees all emitters within a particle system
 */
void freeEmitters(particle_system *ps)
{
    for (int i = 0; i < ps->eamount; i++)
    {
        freeEmitter((ps->emitters)[i]);
    }
}

/*
 * Frees all emitters and particles within a particle system
 */
void freeParticleSystem(particle_system *ps)
{
    freeEmitters(ps);
    free(ps);
}
