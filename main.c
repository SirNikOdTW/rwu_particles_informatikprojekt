#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include "particlesystem.h"

char *printVector(vector3f *v);
void printParticle(particle *v);
void printEmitter(emitter *e);
void initRandomParticles(emitter *e);

int main()
{
    vector3f *epos = initVector3f(0, 0, 0);
    emitter *e = initEmitter(epos, 10);

    initRandomParticles(e);

    printEmitter(e);

    for (int i = 0; i < e->pamount; i++)
    {
        free((e->particles)[i]);
    }

    free(epos);
    free(e);
    return 0;
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
