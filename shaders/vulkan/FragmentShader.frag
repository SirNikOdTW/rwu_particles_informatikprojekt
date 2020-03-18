#version 460

layout(location = 0) in vec3 colV;
layout(location = 0) out vec4 colOut;

void main(void)
{
    colOut = vec4(colV, 1);
}
