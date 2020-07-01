//Demonstrating attenuating lighting

#include<iostream>
#include<cmath>
#include<cstdlib>
#include<string>
#include<GL/glew.h>
#include<GL/gl.h>
#include<GL/freeglut.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include"../headers/shader.h"
#include"../headers/mesh.h"
#include"../headers/camera.h"

#define ESCAPE 27

//window parameters
float winWidth = 1500;
float winHeight = 800;

//not at full screen initially
bool fullScreen = false;

unsigned int shaderProgram[2];
unsigned int VAOs[2];
unsigned int buffSizes[2];

glm::mat4 view;
glm::mat4 model;
glm::mat4 projection;
unsigned int viewLoc, modelLoc, projectionLoc;

//monkey faces
struct Monkey
{
    glm::vec3 pos;
    glm::vec3 col;
    unsigned int colLoc;
    Monkey()
    {
        pos = glm::vec3(0.0f,0.0f,0.0f);
        col = glm::vec3(0.5f,0.2f,0.0f);
    }
};
Monkey monkey;

//point light calculations
struct PointLight
{
    glm::vec3 pos;
    glm::vec3 col;
    glm::vec3 atten; //constant, linear, quadratic attenuation
    float angle, angleStep, amplitude, offset;
    unsigned int posLoc, colLoc, attenLoc;
    PointLight()
    {
        pos = glm::vec3(2.0f,0.0f,0.0f);
        col = glm::vec3(1.0f,1.0f,1.0f);
        atten = glm::vec3(1.0f,0.09f,0.032f);
        angle = 0.0f; angleStep = 0.0005f; amplitude = 5.0f; offset = 6.0f;
    }
};
PointLight lamp; //7 point lights to render

camera cam(glm::vec3(0.0f,-10.0f,0.0f), 0.0f, 0.0f, 90.0f, 0.0f);

void SetupGL()
{
    //import monkey and sphere models
    mesh monkeyMesh("../models/smooth_shade/monkey/verts.txt","../models/smooth_shade/monkey/norms.txt","../models/smooth_shade/monkey/inds.txt");
    mesh sphereMesh("../models/smooth_shade/sphere_16x16/verts.txt","../models/smooth_shade/sphere_16x16/inds.txt");
    VAOs[0] = monkeyMesh.VAO;
    VAOs[1] = sphereMesh.VAO;
    buffSizes[0] = monkeyMesh.mainBuffVerts;
    buffSizes[1] = sphereMesh.mainBuffVerts;
    //1 shader for the monkeys
    shader monkeyShad("atten_light_monkey.vert", "atten_light_monkey.frag");
    //and 1 for the lamp (sphere)
    shader sphereShad("atten_light_lamp.vert","atten_light_lamp.frag");
    shaderProgram[0] = monkeyShad.ID;
    shaderProgram[1] = sphereShad.ID;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f,0.1f,0.1f,1.0f);
}

//each time the window size changes, the following commands shall be executed
void Reshape(GLsizei w, GLsizei h)
{
    glViewport(0,0,w,h);
    float aspectRatio = (float)w/(float)h;
    projection = glm::mat4(1.0f);
    projection = cam.Perspective(aspectRatio);
    for (int i = 0; i < 2; ++i)
    {
        glUseProgram(shaderProgram[i]);
        projectionLoc = glGetUniformLocation(shaderProgram[i], "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    }
}

void Monkey()
{
    glUseProgram(shaderProgram[0]);

    //pass lamp parameters
    lamp.posLoc = glGetUniformLocation(shaderProgram[0], "lamp.pos");
    glUniform3f(lamp.posLoc, lamp.pos.x, lamp.pos.y, lamp.pos.z);
    lamp.colLoc = glGetUniformLocation(shaderProgram[0], "lamp.col");
    glUniform3f(lamp.colLoc, lamp.col.x, lamp.col.y, lamp.col.z);
    lamp.attenLoc = glGetUniformLocation(shaderProgram[0], "lamp.atten");
    glUniform3f(lamp.attenLoc, lamp.atten.x, lamp.atten.y, lamp.atten.z);

    //pass monkey parameters
    monkey.colLoc = glGetUniformLocation(shaderProgram[0], "monkey.col");
    glUniform3f(monkey.colLoc, monkey.col.x, monkey.col.y, monkey.col.z);

    //pass camera parameters
    cam.posLoc = glGetUniformLocation(shaderProgram[0], "cam.pos");
    glUniform3f(cam.posLoc, cam.pos.x, cam.pos.y, cam.pos.z);

    //pass matrices
    glBindVertexArray(VAOs[0]);
    model = glm::mat4(1.0f);
    model = glm::translate(model, monkey.pos);
    modelLoc = glGetUniformLocation(shaderProgram[0], "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, buffSizes[0]);
}

//render a lamp
void Lamp()
{
    glUseProgram(shaderProgram[1]);
    //force the lamp to a linear oscillation, so that the attenuating light effects can be visible
    lamp.pos.x = lamp.offset + lamp.amplitude*sin(lamp.angle);
    lamp.angle += lamp.angleStep;
    if (lamp.angle >= 2.0f*M_PI)
        lamp.angle = 0.0f;

    model = glm::mat4(1.0f);
    model = glm::translate(model, lamp.pos);
    model = glm::scale(model, glm::vec3(0.05f));
    modelLoc = glGetUniformLocation(shaderProgram[1], "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(VAOs[1]);
    glDrawArrays(GL_TRIANGLES, 0, buffSizes[1]);
}

void SetCamera()
{
    for (int i = 0; i < 2; ++i)
    {
        glUseProgram(shaderProgram[i]);
        view = glm::mat4(1.0f);
        view = cam.View();
        viewLoc = glGetUniformLocation(shaderProgram[i], "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    }
}

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    SetCamera();
    Monkey();
    Lamp();
    glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y)
{
    if (key == ESCAPE)
        glutLeaveMainLoop();
    if (key == 'f')
    {
        fullScreen = !fullScreen;
        if (fullScreen)
            glutFullScreen();
        else
            glutLeaveFullScreen();
    }
}

void Idle()
{
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    glutInit(&argc,argv);
    glutInitContextVersion(3,3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
    glutInitWindowPosition((int)((glutGet(GLUT_SCREEN_WIDTH) - (int)winWidth)/2.0f),
                           (int)((glutGet(GLUT_SCREEN_HEIGHT) - (int)winHeight)/2.0f));
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow(argv[0]);
    //initialise glew
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "glew Failed. Exiting...\n";
        return -1;
    }
    SetupGL();
    glutDisplayFunc(Render);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutIdleFunc(Idle);
    glutMainLoop();
    return 0;
}
