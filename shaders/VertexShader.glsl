#version 460 core

layout (location = 0) in vec3 pos;   // the position variable has attribute position 0
layout (location = 1) in vec3 dir; // the direction variable has attribute position 1
layout (location = 2) in vec3 col; // the color variable has attribute position 2
layout (location = 3) in float age; // the age variable has attribute position 3

//in vec3 emitterPos; // the emitter pos variable
//in float newAge; // the age variable

out vec3 outCol; // output a color to the fragment shader

void main()
{
    if (age < 0)
    {
        pos = emitterPos;
        age = newAge;
    }

    age -= 0.1f;
    vec3 newPos = pos.xyz + dir.xyz;
    gl_Position = vec4(newPos, 1.0);

    outCol = col; // set ourColor to the input color we got from the vertex data
}