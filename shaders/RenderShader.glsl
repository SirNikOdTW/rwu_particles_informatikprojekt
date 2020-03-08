#version 460 core

layout (points) in;
layout (points) out;
layout (max_vertices = 40) out;

in vec3 pos0[];
in vec3 vel0[];
in vec3 col0[];
in float age0[];

out vec3 posOut;
out vec3 velOut;
out vec3 colOut;
out float ageOut;

void main()
{
    if (age0[0] < 0)
    {
        posOut = vec3(0, 0, 0);
    }
    else
    {
        posOut = pos0[0];
        velOut = vel0[0];
        colOut = col0[0];
        ageOut = age0[0] - 0.01f;
    }
}
