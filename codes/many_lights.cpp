//Demonstrating multiple lights. 7 point lights are
//being used (with attenuation) and 1 directionl light
//Directional light will not work until you uncomment
//line 94 from 'many_lights_monkey.frag' file.

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

#define MONKEYS 5 //number of monkey faces to be rendered
#define POINT_LIGHTS 7 //number of point lights to be rendered

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

//monkey faces
struct Monkey
{
    glm::vec3 pos[MONKEYS];
    glm::vec3 col = glm::vec3(0.5f,0.2f,0.0f);
    unsigned int colLoc;
};
Monkey monkey;

//directional light calculations
struct DirLight
{
    glm::vec3 dir;
    glm::vec3 col;
    unsigned int dirLoc, colLoc;
    DirLight()
    {
        dir = glm::vec3(0.0f,-1.0f,0.0f);
        col = glm::vec3(1.0f,1.0f,1.0f);
    }
};
DirLight dLight; //1 directional light to render

//point light
struct PointLight
{
    glm::vec3 pos;
    glm::vec3 col;
    glm::vec3 atten; //constant, linear, quadratic attenuation
    unsigned int posLoc, colLoc, attenLoc;
    PointLight()
    {
        col = glm::vec3(1.0f,1.0f,1.0f);
        atten = glm::vec3(1.0f,0.09f,0.032f);
    }
};
PointLight pLight[POINT_LIGHTS]; //7 point lights to render

camera cam(glm::vec3(0.0f,-5.0f,0.0f), 20.0f, 100.0f);

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
    shader monkeyShad("many_lights_monkey.vert", "many_lights_monkey.frag");
    //and 1 for the lamps (spheres)
    shader sphereShad("many_lights_lamp.vert","many_lights_lamp.frag");
    shaderProgram[0] = monkeyShad.ID;
    shaderProgram[1] = sphereShad.ID;

    /* data initialization */

    //monkeys
    monkey.pos[0] = glm::vec3(3.0f,3.0f,2.0f);
    monkey.pos[1] = glm::vec3(-5.0f,3.0f,-3.0f);
    monkey.pos[2] = glm::vec3(1.0f,-3.0f,7.0f);
    monkey.pos[3] = glm::vec3(-5.0f,0.0f,-6.0f);
    monkey.pos[4] = glm::vec3(-6.0f,-1.0f,0.0f);

    //point lights
    pLight[0].pos = glm::vec3(5.0f,0.0f,0.0f);
    pLight[1].pos = glm::vec3(5.0f,3.0f,-0.0f);
    pLight[2].pos = glm::vec3(5.0f,6.0f,6.0f);
    pLight[3].pos = glm::vec3(-5.0f,-1.0f,-1.0f);
    pLight[4].pos = glm::vec3(5.0f,-1.0f,0.0f);
    pLight[5].pos = glm::vec3(-5.5f,0.5f,-8.0f);
    pLight[6].pos = glm::vec3(5.0f,0.0f,4.0f);

    //initialize keyboard's state (nothing is pressed --> all keys are 'up')
    aUp = dUp = sUp = wUp = true;
    leftUp = rightUp = downUp = upUp = true;

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

    //pass directional light
    dLight.dirLoc = glGetUniformLocation(shaderProgram[0], "dLight.dir");
    glUniform3f(dLight.dirLoc, dLight.dir.x, dLight.dir.y, dLight.dir.z);
    dLight.colLoc = glGetUniformLocation(shaderProgram[0], "dLight.col");
    glUniform3f(dLight.colLoc, dLight.col.x, dLight.col.y, dLight.col.z);

    /**************************** pass point lights ****************************/

    //pLight[0]
    pLight[0].posLoc = glGetUniformLocation(shaderProgram[0], "pLight[0].pos");
    glUniform3f(pLight[0].posLoc, pLight[0].pos.x, pLight[0].pos.y, pLight[0].pos.z);
    pLight[0].colLoc = glGetUniformLocation(shaderProgram[0], "pLight[0].col");
    glUniform3f(pLight[0].colLoc, pLight[0].col.x, pLight[0].col.y, pLight[0].col.z);
    pLight[0].attenLoc = glGetUniformLocation(shaderProgram[0], "pLight[0].atten");
    glUniform3f(pLight[0].attenLoc, pLight[0].atten.x, pLight[0].atten.y, pLight[0].atten.z);

    //pLight[1]
    pLight[1].posLoc = glGetUniformLocation(shaderProgram[0], "pLight[1].pos");
    glUniform3f(pLight[1].posLoc, pLight[1].pos.x, pLight[1].pos.y, pLight[1].pos.z);
    pLight[1].colLoc = glGetUniformLocation(shaderProgram[0], "pLight[1].col");
    glUniform3f(pLight[1].colLoc, pLight[1].col.x, pLight[1].col.y, pLight[1].col.z);
    pLight[1].attenLoc = glGetUniformLocation(shaderProgram[0], "pLight[1].atten");
    glUniform3f(pLight[1].attenLoc, pLight[1].atten.x, pLight[1].atten.y, pLight[1].atten.z);

    //pLight[2]
    pLight[2].posLoc = glGetUniformLocation(shaderProgram[0], "pLight[2].pos");
    glUniform3f(pLight[2].posLoc, pLight[2].pos.x, pLight[2].pos.y, pLight[2].pos.z);
    pLight[2].colLoc = glGetUniformLocation(shaderProgram[0], "pLight[2].col");
    glUniform3f(pLight[2].colLoc, pLight[2].col.x, pLight[2].col.y, pLight[2].col.z);
    pLight[2].attenLoc = glGetUniformLocation(shaderProgram[0], "pLight[2].atten");
    glUniform3f(pLight[2].attenLoc, pLight[2].atten.x, pLight[2].atten.y, pLight[2].atten.z);

    //pLight[3]
    pLight[3].posLoc = glGetUniformLocation(shaderProgram[0], "pLight[3].pos");
    glUniform3f(pLight[3].posLoc, pLight[3].pos.x, pLight[3].pos.y, pLight[3].pos.z);
    pLight[3].colLoc = glGetUniformLocation(shaderProgram[0], "pLight[3].col");
    glUniform3f(pLight[3].colLoc, pLight[3].col.x, pLight[3].col.y, pLight[3].col.z);
    pLight[3].attenLoc = glGetUniformLocation(shaderProgram[0], "pLight[3].atten");
    glUniform3f(pLight[3].attenLoc, pLight[3].atten.x, pLight[3].atten.y, pLight[3].atten.z);

    //pLight[4]
    pLight[4].posLoc = glGetUniformLocation(shaderProgram[0], "pLight[4].pos");
    glUniform3f(pLight[4].posLoc, pLight[4].pos.x, pLight[4].pos.y, pLight[4].pos.z);
    pLight[4].colLoc = glGetUniformLocation(shaderProgram[0], "pLight[4].col");
    glUniform3f(pLight[4].colLoc, pLight[4].col.x, pLight[4].col.y, pLight[4].col.z);
    pLight[4].attenLoc = glGetUniformLocation(shaderProgram[0], "pLight[4].atten");
    glUniform3f(pLight[4].attenLoc, pLight[4].atten.x, pLight[4].atten.y, pLight[4].atten.z);

    //pLight[5]
    pLight[5].posLoc = glGetUniformLocation(shaderProgram[0], "pLight[5].pos");
    glUniform3f(pLight[5].posLoc, pLight[5].pos.x, pLight[5].pos.y, pLight[5].pos.z);
    pLight[5].colLoc = glGetUniformLocation(shaderProgram[0], "pLight[5].col");
    glUniform3f(pLight[5].colLoc, pLight[5].col.x, pLight[5].col.y, pLight[5].col.z);
    pLight[5].attenLoc = glGetUniformLocation(shaderProgram[0], "pLight[5].atten");
    glUniform3f(pLight[5].attenLoc, pLight[5].atten.x, pLight[5].atten.y, pLight[5].atten.z);

    //pLight[6]
    pLight[6].posLoc = glGetUniformLocation(shaderProgram[0], "pLight[6].pos");
    glUniform3f(pLight[6].posLoc, pLight[6].pos.x, pLight[6].pos.y, pLight[6].pos.z);
    pLight[6].colLoc = glGetUniformLocation(shaderProgram[0], "pLight[6].col");
    glUniform3f(pLight[6].colLoc, pLight[6].col.x, pLight[6].col.y, pLight[6].col.z);
    pLight[6].attenLoc = glGetUniformLocation(shaderProgram[0], "pLight[6].atten");
    glUniform3f(pLight[6].attenLoc, pLight[6].atten.x, pLight[6].atten.y, pLight[6].atten.z);

    /***************************************************************************/

    //pass monkey parameters
    monkey.colLoc = glGetUniformLocation(shaderProgram[0], "monkey.col");
    glUniform3f(monkey.colLoc, monkey.col.x, monkey.col.y, monkey.col.z);

    //pass camera parameters
    cam.posLoc = glGetUniformLocation(shaderProgram[0], "cam.pos");
    glUniform3f(cam.posLoc, cam.pos.x, cam.pos.y, cam.pos.z);

    //pass matrices
    glBindVertexArray(VAOs[0]);
    for (int i = 0; i < MONKEYS; ++i)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, monkey.pos[i]);
        modelLoc = glGetUniformLocation(shaderProgram[0], "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, buffSizes[0]);
    }
}

//render a lamp
void Light()
{
    for (int i = 0; i < POINT_LIGHTS; ++i)
    {
        glUseProgram(shaderProgram[1]);
        model = glm::mat4(1.0f);
        model = glm::translate(model, pLight[i].pos);
        model = glm::scale(model, glm::vec3(0.15f));
        modelLoc = glGetUniformLocation(shaderProgram[1], "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, buffSizes[1]);
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

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    currentFrame = (float)glutGet(GLUT_ELAPSED_TIME)/1000.0f; // in seconds
    cam.speedMultiplyFactor = currentFrame - previousFrame;
    previousFrame = currentFrame;
    ProcessInputs();
    SetCamera();
    Monkey();
    Light();
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
