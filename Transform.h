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

const float _PITCH = 0.0f;
const float _YAW = -90.0f;
const float _ROLL = 0.0f;

enum Move_direction {
    M_FORWARD,
    M_BACKWARD,
    M_LEFT,
    M_RIGHT
};

class Transform
{
public:
    // Attributes
    glm::vec3 Position;
    
    // Space vectors
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    
    
    // euler Angles
    float Pitch;
    float Yaw;
    float Roll;

    Transform(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float roll = _ROLL) : Front(glm::vec3(0.0f, 0.0f, -1.0f))
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Roll = roll;
        Pitch = pitch;
        
        updateVectors();
    }

    void SetPosition(glm::vec3 position)
    {
        Position = position;

    }

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

    void SetRotation(glm::vec3 rotation)
    {
        Pitch = rotation.x;
        Yaw = rotation.y;
        Roll = rotation.z;
        updateVectors();
    }


private:
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

        glm::mat4 roll_mat = glm::rotate(glm::mat4(1.0f), glm::radians(Roll), Front);
        Up =  glm::normalize(glm::mat3(roll_mat) * Up);
    }

};
#endif

