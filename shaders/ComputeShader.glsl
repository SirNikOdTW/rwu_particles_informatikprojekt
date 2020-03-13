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

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

void main()
{
    uint gid = gl_GlobalInvocationID.x;
    particle part = p[gid];

    if (part.age > 0 || part.pos.x > 1 || part.pos.y > 1 || part.pos.z > 1)
    {
        part.pos += part.vel * dt;
        part.age -= 0.01f;
    }
    else
    {
        part.pos = vec3(0, 0, 0);
    }

    p[gid] = part;
}
