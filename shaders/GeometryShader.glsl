#version 460 core

layout(points) in;
layout(points, max_vertices = 256) out;

in vec3 colV;
out vec3 colG;

void main(void)
{
    colG = colV;
}
