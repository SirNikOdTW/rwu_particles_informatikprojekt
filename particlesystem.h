/*
 * A vector with three floats
 */
struct vector3f
{
    float x;
    float y;
    float z;
};

/*
 * A particle has a position and a direction
 */
struct particle
{
    struct vector3f position;
    struct vector3f direction;
};

/*
 * An emitter has a position and contains an array of particles
 */
struct emitter
{
    struct vector3f position;
    struct particle *particles;
    int pamount;
};

/*
 * A particle system consists of one or more emitter
 */
struct particle_system
{
    struct emitter *emitters;
    int eamount;
};

/*
 *
 */
int initParticle(struct vector3f pos, struct vector3f dir);

/*
 *
 */
int initVector3f(int x, int y, int z);


