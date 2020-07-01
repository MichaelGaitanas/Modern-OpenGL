

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
#define SPACEBAR 32

//window parameters
float winWidth = 1000;
float winHeight = 800;

//not at full screen initially
bool fullScreen = false;

unsigned int shaderProgram;
unsigned int VAO;
unsigned int buffSize;

glm::mat4 view;
glm::mat4 model;
glm::mat4 projection;
unsigned int viewLoc, modelLoc, projectionLoc;

//keyboard state: which keys are 'up' (not being pressed))
bool aUp,dUp,sUp,wUp;
bool leftUp,rightUp,downUp,upUp;
bool jumpMode;

//phyics parameters
float g = 10.0f, dt = 0.001f;

//parameters in order to time the render loop
float currentFrame, previousFrame = 0.0f;

//terrain model
struct Terrain
{
    glm::vec3 col;
    unsigned int colLoc;
    Terrain()
    {
        col = glm::vec3(0.7f,0.7f,0.8f);
    }
};
Terrain terrain;

//directional light calculations
struct DirLight
{
    glm::vec3 dir;
    glm::vec3 col;
    unsigned int dirLoc, colLoc;
    DirLight()
    {
        dir = glm::vec3(1.0f,0.0f,0.5f);
        col = glm::vec3(1.0f,1.0f,1.0f);
    }
};
DirLight dLight; //1 directional light to render

struct Player
{
    float z0,vz0; //player's inital altitude and velocity
    float z,vz; //player's altitude and velocity through time
    Player()
    {
        z0 = z = 2.0f;
        vz0 = vz = 7.0f;
    }
};
Player player;

camera cam(glm::vec3(0.0f,-10.0f,player.z0), 20.0f, 100.0f);

void SetupGL()
{
    //import the terrain model
    mesh terrainMesh("../models/smooth_shade/terrain/verts.txt","../models/smooth_shade/terrain/norms.txt","../models/smooth_shade/terrain/inds.txt");
    VAO = terrainMesh.VAO;
    buffSize = terrainMesh.mainBuffVerts;
    //1 shader for the monkeys
    shader terrainShad("first_person.vert","first_person.frag");
    shaderProgram = terrainShad.ID;

    //initialize keyboard's state (nothing is pressed --> all keys are 'up')
    aUp = dUp = sUp = wUp = true;
    leftUp = rightUp = downUp = upUp = true;
    jumpMode = false;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.8f,1.0f,1.0f,1.0f);
}

//each time the window size changes, the following commands shall be executed
void Reshape(GLsizei w, GLsizei h)
{
    glViewport(0,0,w,h);
    float aspectRatio = (float)w/(float)h;
    projection = glm::mat4(1.0f);
    projection = cam.Perspective(aspectRatio);
    glUseProgram(shaderProgram);
    projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void Terrain()
{
    glUseProgram(shaderProgram);
    //pass directional light
    dLight.dirLoc = glGetUniformLocation(shaderProgram, "dLight.dir");
    glUniform3f(dLight.dirLoc, dLight.dir.x, dLight.dir.y, dLight.dir.z);
    dLight.colLoc = glGetUniformLocation(shaderProgram, "dLight.col");
    glUniform3f(dLight.colLoc, dLight.col.x, dLight.col.y, dLight.col.z);

    //pass terrain parameters
    terrain.colLoc = glGetUniformLocation(shaderProgram, "terrain.col");
    glUniform3f(terrain.colLoc, terrain.col.x, terrain.col.y, terrain.col.z);

    //pass camera parameters
    cam.posLoc = glGetUniformLocation(shaderProgram, "cam.pos");
    glUniform3f(cam.posLoc, cam.pos.x, cam.pos.y, cam.pos.z);

    //pass matrices
    glBindVertexArray(VAO);
    model = glm::mat4(1.0f);
    modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, buffSize);
}

void ProcessInputs()
{
    //camera translation
    if (!wUp)
        cam.TranslateFront();
    if (!sUp)
        cam.TranslateBack();
    if (!dUp)
        cam.TranslateRight();
    if (!aUp)
        cam.TranslateLeft();

    //camera rotation
    if (!rightUp)
        cam.RotateRight();
    if (!leftUp)
        cam.RotateLeft();
    if (!upUp)
        cam.RotateUp();
    if (!downUp)
        cam.RotateDown();

    //physics calculations
    if (jumpMode && player.z >= player.z0)
    {
        player.vz0 += (-g*dt);
        player.z += player.vz0*dt - g*dt*dt/2.0f;
    }
    else
    {
        player.z = player.z0;
        player.vz0 = player.vz;
        jumpMode = false; //now, you are on the ground
    }

    cam.pos.z = player.z;
    cam.UpdateVectors();
}

void SetCamera()
{
    glUseProgram(shaderProgram);
    view = glm::mat4(1.0f);
    view = cam.View();
    viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    currentFrame = (float)glutGet(GLUT_ELAPSED_TIME)/1000.0f; //in seconds
    cam.speedMultiplyFactor = currentFrame - previousFrame;
    previousFrame = currentFrame;
    ProcessInputs();
    SetCamera();
    Terrain();
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
    if (key == SPACEBAR)
        jumpMode = true;
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
