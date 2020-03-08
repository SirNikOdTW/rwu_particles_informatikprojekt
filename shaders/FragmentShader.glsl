#version 460 core

in vec3 col;
out vec4 colOut;

void main(void)
{
    colOut = vec4(col, 1);
}