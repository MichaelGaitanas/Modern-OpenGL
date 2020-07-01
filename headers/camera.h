//A header to manipulate the first person 3D camera calculations.

#ifndef CAMERA_H
#define CAMERA_H

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

class camera
{
public:
    glm::vec3 pos; //camera position in 3D space
    glm::vec3 front; //camera's front direction
    glm::vec3 up; //camera's up direction
    float transSpeed, rotSpeed; //camera's translate and rotate rotate speed
    float speedMultiplyFactor;
    float yaw, pitch; //camera's right-left and up-down angles
    unsigned int posLoc;
    //camera constructor
    camera(glm::vec3 initPos = glm::vec3(0.0f,0.0f,0.0f),
           float initTransSpeed = 0.0f, float initRotSpeed = 0.0f, float initYaw = 90.0f, float initPitch = 0.0f)
    {
        pos = initPos;
        front = glm::vec3(0.0f,1.0f,0.0f); //y-axis is always front-directed by default
        up = glm::vec3(0.0f,0.0f,1.0f); //z-axis is always up-directed by default
        transSpeed = initTransSpeed;
        rotSpeed = initRotSpeed;
        speedMultiplyFactor = 1.0f; //In case you don't calculate this value in the .cpp file
        yaw = initYaw; //yaw = 90 by default, but it can be set to whatever you want
        pitch = initPitch; //pitch = 90 by default, but it can be set to whaterver you want
    }

    //you program the means that will move the camera (keyboard, mouse, controller, etc)
    //and the camera will move in accordance with the following functions
    ////////////////////////////////////////////////////////////////////////////
    void TranslateFront()
    {
        pos += transSpeed*speedMultiplyFactor*front;
    }
    void TranslateBack()
    {
        pos -= transSpeed*speedMultiplyFactor*front;
    }
    void TranslateRight()
    {
        pos += transSpeed*speedMultiplyFactor*glm::normalize(glm::cross(front,up));
    }
    void TranslateLeft()
    {
        pos -= transSpeed*speedMultiplyFactor*glm::normalize(glm::cross(front,up));
    }

    void RotateRight()
    {
        yaw -= rotSpeed*speedMultiplyFactor;
    }
    void RotateLeft()
    {
        yaw += rotSpeed*speedMultiplyFactor;
    }
    void RotateUp()
    {
        pitch += rotSpeed*speedMultiplyFactor;
    }
    void RotateDown()
    {
        pitch -= rotSpeed*speedMultiplyFactor;
    }
    ////////////////////////////////////////////////////////////////////////////

    void UpdateVectors()
    {
        //bound the yaw
        if (fabs(yaw) >= 360.0f)
            yaw = 0.0f;

        //bound the pitch
        if (pitch >= 89.0f)
            pitch = 89.0f;
        else if (pitch <= -89.0f)
            pitch = -89.0f;

        front.x = cos(glm::radians(pitch))*cos(glm::radians(yaw));
        front.y = cos(glm::radians(pitch))*sin(glm::radians(yaw));
        front.z = sin(glm::radians(pitch));
        front = glm::normalize(front);
    }

    glm::mat4 Perspective(float aspectRatio)
    {
        return glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);
    }
    glm::mat4 View()
    {
        return glm::lookAt(pos, pos + front, up);
    }
};
#endif
