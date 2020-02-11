#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#include "particlesystem.h"
#include "initOpenGL.h"

char *printVector(vector3f *v);
void printParticle(particle *v);
void printEmitter(emitter *e);
void initRandomParticles(emitter *e);
void error_callback(int error, const char* description);

void calcPos(particle *p, float dt);
void calcCol(particle *p);

float *serializeParticles(const particle_system *ps);

int main()
{

    /************* INIT *************/
    // Init OpenGL and GLFW
    initGLFW();
    setErrorCallbackGL();

    int width = 800, height = 800;
    GLFWwindow *window = createGLFWWindow(width, height, "Informatikprojekt - OpenGL");

    setCurrentContextGL(window);
    setFramebufferSizeCallbackGL(window);

    // glad
    initGlad();

    /************* PARTICLE SYSTEM *************/
    int particelAmount = 10000;
    vector3f *epos1 = initVector3f(0, 0, 0);
    emitter *e1 = initEmitter(epos1, particelAmount);

    particle_system *ps = initParticleSystem(1);
    (ps->emitters)[0] = e1;

    initRandomParticles(e1);

    /************* COMPILING SHADER *************/
    const char *vertexShaderSource = "#version 460 core\n"
                                     "\n"
                                     "layout (location = 0) in vec3 pos;   // the position variable has attribute position 0\n"
                                     "layout (location = 1) in vec3 dir; // the direction variable has attribute position 1\n"
                                     "layout (location = 2) in vec3 col; // the color variable has attribute position 2\n"
                                     "layout (location = 3) in float age; // the age variable has attribute position 3\n"
                                     "\n"
                                     "//in vec3 emitterPos; // the emitter pos variable\n"
                                     "//in float newAge; // the age variable\n"
                                     "\n"
                                     "out vec3 outCol; // output a color to the fragment shader\n"
                                     "\n"
                                     "void main()\n"
                                     "{\n"
                                     "    if (age < 0)\n"
                                     "    {\n"
                                     "        //pos = vec3(0, 0, 0); //emitterPos;\n"
                                     "        //age = 200; //newAge;\n"
                                     "    }\n"
                                     "\n"
                                     "    age -= 0.1f;\n"
                                     "    vec3 newPos = pos.xyz + dir.xyz;\n"
                                     "    gl_Position = vec4(newPos, 1.0);\n"
                                     "\n"
                                     "    outCol = col; // set ourColor to the input color we got from the vertex data\n"
                                     "}";

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int  successCompileVertex;
    char infoLogCompileVertex[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &successCompileVertex);
    if (!successCompileVertex)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLogCompileVertex);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s", infoLogCompileVertex);
    }

    const char *fragmentShaderSource = "#version 460 core\n"
                                       "\n"
                                       "in vec3 col; // the input variable from the vertex shader (same name and same type)\n"
                                       "\n"
                                       "out vec4 outCol;\n"
                                       "\n"
                                       "void main()\n"
                                       "{\n"
                                       "    outCol = vec4(col, 1.0);\n"
                                       "}";

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    int  successCompileFragment;
    char infoLogCompileFragment[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &successCompileFragment);
    if (!successCompileFragment)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLogCompileFragment);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s", infoLogCompileFragment);
    }

    /************* LINKING SHADER *************/
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int successLink;
    char infoLogLink[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &successLink);
    if (!successLink)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLogLink);
        printf("ERROR::SHADER::LINKING_FAILED\n%s", infoLogLink);
    }

    /*************** VAO / VBO ***************/
    // Init vertex data
    unsigned int vao, vbo;
    float *vertexData = serializeParticlesystem(ps);
    initVertexArrayBuffer(&vao);
    initVertexBufferObject(&vbo, vertexData);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // direction attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // color attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // age attribute
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);

    /************* RENDER LOOP *************/
    double time, tFrame, tLast = 0;
    while (!glfwWindowShouldClose(window))
    {
        time = glfwGetTime();
        tFrame = time - tLast;
        tLast = time;

        glClear(GL_COLOR_BUFFER_BIT);
        glfwGetFramebufferSize(window, &width, &height);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, particelAmount);

        /*updateParticles((float) tFrame, ps, calcPos, calcCol);

        emitter *e;
        particle *p;
        vector3f *pos;
        for (int j = 0; j < ps->eamount; j++)
        {
            e = (ps->emitters)[j];
            for (int i = 0; i < e->pamount; i++)
            {
                p = (e->particles)[i];
                pos = p->position;

                glColor3f(p->color->x, p->color->y, p->color->z);
                glBegin(GL_POINTS);
                glVertex3f(pos->x, pos->y, pos->z);
                glEnd();
            }
        }*/

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //END
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    terminateGLFW(window);

    freeParticleSystem(ps);

    return 0;
}

void calcPos(particle *p, float dt)
{
    p->position->x += p->direction->x * dt;
    p->position->y += p->direction->y * dt;
    p->position->z += p->direction->z * dt;
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

char *printVector(vector3f *v)
{
    char *c = malloc(100);
    sprintf(c, "(%f, %f, %f)", v->x, v->y, v->z);
    return c;
}

void printParticle(particle *v)
{
    printf("   Particle {\n");
    printf("      position = %s", printVector(v->position));
    printf("\n      direction = %s", printVector(v->direction));
    printf("\n   }");
}

void printEmitter(emitter *e)
{
    printf("Emitter {\n");

    for (int i = 0; i < e->pamount; i++)
    {
        printParticle((e->particles)[i]);
        printf("\n");
    }

    printf("\n}");
}
