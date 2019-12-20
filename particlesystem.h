/*
 * A vector with three floats
 */
typedef struct vector3f
{
    float x;
    float y;
    float z;
} vector3f;

/*
 * A particle has a position and a direction
 */
typedef struct particle
{
    vector3f *position;
    vector3f *direction;
} particle;

/*
 * An emitter has a position and contains an array of particles
 */
typedef struct emitter
{
    vector3f *position;
    particle **particles;
    int pamount;
} emitter;

/*
 * A particle system consists of one or more emitter
 */
typedef struct particle_system
{
    emitter **emitters;
    int eamount;
} particle_system;

/*
 * Initializes a particle
 */
particle *initParticle(vector3f *pos, vector3f *dir);

/*
 * Initializes an emitter
 */
emitter *initEmitter(vector3f *pos, int pamount);

/*
 * Initializes a particle system
 */
particle_system *initParticleSystem(int eamount);

/*
 * Updates particle
 */
int updateParticles(float dt, particle_system *particleSystem);

/*
 * Updates particle
 */
int drawParticles(particle_system *particleSystem);

/*
 * Initializes a vector
 * For that it allocates memory that must be freed later
 */
vector3f *initVector3f(float x, float y, float z);

/*
 * Frees an given emitter with all particles
 */
void freeEmitter(emitter *e);

/*
 * Frees all emitters within an particle system
 */
void freeEmitters(particle_system *ps);

/*
 * Frees all emitter and particles within a particle system
 */
void freeParticleSystem(particle_system *ps);