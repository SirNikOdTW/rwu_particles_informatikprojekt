#version 460 core

in vec3 col; // the input variable from the vertex shader (same name and same type)

out vec4 outCol;

void main()
{
    outCol = vec4(vertexColor, 1.0);
}