// Demonstrating face culling //

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
#include"../headers/font.h"
#include"../headers/camera.h"

#define ESCAPE 27

//window dimensions and aspect ratio
int winWidth = 1500;
int winHeight = 800;
float aspectRatio;

bool fullScreen = false; //not at full screen initially
bool cullFace = false; //cull face disabled initially (exterior and interior are rendered)

unsigned int shadProgMonkey, shadProgText;
unsigned int VAO;
unsigned int buffSize;

//timing the render loop
float currentFrame, previousFrame = 0.0f;

//separate matrices for the 3D rendering and the text rendering
glm::mat4 projection, view, model;
glm::mat4 textProjection, textModel;
unsigned int viewLoc, modelLoc, projectionLoc;
unsigned int textProjectionLoc, textModelLoc;

//keyboard state: which keys are 'up' (not being pressed))
bool aUp,dUp,sUp,wUp;
bool leftUp,rightUp,downUp,upUp;

//asteroid model
struct Monkey
{
    glm::vec3 pos;
    glm::vec3 col;
    unsigned int colLoc;
    Monkey()
    {
        pos = glm::vec3(0.0f,0.0f,0.0f);
        col = glm::vec3(0.4f,0.4f,0.4f);
    }
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
        dir = glm::vec3(1.0f,-1.0f,0.5f);
        col = glm::vec3(1.0f,1.0f,1.0f);
    }
};
DirLight dLight; //1 directional light to render

glm::vec3 textColor = glm::vec3(0.6f,0.0f,0.0f);
unsigned int textColorLoc;

camera cam(glm::vec3(0.0f,-10.0f,0.0f), 20.0f, 100.0f);

void SetupGL()
{
    //import monkey data and store them appropriately in a buffer
    mesh monkeyMesh("../models/flat_shade/monkey/verts.txt","../models/flat_shade/monkey/norms.txt","../models/flat_shade/monkey/inds.txt");
    VAO = monkeyMesh.VAO;
    buffSize = monkeyMesh.mainBuffVerts;
    //create 4 shaders
    shader monkeyShad("face_cull.vert", "face_cull.frag");
    shader textShad("face_cull_text.vert", "face_cull_text.frag");
    shadProgMonkey = monkeyShad.ID;
    shadProgText = textShad.ID;
    //import the font that will be used
    font myFont("../fonts/KhmerOS.ttf");

    //initialize keyboard's state (nothing is pressed --> all keys are 'up')
    aUp = dUp = sUp = wUp = true;
    leftUp = rightUp = downUp = upUp = true;

    glEnable(GL_DEPTH_TEST);
    //enabling blending for the text rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f,0.2f,0.4f,1.0f);
}

//each time the window size changes, the following commands shall be executed
void Reshape(GLsizei w, GLsizei h)
{
    glViewport(0,0,w,h);
    winWidth = w;
    winHeight = h;
    aspectRatio = (float)w/(float)h;
    //matrix for the 3D stuff
    projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f),  aspectRatio, 0.1f,1000.0f);
    glUseProgram(shadProgMonkey);
    projectionLoc = glGetUniformLocation(shadProgMonkey, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    //matrix for the 2D text
    textProjection = glm::mat4(1.0f);
    if (w >= h)
        textProjection = glm::ortho(0.0f, static_cast<float>(winWidth)*aspectRatio,
                                    0.0f, static_cast<float>(winHeight));
    else
        textProjection = glm::ortho(0.0f, static_cast<float>(winWidth),
                                    0.0f, static_cast<float>(winHeight)*aspectRatio);
    glUseProgram(shadProgText);
    textProjectionLoc = glGetUniformLocation(shadProgText, "textProjection");
    glUniformMatrix4fv(textProjectionLoc, 1, GL_FALSE, glm::value_ptr(textProjection));
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

//place the camera in the 3D world
void SetCamera()
{
    glUseProgram(shadProgMonkey);
    view = glm::mat4(1.0f);
    view = cam.View();
    viewLoc = glGetUniformLocation(shadProgMonkey, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

//render the monkey
void Monkey()
{
    glUseProgram(shadProgMonkey);
    model = glm::mat4(1.0f);
    model = glm::translate(model, monkey.pos);
    model = glm::scale(model, glm::vec3(5.0f,5.0f,5.0f));
    modelLoc = glGetUniformLocation(shadProgMonkey, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    monkey.colLoc = glGetUniformLocation(shadProgMonkey, "monkey.col");
    glUniform3f(monkey.colLoc, monkey.col.x, monkey.col.y, monkey.col.z);
    dLight.colLoc = glGetUniformLocation(shadProgMonkey, "dLight.col");
    glUniform3f(dLight.colLoc, dLight.col.x, dLight.col.y, dLight.col.z);
    dLight.dirLoc = glGetUniformLocation(shadProgMonkey, "dLight.dir");
    glUniform3f(dLight.dirLoc, dLight.dir.x, dLight.dir.y, dLight.dir.z);
    cam.posLoc = glGetUniformLocation(shadProgMonkey, "cam.pos");
    glUniform3f(cam.posLoc, cam.pos.x, cam.pos.y, cam.pos.z);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, buffSize);
}

//render text stuff
void TextInfo()
{
    glUseProgram(shadProgText);
    textModel = glm::mat4(1.0f);
    textModel = glm::scale(textModel, glm::vec3(2.0f,1.0f,1.0f));
    textModelLoc = glGetUniformLocation(shadProgText, "textModel");
    glUniformMatrix4fv(textModelLoc, 1, GL_FALSE, glm::value_ptr(textModel));
    textColorLoc = glGetUniformLocation(shadProgText, "textColor");
    glUniform3f(textColorLoc, textColor.x, textColor.y, textColor.z);
    //the text is being render on the top left of the window
    if (winWidth >= winHeight)
    {
        RenderText("Use a,d,s,w and arrows to navigate.", (0.0f + 20.0f)*aspectRatio, winHeight-50.0f, 0.4f);
        RenderText("Press 1 to enable cull face", (0.0f + 20.0f)*aspectRatio, winHeight-80.0f, 0.4f);
        RenderText("Press 2 to disable cull face", (0.0f + 20.0f)*aspectRatio, winHeight-110.0f, 0.4f);
        if (cullFace)
            RenderText("Cull face: enabled (get inside the monkey)", (0.0f + 20.0f)*aspectRatio, winHeight-160.0f, 0.4f);
        else
            RenderText("Cull face: disabled", (0.0f + 20.0f)*aspectRatio, winHeight-160.0f, 0.4f);
    }
    else
    {
        RenderText("Use a,d,s,w and arrows to navigate.", (0.0f + 20.0f), (winHeight-50.0f)*aspectRatio, 0.4f);
        RenderText("Press 1 to enable cull face", (0.0f + 20.0f), (winHeight-80.0f)*aspectRatio, 0.4f);
        RenderText("Press 2 to disable cull face", (0.0f + 20.0f), (winHeight-110.0f)*aspectRatio, 0.4f);
        if (cullFace)
            RenderText("Cull face: enabled (get inside the monkey)", (0.0f + 20.0f), (winHeight-160.0f)*aspectRatio, 0.4f);
        else
            RenderText("Cull face: disabled", 0.0f + 20.0f, (winHeight-160.0f)*aspectRatio, 0.4f);
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
    TextInfo();
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
    if (key == '1')
    {
        glEnable(GL_CULL_FACE); //<---------------------------
        cullFace = true;
    }
    if (key == '2')
    {
        glDisable(GL_CULL_FACE); //<---------------------------
        cullFace = false;
    }
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
