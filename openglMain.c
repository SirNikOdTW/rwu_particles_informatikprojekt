#include <stdlib.h>
#include "particlesystem.h"
#include "initOpenGL.h"
#include "utils.h"

#define PARTICLE_AMOUNT 1000000

int main()
{
    /************* INIT *************/
    // Init OpenGL and GLFW
    initGLFW();
    setErrorCallbackGL();

    int width = WIDTH, height = HEIGHT;
    GLFWwindow *window = createGLFWWindow(WIDTH, HEIGHT, "Informatikprojekt - OpenGL");

    setCurrentContextGL(window);
    setFramebufferSizeCallbackGL(window);

    // glad
    initGlad();

    /************* PARTICLE SYSTEM *************/
    vector3f *epos1 = initVector3f(0, 0, 0);
    emitter *e1 = initEmitter(epos1, PARTICLE_AMOUNT);
    particle_system *ps = initParticleSystem(1);
    (ps->emitters)[0] = e1;
    initRandomParticles(e1);

    /************* SHADER *************/
    const GLchar *computeShaderSource = "#version 460\n"
                                        "\n"
                                        "#define FLOAT_MAX 4294967296.0f\n"
                                        "#define FLOAT_FACTOR 0.00000000023283064365386962890625f\n"
                                        "\n"
                                        "struct particle\n"
                                        "{\n"
                                        "    float px, py, pz;\n"
                                        "    float vx, vy, vz;\n"
                                        "    float cx, cy, cz;\n"
                                        "    float age;\n"
                                        "};\n"
                                        "\n"
                                        "layout(std430, binding = 0) buffer particles\n"
                                        "{\n"
                                        "    particle p[];\n"
                                        "};\n"
                                        "\n"
                                        "layout(location = 0) uniform float dt;\n"
                                        "layout(location = 1) uniform vec3 resetPos;\n"
                                        "layout(location = 2) uniform uint maxParticles;\n"
                                        "\n"
                                        "layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;\n"
                                        "\n"
                                        "uint hash(uvec3 seed)\n"
                                        "{\n"
                                        "    uint hash;\n"
                                        "    hash = (seed.x ^ 61u) ^ (seed.y >> 16u);\n"
                                        "    hash *= 9u;\n"
                                        "    hash = seed.z ^ (seed.x >> 4u);\n"
                                        "    hash *= 0x27d4eb2du;\n"
                                        "    hash = seed.y ^ (seed.z >> 15u);\n"
                                        "    return hash;\n"
                                        "}\n"
                                        "\n"
                                        "uint rand(uint seed)\n"
                                        "{\n"
                                        "    seed ^= (seed << 13u);\n"
                                        "    seed ^= (seed >> 17u);\n"
                                        "    seed ^= (seed << 5u);\n"
                                        "    return seed;\n"
                                        "}\n"
                                        "\n"
                                        "int foreSign(uint seed)\n"
                                        "{\n"
                                        "    return rand(seed) % 2 == 0 ? 1 : -1;\n"
                                        "}\n"
                                        "\n"
                                        "void main()\n"
                                        "{\n"
                                        "    uint gid = gl_GlobalInvocationID.x;\n"
                                        "\n"
                                        "    if (gid <= maxParticles)\n"
                                        "    {\n"
                                        "        particle part = p[gid];\n"
                                        "\n"
                                        "        uint hash1 = hash(uvec3(uint(part.px * FLOAT_MAX), uint(part.cy * FLOAT_MAX), uint(part.vz * FLOAT_MAX)));\n"
                                        "        uint hash2 = hash(uvec3(uint(part.vx * FLOAT_MAX), uint(part.py * FLOAT_MAX), uint(part.cz * FLOAT_MAX)));\n"
                                        "        uint hash3 = hash(uvec3(uint(part.cx * FLOAT_MAX), uint(part.vy * FLOAT_MAX), uint(part.pz * FLOAT_MAX)));\n"
                                        "\n"
                                        "        if (part.age < 0 || part.px > 1 || part.py > 1 || part.pz > 1 || part.px < -1 || part.py < -1 || part.pz < -1)\n"
                                        "        {\n"
                                        "            part.px = resetPos.x;\n"
                                        "            part.py = resetPos.y;\n"
                                        "            part.pz = resetPos.z;\n"
                                        "\n"
                                        "            part.age = rand(hash(uvec3(hash1, hash2, hash3))) % (250 - 60 + 1) + 60;\n"
                                        "\n"
                                        "            part.vx = foreSign(hash1) * float(rand(hash2)) * FLOAT_FACTOR;\n"
                                        "            part.vy = foreSign(hash3) * float(rand(hash1)) * FLOAT_FACTOR;\n"
                                        "            part.vz = foreSign(hash2) * float(rand(hash3)) * FLOAT_FACTOR;\n"
                                        "\n"
                                        "            part.cx = float(rand(hash1 ^ hash2)) * FLOAT_FACTOR;\n"
                                        "            part.cy = float(rand(hash2 ^ hash3)) * FLOAT_FACTOR;\n"
                                        "            part.cz = float(rand(hash3 ^ hash1)) * FLOAT_FACTOR;\n"
                                        "        }\n"
                                        "        else\n"
                                        "        {\n"
                                        "            part.px += part.vx * dt;\n"
                                        "            part.py += part.vy * dt;\n"
                                        "            part.pz += part.vz * dt;\n"
                                        "\n"
                                        "            part.age -= 0.01f;\n"
                                        "        }\n"
                                        "\n"
                                        "        p[gid] = part;\n"
                                        "    }\n"
                                        "}";

    const GLchar *vertexShaderSource = "#version 460\n"
                                       "\n"
                                       "layout(location = 0) in vec3 pos;\n"
                                       "layout(location = 1) in vec3 colIn;\n"
                                       "\n"
                                       "layout(location = 0) out vec3 colV;\n"
                                       "\n"
                                       "void main(void)\n"
                                       "{\n"
                                       "    colV = colIn;\n"
                                       "    gl_Position = vec4(pos, 1);\n"
                                       "}";

    const GLchar *fragmentShaderSource = "#version 460\n"
                                         "\n"
                                         "layout(location = 0) in vec3 colV;\n"
                                         "layout(location = 0) out vec4 colOut;\n"
                                         "\n"
                                         "void main(void)\n"
                                         "{\n"
                                         "    colOut = vec4(colV, 1);\n"
                                         "}";

    GLuint computeShader = compileShader(computeShaderSource, GL_COMPUTE_SHADER);
    GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    GLuint computeShaders[] = { computeShader };
    GLuint computeShaderProgram = linkShaders(computeShaders, 1);
    glUseProgram(computeShaderProgram);
    GLint dtUniformLocation = glGetUniformLocation(computeShaderProgram, "dt");
    GLint resetPosUniformLocation = glGetUniformLocation(computeShaderProgram, "resetPos");
    glUniform3f(resetPosUniformLocation, e1->position->x, e1->position->y, e1->position->z);
    GLint maxParticlesUniformLocation = glGetUniformLocation(computeShaderProgram, "maxParticles");
    glUniform1ui(maxParticlesUniformLocation, PARTICLE_AMOUNT);

    GLuint renderShaders[] = { vertexShader, fragmentShader };
    GLuint renderShaderProgram = linkShaders(renderShaders, 2);

    float *particles = serializeParticlesystem(ps);
    GLsizeiptr sizeOfParticle = 3 * sizeof(vector3f) + sizeof(float);

    GLuint particleBuffer;
    glGenBuffers(1, &particleBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, PARTICLE_AMOUNT * sizeOfParticle, particles, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfParticle, (GLvoid *)0);
    // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeOfParticle, (GLvoid *)24); // 32
    glBindVertexArray(0);

    /************* RENDER LOOP *************/
    double time, tFrame, tLast = 0;
    while (!glfwWindowShouldClose(window))
    {
        time = glfwGetTime();
        tFrame = time - tLast;
        tLast = time;

        /*** UPDATE ***/
        glUseProgram(computeShaderProgram);
        glUniform1f(dtUniformLocation, tFrame);
        glDispatchCompute(PARTICLE_AMOUNT / 256, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

        /*** RENDER ***/
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glfwGetFramebufferSize(window, &width, &height);
        glBindVertexArray(vertexArray);
        glUseProgram(renderShaderProgram);
        glDrawArrays(GL_POINTS, 0, PARTICLE_AMOUNT);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //END
    deleteShaders(renderShaders, 2);
    deleteShaders(computeShaders, 1);

    terminateGLFW(window);
    freeParticleSystem(ps);

    return 0;
}