#version 460 core

in vec3 colV;
out vec4 colOut;

void main(void)
{
    colOut = vec4(colV, 1);
}