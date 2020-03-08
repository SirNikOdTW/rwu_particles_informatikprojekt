#version 460 core

layout (location = 0) in vec3 pos;   // the position variable has attribute position 0
layout (location = 1) in vec3 vel; // the velocity variable has attribute position 1
layout (location = 2) in vec3 col; // the color variable has attribute position 2
layout (location = 3) in float age; // the age variable has attribute position 3

out vec3 pos0;
out vec3 vel0;
out vec3 col0;
out float age0;

void main()
{
    pos0 = pos;
    vel0 = vel;
    col0 = col;
    age0 = age;
}