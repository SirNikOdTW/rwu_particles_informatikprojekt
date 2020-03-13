#version 460 core
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable

struct particle
{
    vec3  pos;
    vec3  vel;
    vec3  col;
    float age;
};

layout(std430, binding = 0) buffer particles
{
    particle p[];
};

uniform float dt;
uniform vec3 resetPos;
uniform uint seed;

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

float rand()
{
    uint seedR;
    seedR = (seed * 1103515245u + 12345u);
    return float(seed) / 4294967296.0;
}

vec3 rand3() {
    vec3 result;
    uint seedR;
    seedR = (seed * 1103515245u + 12345u);
    result.x = float(seed);
    seedR = (seed * 1103515245u + 12345u);
    result.y = float(seed);
    seedR = (seed * 1103515245u + 12345u);
    result.z = float(seed);
    return (result / 2147483648.0) - vec3(1,1,1);
}

void main()
{
    uint gid = gl_GlobalInvocationID.x;
    particle part = p[gid];

    if (part.age < 0 || part.pos.x > 1 || part.pos.y > 1 || part.pos.z > 1
        || part.pos.x < -1 || part.pos.y < -1 || part.pos.z < -1 )
    {
        part.pos = resetPos;
        part.vel = rand3();
        part.col = vec3(rand(), rand(), rand());
        part.age = rand() * 0x7fff * 0.01f;

    }
    else
    {
        part.pos += part.vel * dt;
        part.age -= 0.01f;
    }

    p[gid] = part;
}
