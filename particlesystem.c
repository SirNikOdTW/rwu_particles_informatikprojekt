#include <malloc.h>
#include <stdlib.h>

#include "particlesystem.h"

/*
 * Initializes a particle
 */
particle *initParticle(vector3f *pos, vector3f *dir, vector3f *color, float age)
{
    particle *p = malloc(sizeof(particle));
    p->position = pos;
    p->velocity = dir;
    p->color = color;
    p->age = age;
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
int updateParticles(float dt, particle_system *ps, CalculatePositionFunction calculatePosition, CalculateColorFunction calculateColor)
{
    emitter *e;
    particle *p;
    for (int i = 0; i < ps->eamount; i++)
    {
        e = (ps->emitters)[i];
        for (int j = 0; j < e->pamount; j++)
        {
            p = (e->particles)[j];

            if (p->age < 0)
            {
                resetParticle(e, p);
            }
            else
            {
                calculatePosition(p, dt);
                calculateColor(p);
                p->age -= 0.1f;
            }
        }
    }
}

void resetParticle(emitter *e, particle *p)
{
    p->position->x = e->position->x;
    p->position->y = e->position->y;
    p->position->z = e->position->z;

    p->velocity->x = ((float) (rand() % 2 ? -1 : 1) * rand()) / RAND_MAX;
    p->velocity->y = ((float) (rand() % 2 ? -1 : 1) * rand()) / RAND_MAX;
    p->velocity->z = ((float) (rand() % 2 ? -1 : 1) * rand()) / RAND_MAX;

    p->age = rand() / 10;
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
 * Frees a given particle and all corresponding data
 */
void freeParticle(particle *p)
{
    free(p->position);
    free(p->velocity);
    free(p->color);
    free(p);
}

/*
 * Frees a given emitter and all corresponding particles
 */
void freeEmitter(emitter *e)
{
    for (int j = 0; j < e->pamount; j++)
    {
        freeParticle((e->particles)[j]);
    }

    free(e->position);
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

/*
 * Creates float array out of a particle system
 */
float *serializeParticlesystem(particle_system *ps)
{
    int particleAmount = 0;
    for (int i = 0; i < ps->eamount; i++)
    {
        particleAmount += (ps->emitters[i])->pamount;
    }

    unsigned long particleBytesAmount = sizeof(vector3f) * 3 + sizeof(float);
    float *vert = malloc(particleBytesAmount * particleAmount);

    emitter *e;
    particle *p;
    for (int y = 0, j = 0; y < ps->eamount; y++)
    {
        e = (ps->emitters)[y];
        for (int x = 0; x < e->pamount; x++)
        {
            p = e->particles[x];
            // Position
            vert[j++] = p->position->x;
            vert[j++] = p->position->y;
            vert[j++] = p->position->z;

            // Direction
            vert[j++] = p->velocity->x;
            vert[j++] = p->velocity->y;
            vert[j++] = p->velocity->z;

            // Color
            vert[j++] = p->color->x;
            vert[j++] = p->color->y;
            vert[j++] = p->color->z;

            // Age
            vert[j++] = p->age;
        }
    }

    return vert;
}
