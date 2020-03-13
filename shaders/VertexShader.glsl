#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 colIn;

out vec3 colV;

void main(void)
{
    colV = colIn;
    gl_Position = vec4(pos, 0);
}