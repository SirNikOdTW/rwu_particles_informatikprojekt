#include "particlesystem.h"
#include <malloc.h>

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
int updateParticles(float dt, particle_system *particleSystem)
{

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