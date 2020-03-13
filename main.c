#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <stddef.h>

#include "particlesystem.h"
#include "initOpenGL.h"

#define PARTICLE_AMOUNT 10000
#define WIDTH 800
#define HEIGHT 800

void printVector(vector3f *v);
void printParticle(particle *v);
void printEmitter(emitter *e);
void initRandomParticles(emitter *e);
void error_callback(int error, const char* description);

void calcPos(particle *p, float dt);
void calcCol(particle *p);

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
    const GLchar *computeShaderSource = "#version 460 core\n"
                                        "#extension GL_ARB_compute_shader : enable\n"
                                        "#extension GL_ARB_shader_storage_buffer_object : enable\n"
                                        "\n"
                                        "struct particle\n"
                                        "{\n"
                                        "    vec3  pos;\n"
                                        "    vec3  vel;\n"
                                        "    vec3  col;\n"
                                        "    float age;\n"
                                        "};\n"
                                        "\n"
                                        "layout(std430, binding = 0) buffer particles\n"
                                        "{\n"
                                        "    particle p[];\n"
                                        "};\n"
                                        "\n"
                                        "uniform float dt;\n"
                                        "uniform vec3 resetPos;\n"
                                        "uniform uint seed;\n"
                                        "\n"
                                        "layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;\n"
                                        "\n"
                                        "float rand()\n"
                                        "{\n"
                                        "    uint seedR;\n"
                                        "    seedR = (seed * 1103515245u + 12345u);\n"
                                        "    return float(seed) / 4294967296.0;\n"
                                        "}\n"
                                        "\n"
                                        "vec3 rand3() {\n"
                                        "    vec3 result;\n"
                                        "    uint seedR;\n"
                                        "    seedR = (seed * 1103515245u + 12345u);\n"
                                        "    result.x = float(seed);\n"
                                        "    seedR = (seed * 1103515245u + 12345u);\n"
                                        "    result.y = float(seed);\n"
                                        "    seedR = (seed * 1103515245u + 12345u);\n"
                                        "    result.z = float(seed);\n"
                                        "    return (result / 2147483648.0) - vec3(1,1,1);\n"
                                        "}\n"
                                        "\n"
                                        "void main()\n"
                                        "{\n"
                                        "    uint gid = gl_GlobalInvocationID.x;\n"
                                        "    particle part = p[gid];\n"
                                        "\n"
                                        "    if (part.age < 0 || part.pos.x > 1 || part.pos.y > 1 || part.pos.z > 1\n"
                                        "        || part.pos.x < -1 || part.pos.y < -1 || part.pos.z < -1 )\n"
                                        "    {\n"
                                        "        part.pos = resetPos;\n"
                                        "        //part.vel = rand3();\n"
                                        "        //part.col = vec3(rand(), rand(), rand());\n"
                                        "        //part.age = rand() * 0x7fff * 0.01f;\n"
                                        "        \n"
                                        "    }\n"
                                        "    else\n"
                                        "    {\n"
                                        "        part.pos += part.vel * dt;\n"
                                        "        part.age -= 0.01f;\n"
                                        "    }\n"
                                        "\n"
                                        "    p[gid] = part;\n"
                                        "}";

    const GLchar *vertexShaderSource = "#version 460\n"
                                       "\n"
                                       "layout(location = 0) in vec3 pos;\n"
                                       "layout(location = 1) in vec3 colIn;\n"
                                       "\n"
                                       "out vec3 colV;\n"
                                       "\n"
                                       "void main(void)\n"
                                       "{\n"
                                       "    colV = colIn;\n"
                                       "    gl_Position = vec4(pos, 1);\n"
                                       "}";

    const GLchar *fragmentShaderSource = "#version 460 core\n"
                                         "\n"
                                         "in vec3 colV;\n"
                                         "out vec4 colOut;\n"
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
    GLint seedUniformLocation = glGetUniformLocation(computeShaderProgram, "seed");
    glUniform3f(resetPosUniformLocation, e1->position->x, e1->position->y, e1->position->z);

    GLuint renderShaders[] = { vertexShader, fragmentShader };
    GLuint renderShaderProgram = linkShaders(renderShaders, 2);

    float *particles = serializeParticlesystem(ps);
    GLsizeiptr sizeOfParticle = 3 * sizeof(vector3f) + sizeof(float);

    GLuint particleBuffer;
    glGenBuffers(1, &particleBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, PARTICLE_AMOUNT * sizeOfParticle, particles, GL_STATIC_DRAW);
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
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeOfParticle, (GLvoid *)24);
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
        glUniform1ui(seedUniformLocation, rand());
        glDispatchCompute((PARTICLE_AMOUNT / 256) + 1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

        /*** RENDER ***/
        glClear(GL_COLOR_BUFFER_BIT);
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

void calcPos(particle *p, float dt)
{
    p->position->x += p->velocity->x * dt;
    p->position->y += p->velocity->y * dt;
    p->position->z += p->velocity->z * dt;
}

void calcCol(particle *p)
{
//    p->color->x -= 0.01f;
//    p->color->y -= 0.01f;
//    p->color->z -= 0.01f;
}

/*************************************************************************************************************/
void initRandomParticles(emitter *e)
{
    for (int i = 0; i < e->pamount; i++)
    {
        vector3f *pos = initVector3f(e->position->x, e->position->y, e->position->z);
        vector3f *dir = initVector3f(((float) (rand()%2 ? -1 : 1) * rand()) / RAND_MAX,
                                     ((float) (rand()%2 ? -1 : 1) * rand()) / RAND_MAX,
                                     ((float) (rand()%2 ? -1 : 1) * rand()) / RAND_MAX);
        vector3f *color = initVector3f(((float) (rand() % 255)) / 255,
                                       ((float) (rand() % 255)) / 255,
                                       ((float) (rand() % 255)) / 255);
        (e->particles)[i] = initParticle(pos, dir, color, rand() / 100.0f);
    }
}

void printParticle(particle *v)
{
    printVector(v->position);
    printf("; ");
    printVector(v->velocity);
    printf("; ");
    printVector(v->color);
    printf("; %f\n", v->age);
}

void printVector(vector3f *v)
{
    printf("(%f, %f, %f)", v->x, v->y, v->z);
}

/// VERY OLD
//        glUseProgram(shaderProgram);
//        glBindVertexArray(vao);
//        glDrawArrays(GL_POINTS, 0, particleAmount);

//        updateParticles((float) tFrame, ps, calcPos, calcCol);
//
//        emitter *e;
//        particle *p;
//        vector3f *pos;
//        for (int j = 0; j < ps->eamount; j++)
//        {
//            e = (ps->emitters)[j];
//            for (int i = 0; i < e->pamount; i++)
//            {
//                p = (e->particles)[i];
//                pos = p->position;
//
//                glColor3f(p->color->x, p->color->y, p->color->z);
//                glBegin(GL_POINTS);
//                glVertex3f(pos->x, pos->y, pos->z);
//                glEnd();
//            }
//        }

/// FEEDBACK TRANSFORM BEFORE
//    /*************** NEW ***************/
//    float *vertexData = serializeParticlesystem(ps);
//    unsigned int currentVertexBuffer = 0, currentTransformFeedbackBuffer = 1;
//    int buffersSize = 2;
//    unsigned int particleBuffers[buffersSize], transformFeedbackBuffers[buffersSize];
//
//    glGenTransformFeedbacks(buffersSize, transformFeedbackBuffers);
//    glGenBuffers(buffersSize, particleBuffers);
//
//    for (int i = 0; i < buffersSize; i++)
//    {
//        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedbackBuffers[i]);
//        glBindBuffer(GL_ARRAY_BUFFER, particleBuffers[i]);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_DYNAMIC_DRAW);
//        glBindBufferBase(GL_TRANSFORM_FEEDBACK, 0, particleBuffers[i]);
//    }

//    // position attribute
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
//    // velocity attribute
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
//    // color attribute
//    glEnableVertexAttribArray(2);
//    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(6 * sizeof(float)));
//    // age attribute
//    glEnableVertexAttribArray(3);
//    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(9 * sizeof(float)));

/// TRANSFORM FEEDBACK RENDER LOOP
///*** UPDATE PARTICLES ***/
//glEnable(GL_RASTERIZER_DISCARD);
//glBindBuffer(GL_ARRAY_BUFFER, particleBuffers[currentVertexBuffer]);
//glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedbackBuffers[currentTransformFeedbackBuffer]);
//glBeginTransformFeedback(GL_POINTS);
//if (isFirst)
//{
//glDrawArrays(GL_POINTS, 0, particleAmount);
//isFirst = !isFirst;
//}
//else
//{
//glDrawTransformFeedback(GL_POINTS, transformFeedbackBuffers[currentVertexBuffer]);
//}
//glEndTransformFeedback();
//
///*** RENDER PARTICLES ***/
//glDisable(GL_RASTERIZER_DISCARD);
//glBindBuffer(GL_ARRAY_BUFFER, particleBuffers[currentTransformFeedbackBuffer]);
//glDrawTransformFeedback(GL_POINTS, transformFeedbackBuffers[currentTransformFeedbackBuffer]);
//
///***************************************************/
//currentVertexBuffer = currentTransformFeedbackBuffer;
//currentTransformFeedbackBuffer = !currentTransformFeedbackBuffer;