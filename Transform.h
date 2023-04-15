#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>


// This class represents local coordinate system of an object

// initial Euler angles 
// initial _YAW set to -90.0f to make objects face Z coordinate (in this case - front of the object faces -Z axis - object "looks into screen"
const float _PITCH = 0.0f;
const float _YAW = -90.0f;
const float _ROLL = 0.0f;

// declaractrion of possible move() inputs
enum Move_direction {
    M_FORWARD,
    M_BACKWARD,
    M_LEFT,
    M_RIGHT,
    M_RISE,
    M_DIVE
};

class Transform
{
public:
    // Attributes
    glm::vec3 Position;
    
    // XYZ axes and world Y axis
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    
    
    // euler Angles
    glm::vec3 EulerAngles;
    float Pitch;
    float Yaw;
    float Roll;


    // scale
    glm::vec3 scale;
    float scaleX;
    float scaleY;
    float scaleZ;

    // constructor with default settings
    Transform(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = _YAW, float pitch = _PITCH, float roll = _ROLL)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Roll = roll;
        Pitch = pitch;
        
        EulerAngles.x = Pitch;
        EulerAngles.y = Yaw;
        EulerAngles.z = Roll;

        updateVectors();
    }

    // sets current position of transform
    void SetPosition(glm::vec3 position)
    {
        Position = position;

    }

    // move transform on X/Z axis: M_FORWARD - local X etc.
    void Move(Move_direction direction, float distance)
    {
        if (direction == M_FORWARD)
            Position += Front * distance;
        if (direction == M_BACKWARD)
            Position -= Front * distance;
        if (direction == M_LEFT)
            Position -= Right * distance;
        if (direction == M_RIGHT)
            Position += Right * distance;
    }

    // move transform by vector
    void Move(glm::vec3 m)
    {
        Position += m;
    }

    // set rotation of transform
    void SetRotation(glm::vec3 rotation)
    {
        Pitch = rotation.x;
        Yaw = rotation.y;
        Roll = rotation.z;

        EulerAngles.x = Pitch;
        EulerAngles.y = Yaw;
        EulerAngles.z = Roll;

        updateVectors();
    }

    // set rotation of transform
    void SetRotation(float pitch, float yaw, float roll)
    {
        Pitch = pitch;
        Yaw = yaw;
        Roll = roll;

        EulerAngles.x = Pitch;
        EulerAngles.y = Yaw;
        EulerAngles.z = Roll;

        updateVectors();
    }

    // rotate transform by given vector of Euler angles
    void Rotate(glm::vec3 rotation)
    {
        Pitch += rotation.x;
        Yaw += rotation.y;
        Roll += rotation.z;

        EulerAngles.x = Pitch;
        EulerAngles.y = Yaw;
        EulerAngles.z = Roll;

        updateVectors();
    }
    

    void setScale(float x, float y, float z)
    {
        scaleX = x;
        scaleY = y;
        scaleZ = z;
        scale = glm::vec3(x,y,z);
    }

    void setScale(glm::vec3 s)
    {
        scale.x = s.x;
        scale.y = s.y;
        scale.z = s.z;
        scaleX = s.x;
        scaleY = s.y;
        scaleZ = s.z;
    }


private:

    // calculate local axes with current Euler angles
    void updateVectors()
    {

        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));

        //include Roll - rotation around Front axis -- needs to be tested with drawLine functionality
        glm::mat4 roll_mat = glm::rotate(glm::mat4(1.0f), glm::radians(Roll), Front);
        Up =  glm::normalize(glm::mat3(roll_mat) * Up);
        Right = glm::normalize(glm::cross(Front, Up));

    }


};
#endif

