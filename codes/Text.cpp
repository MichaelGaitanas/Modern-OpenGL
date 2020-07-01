// Demonstrating simple 2D text rendering

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
#include"../headers/font.h"

#define ESCAPE 27

//window parameters
int winWidth = 800;
int winHeight = 600;
float aspectRatio;

unsigned int shaderProgram;

glm::mat4 projection;
unsigned int projectionLoc;

glm::vec3 textColor = glm::vec3(0.4f,0.0f,0.0f);

void SetupGL()
{
    shader shad("Text.vert","Text.frag");
    font myFont("../fonts/KhmerOS.ttf");
    shaderProgram = shad.ID;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //enabling blending for the text rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(1.0f,1.0f,1.0f,1.0f);
}

//each time the window size changes, the following commands shall be executed
void Reshape(GLsizei w, GLsizei h)
{
    glViewport(0,0,w,h);
    aspectRatio = (float)w/(float)h;
    if (w >= h)
        projection = glm::ortho(0.0f, static_cast<float>(winWidth)*aspectRatio,
                                0.0f, static_cast<float>(winHeight));
    else
        projection = glm::ortho(0.0f, static_cast<float>(winWidth),
                                0.0f, static_cast<float>(winHeight)*aspectRatio);
    glUseProgram(shaderProgram);
    projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glUniform3f(glGetUniformLocation(shaderProgram, "textColor"), textColor.x, textColor.y, textColor.z);
    if (winWidth >= winHeight)
    {
        RenderText("This is a test text.", (0.0f + 20.0f)*aspectRatio, winHeight-100.0f, 0.5f);
        RenderText("This is another test text", (0.0f + 120.0f)*aspectRatio, winHeight-300.0f, 0.8f);
    }
    else
    {
        RenderText("This is a test text.", (0.0f + 20.0f), (winHeight-100.0f)*aspectRatio, 0.5f);
        RenderText("This is another test text", (0.0f + 120.0f), (winHeight-300.0f)*aspectRatio, 0.8f);
    }
    glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y)
{
    if (key == ESCAPE)
        glutLeaveMainLoop();
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
