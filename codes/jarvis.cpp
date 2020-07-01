//Displaying JARVIS text on screen. No mesh model was imported. All shapes (except the fonts)
//are drawn dynamically.

#include<iostream>
#include<cmath>
#include<string>
#include<vector>
#include<GL/glew.h>
#include<GL/gl.h>
#include<GL/freeglut.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include"../headers/shader.h"
#include"../headers/font.h"

#define ESCAPE 27

bool fullScreen = true; //at full screen initially

//window dimensions
int winWidth = 1000;
int winHeight = 800;

//1 shader for the text and 1 for the rest
unsigned int shadProgText, shadProgArcs;

//disk color
glm::vec3 diskCol = glm::vec3(0.0f,0.3f,0.1f);
unsigned int diskColLoc;

//JARVIS text color
glm::vec3 textCol = glm::vec3(0.0f,0.7f,0.1f);
unsigned int textColLoc;

//arcs color
glm::vec3 arcCol = glm::vec3(0.0f,0.2f,0.0f);
unsigned int arcColLoc;

//auxiliary variable
glm::vec3 auxCol;
unsigned int auxColLoc;

glm::mat4 model;
glm::mat4 projection;
unsigned int modelLoc, projectionLoc;

//JARVIS's background
struct disk
{
    unsigned int VAO, VBO;
    std::vector<float> buffer;
    const float angStep = 0.00005f;
    disk(float r)
    {
        //algorithm to create a disk (z coordinate = 0)
        for (float ang = 0.0f; ang <= 2.0f*M_PI; ang += angStep)
        {
            //triangle vertex 1
            buffer.push_back(0.0f);
            buffer.push_back(0.0f);
            buffer.push_back(0.0f);
            //triangle vertex 2
            buffer.push_back(r*cos(ang));
            buffer.push_back(r*sin(ang));
            buffer.push_back(0.0f);
            //triangle vertex 3
            buffer.push_back(r*cos(ang + angStep));
            buffer.push_back(r*sin(ang + angStep));
            buffer.push_back(0.0f);
        }
    }
};
disk disk1(0.6f);

//rotating arcs around the disk
struct arc
{
    unsigned int VAO, VBO;
    std::vector<float> buffer;
    float rotAng = 0.0f;
    const float angStep = 0.00005f;
    float rotAngStep;
    arc(float r, float ang1, float ang2, float rotAngleStep)
    {
        //Algorithm to create an arc (z coordinate = 0).
        //Create points tha lie on the circumference of a circle's arc
        //and connect them via GL_LINE_STRIP.
        rotAngStep = rotAngleStep;
        for (float ang = ang1; ang <= ang2; ang += angStep)
        {
            buffer.push_back(r*cos(ang));
            buffer.push_back(r*sin(ang));
            buffer.push_back(0.0f);
        }
    }
};
arc arc1(0.605f, 0.0f, (float)M_PI, 0.01f);
arc arc2(0.630f, 0.0f, (float)M_PI, 0.008f);
arc arc3(0.655f, 0.0f, (float)M_PI/2.0f, 0.005f);
arc arc4(0.680f, 0.0f, (float)M_PI, 0.003f);

void SetupGL()
{
    //4 shaders: 2 for JARVIS text and 2 for the rest
    shader shadJarvisText("jarvis_text.vert","jarvis_text.frag");
    shader shadArcs("jarvis_arcs.vert","jarvis_arcs.frag");
    shadProgText = shadJarvisText.ID;
    shadProgArcs = shadArcs.ID;
    //import the font
    font jarvisFont("../fonts/KhmerOS.ttf");

    //configure disk1
    glGenVertexArrays(1, &disk1.VAO);
    glBindVertexArray(disk1.VAO);
    glGenBuffers(1, &disk1.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, disk1.VBO);
    glBufferData(GL_ARRAY_BUFFER, disk1.buffer.size()*sizeof(float), &disk1.buffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //configure arc1
    glGenVertexArrays(1, &arc1.VAO);
    glBindVertexArray(arc1.VAO);
    glGenBuffers(1, &arc1.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, arc1.VBO);
    glBufferData(GL_ARRAY_BUFFER, arc1.buffer.size()*sizeof(float), &arc1.buffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //configure arc2
    glGenVertexArrays(1, &arc2.VAO);
    glBindVertexArray(arc2.VAO);
    glGenBuffers(1, &arc2.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, arc2.VBO);
    glBufferData(GL_ARRAY_BUFFER, arc2.buffer.size()*sizeof(float), &arc2.buffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //configure arc3
    glGenVertexArrays(1, &arc3.VAO);
    glBindVertexArray(arc3.VAO);
    glGenBuffers(1, &arc3.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, arc3.VBO);
    glBufferData(GL_ARRAY_BUFFER, arc3.buffer.size()*sizeof(float), &arc3.buffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //configure arc4
    glGenVertexArrays(1, &arc4.VAO);
    glBindVertexArray(arc4.VAO);
    glGenBuffers(1, &arc4.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, arc4.VBO);
    glBufferData(GL_ARRAY_BUFFER, arc4.buffer.size()*sizeof(float), &arc4.buffer[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(10.0f);
    glClearColor(0.0f,0.0f,0.0f,1.0f);
}

//each time the window size changes, the following commands shall be executed
void Reshape(GLsizei w, GLsizei h)
{
    glViewport(0,0,w,h);
    float aspectRatio = (float)w/(float)h;
    projection = glm::mat4(1.0f);
    if (w >= h)
        projection = glm::ortho(-1.0f*aspectRatio, 1.0f*aspectRatio, -1.0f, 1.0f);
    else
        projection = glm::ortho(-1.0f, 1.0f, -1.0f*aspectRatio, 1.0f*aspectRatio);
    glUseProgram(shadProgText);
    projectionLoc = glGetUniformLocation(shadProgText, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUseProgram(shadProgArcs);
    projectionLoc = glGetUniformLocation(shadProgArcs, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void Disk()
{
    glUseProgram(shadProgArcs);
    model = glm::mat4(1.0f);
    modelLoc = glGetUniformLocation(shadProgArcs, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    auxCol = diskCol;
    auxColLoc = glGetUniformLocation(shadProgArcs, "auxCol");
    glUniform3f(auxColLoc, auxCol.x, auxCol.y, auxCol.z);
    glBindVertexArray(disk1.VAO);
    glDrawArrays(GL_TRIANGLES, 0, disk1.buffer.size()/3);
}

void Jarvis()
{
    //smooth color change
    static float colVar = 0.0f;
    textCol.z = fabs(sin(colVar));
    colVar += 0.01f;
    if (colVar > 100.0f)
        colVar = 0.0f;

    glUseProgram(shadProgText);
    textColLoc = glGetUniformLocation(shadProgText, "textCol");
    glUniform3f(textColLoc, textCol.x, textCol.y, textCol.z);
    RenderText("JARVIS", -0.51f, -0.1f, 0.006f);
}

void Arcs()
{
    glUseProgram(shadProgArcs);
    auxCol = arcCol;
    auxColLoc = glGetUniformLocation(shadProgArcs, "auxCol");
    glUniform3f(auxColLoc, auxCol.x, auxCol.y, auxCol.z);

    model = glm::mat4(1.0f);
    model = glm::rotate(model, arc1.rotAng, glm::vec3(0.0f,0.0f,1.0f));
    modelLoc = glGetUniformLocation(shadProgArcs, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    arc1.rotAng += arc1.rotAngStep;
    if (arc1.rotAng >= 2.0f*(float)M_PI)
        arc1.rotAng = 0.0f;
    glBindVertexArray(arc1.VAO);
    glDrawArrays(GL_LINE_STRIP, 0, arc1.buffer.size()/3);

    model = glm::mat4(1.0f);
    model = glm::rotate(model, arc2.rotAng, glm::vec3(0.0f,0.0f,1.0f));
    modelLoc = glGetUniformLocation(shadProgArcs, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    arc2.rotAng -= arc2.rotAngStep;
    if (arc2.rotAng <= -2.0f*(float)M_PI)
        arc2.rotAng = 0.0f;
    glBindVertexArray(arc2.VAO);
    glDrawArrays(GL_LINE_STRIP, 0, arc2.buffer.size()/3);

    model = glm::mat4(1.0f);
    model = glm::rotate(model, arc3.rotAng, glm::vec3(0.0f,0.0f,1.0f));
    modelLoc = glGetUniformLocation(shadProgArcs, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    arc3.rotAng -= arc3.rotAngStep;
    if (arc3.rotAng <= -2.0f*(float)M_PI)
        arc3.rotAng = 0.0f;
    glBindVertexArray(arc3.VAO);
    glDrawArrays(GL_LINE_STRIP, 0, arc3.buffer.size()/3);

    model = glm::mat4(1.0f);
    model = glm::rotate(model, arc4.rotAng, glm::vec3(0.0f,0.0f,1.0f));
    modelLoc = glGetUniformLocation(shadProgArcs, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    arc4.rotAng += arc4.rotAngStep;
    if (arc4.rotAng >= 2.0f*(float)M_PI)
        arc4.rotAng = 0.0f;
    glBindVertexArray(arc4.VAO);
    glDrawArrays(GL_LINE_STRIP, 0, arc4.buffer.size()/3);
}

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    Disk();
    Jarvis();
    Arcs();
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
    glutInit(&argc, argv);
    glutInitContextVersion(3,3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
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
    glutFullScreen();
    glutMainLoop();
    return 0;
}
