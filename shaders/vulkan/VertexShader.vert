#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 colIn;

layout(location = 0) out vec3 colV;

void main(void)
{
    colV = colIn;
    gl_PointSize = 1.0f;
    gl_Position = vec4(pos, 1);
}