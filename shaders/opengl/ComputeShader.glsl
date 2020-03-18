#version 460

#define FLOAT_MAX 4294967296.0f
#define FLOAT_FACTOR 0.00000000023283064365386962890625f

struct particle
{
    float px, py, pz;
    float vx, vy, vz;
    float cx, cy, cz;
    float age;
};

layout(std430, binding = 0) buffer particles
{
    particle p[];
};

layout(location = 0) uniform float dt;
layout(location = 1) uniform vec3 resetPos;
layout(location = 2) uniform uint maxParticles;

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

uint hash(uvec3 seed)
{
    uint hash;
    hash = (seed.x ^ 61u) ^ (seed.y >> 16u);
    hash *= 9u;
    hash = seed.z ^ (seed.x >> 4u);
    hash *= 0x27d4eb2du;
    hash = seed.y ^ (seed.z >> 15u);
    return hash;
}

uint rand(uint seed)
{
    seed ^= (seed << 13u);
    seed ^= (seed >> 17u);
    seed ^= (seed << 5u);
    return seed;
}

int foreSign(uint seed)
{
    return rand(seed) % 2 == 0 ? 1 : -1;
}

void main()
{
    uint gid = gl_GlobalInvocationID.x;

    if (gid <= maxParticles)
    {
        particle part = p[gid];

        uint hash1 = hash(uvec3(uint(part.px * FLOAT_MAX), uint(part.cy * FLOAT_MAX), uint(part.vz * FLOAT_MAX)));
        uint hash2 = hash(uvec3(uint(part.vx * FLOAT_MAX), uint(part.py * FLOAT_MAX), uint(part.cz * FLOAT_MAX)));
        uint hash3 = hash(uvec3(uint(part.cx * FLOAT_MAX), uint(part.vy * FLOAT_MAX), uint(part.pz * FLOAT_MAX)));

        if (part.age < 0 || part.px > 1 || part.py > 1 || part.pz > 1 || part.px < -1 || part.py < -1 || part.pz < -1)
        {
            part.px = resetPos.x;
            part.py = resetPos.y;
            part.pz = resetPos.z;

            part.age = rand(hash(uvec3(hash1, hash2, hash3))) % (250 - 60 + 1) + 60;

            part.vx = foreSign(hash1) * float(rand(hash2)) * FLOAT_FACTOR;
            part.vy = foreSign(hash3) * float(rand(hash1)) * FLOAT_FACTOR;
            part.vz = foreSign(hash2) * float(rand(hash3)) * FLOAT_FACTOR;

            part.cx = float(rand(hash1 ^ hash2)) * FLOAT_FACTOR;
            part.cy = float(rand(hash2 ^ hash3)) * FLOAT_FACTOR;
            part.cz = float(rand(hash3 ^ hash1)) * FLOAT_FACTOR;
        }
        else
        {
            part.px += part.vx * dt;
            part.py += part.vy * dt;
            part.pz += part.vz * dt;

            part.age -= 0.01f;
        }

        p[gid] = part;
    }
}
