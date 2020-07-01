//Demonstrating one side lighting using a plane. The one side lighting occurs by changing
//the direction of the normal vector in the fragment shader 'plane_lighting_plane.frag' (lines 21-22)

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
float winWidth = 1000;
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

//keyboard state: which keys are 'up' (not being pressed))
bool aUp,dUp,sUp,wUp;
bool leftUp,rightUp,downUp,upUp;

//timing the render loop
float currentFrame, previousFrame = 0.0f;

//plane model
struct Plane
{
    glm::vec3 pos;
    glm::vec3 col;
    unsigned int colLoc;
    Plane()
    {
        pos = glm::vec3(0.0f,0.0f,0.0f);
        col = glm::vec3(0.5f,0.2f,0.0f);
    }
};
Plane plane;

//point light
struct PointLight
{
    glm::vec3 pos;
    glm::vec3 col;
    float ang;
    unsigned int posLoc, colLoc;
    PointLight()
    {
        pos = glm::vec3(0.0f,0.0f,8.0f);
        col = glm::vec3(1.0f,1.0f,1.0f);
        ang = 0.0f;
    }
};
PointLight lamp;

camera cam(glm::vec3(0.0f,-12.0f,2.0f), 20.0f, 100.0f);

void SetupGL()
{
    //import a plane mesh
    mesh planeMesh("../models/flat_shade/plane/verts.txt","../models/flat_shade/plane/norms.txt","../models/flat_shade/plane/inds.txt");
    //import a sphere (lamp) mesh
    mesh sphereMesh("../models/flat_shade/sphere_16x16/verts.txt","../models/flat_shade/sphere_16x16/norms.txt","../models/flat_shade/sphere_16x16/inds.txt");
    VAOs[0] = planeMesh.VAO;
    VAOs[1] = sphereMesh.VAO;
    buffSizes[0] = planeMesh.mainBuffVerts;
    buffSizes[1] = sphereMesh.mainBuffVerts;
    shader planeShad("plane_lighting_plane.vert", "plane_lighting_plane.frag");
    shader sphereShad("plane_lighting_lamp.vert","plane_lighting_lamp.frag");
    shaderProgram[0] = planeShad.ID;
    shaderProgram[1] = sphereShad.ID;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f,0.3f,0.8f,1.0f);

    //initialize keyboard's state (nothing is pressed --> all keys are 'up')
    aUp = dUp = sUp = wUp = true;
    leftUp = rightUp = downUp = upUp = true;
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

void ProcessInputs()
{
    //translation
    if (!wUp)
        cam.TranslateFront();
    if (!sUp)
        cam.TranslateBack();
    if (!dUp)
        cam.TranslateRight();
    if (!aUp)
        cam.TranslateLeft();

    //rotation
    if (!rightUp)
        cam.RotateRight();
    if (!leftUp)
        cam.RotateLeft();
    if (!upUp)
        cam.RotateUp();
    if (!downUp)
        cam.RotateDown();

    cam.UpdateVectors();
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

void Plane()
{
    glUseProgram(shaderProgram[0]);
    model = glm::mat4(1.0f);
    model = glm::translate(model, plane.pos);
    model = glm::scale(model, glm::vec3(5.0f,5.0f,5.0f));
    modelLoc = glGetUniformLocation(shaderProgram[0], "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    plane.colLoc = glGetUniformLocation(shaderProgram[0], "plane.col");
    glUniform3f(plane.colLoc, plane.col.x, plane.col.y, plane.col.z);
    //set the lamp to constantly rotate around the plane
    lamp.pos = glm::vec3(8.0f*cos(lamp.ang), 0.0f, 8.0f*sin(lamp.ang));
    lamp.ang += 0.001f;
    if (lamp.ang >= 2.0f*M_PI)
        lamp.ang = 0.0f;
    lamp.colLoc = glGetUniformLocation(shaderProgram[0], "lamp.col");
    glUniform3f(lamp.colLoc, lamp.col.x, lamp.col.y, lamp.col.z);
    lamp.posLoc = glGetUniformLocation(shaderProgram[0], "lamp.pos");
    glUniform3f(lamp.posLoc, lamp.pos.x, lamp.pos.y, lamp.pos.z);
    cam.posLoc = glGetUniformLocation(shaderProgram[0], "cam.pos");
    glUniform3f(cam.posLoc, cam.pos.x, cam.pos.y, cam.pos.z);

    glBindVertexArray(VAOs[0]);
    glDrawArrays(GL_TRIANGLES, 0, buffSizes[0]);
}

void Lamp()
{
    glUseProgram(shaderProgram[1]);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lamp.pos);
    model = glm::scale(model, glm::vec3(0.2f));
    modelLoc = glGetUniformLocation(shaderProgram[1], "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    lamp.colLoc = glGetUniformLocation(shaderProgram[1], "lamp.col");
    glUniform3f(lamp.colLoc, lamp.col.x, lamp.col.y, lamp.col.z);

    glBindVertexArray(VAOs[1]);
    glDrawArrays(GL_TRIANGLES, 0, buffSizes[1]);
}

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    currentFrame = (float)glutGet(GLUT_ELAPSED_TIME)/1000.0f; // in seconds
    cam.speedMultiplyFactor = currentFrame - previousFrame;
    previousFrame = currentFrame;
    ProcessInputs();
    SetCamera();
    Plane();
    Lamp();
    glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y)
{
    if (key == 'a')
        aUp = false;
    if (key == 'd')
        dUp = false;
    if (key == 's')
        sUp = false;
    if (key == 'w')
        wUp = false;
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

void KeyboardUp(unsigned char key, int x, int y)
{
    if (key == 'a')
        aUp = true;
    if (key == 'd')
        dUp = true;
    if (key == 's')
        sUp = true;
    if (key == 'w')
        wUp = true;
}

void SpecialKeyboard(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT)
        leftUp = false;
    if (key == GLUT_KEY_RIGHT)
        rightUp = false;
    if (key == GLUT_KEY_DOWN)
        downUp = false;
    if (key == GLUT_KEY_UP)
        upUp = false;
}

void SpecialKeyboardUp(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT)
        leftUp = true;
    if (key == GLUT_KEY_RIGHT)
        rightUp = true;
    if (key == GLUT_KEY_DOWN)
        downUp = true;
    if (key == GLUT_KEY_UP)
        upUp = true;
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
    glutKeyboardUpFunc(KeyboardUp);
    glutSpecialFunc(SpecialKeyboard);
    glutSpecialUpFunc(SpecialKeyboardUp);
    glutIdleFunc(Idle);
    glutMainLoop();
    return 0;
}
