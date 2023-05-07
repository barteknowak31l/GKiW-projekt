#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "Shader.h"


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

        swapY = false;

        updateVectors();
        setup();
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

        constrainAngles();

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

        constrainAngles();

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


        constrainAngles();


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


    void draw(Shader& shader, float scale)
    {
        shader.use();
        glBindVertexArray(VAO);


        //middle - white
        glm::mat4 middle = glm::mat4(1.0f);
        middle = glm::translate(middle, Position);
        middle = glm::rotate(middle, -glm::radians(Yaw), glm::vec3(0.0, 1.0, 0.0));
        middle = glm::rotate(middle, glm::radians(Pitch), glm::vec3(0.0, 0.0, 1.0));
        middle = glm::rotate(middle, glm::radians(Roll), glm::vec3(1.0, 0.0, 0.0));
        middle = glm::scale(middle, glm::vec3(0.5) * scale);
        shader.setMat4("model", middle);
        shader.setVec3("color", glm::vec3(1.0, 1.0, 1.0));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // FRONT - RED
        glm::mat4 front = glm::mat4(1.0f);
        front = glm::translate(front, Position + Front * scale);
        front = rotateToFrontMatrix(front);
        front = glm::scale(front, glm::vec3(glm::length(Front) * scale * 2.0f ,0.5f ,0.5f));
        shader.setMat4("model", front);
        shader.setVec3("color", glm::vec3(1.0,0.0,0.0));
        glDrawArrays(GL_TRIANGLES, 0, 36);


        ////////// RIGHT - GREEN
        glm::mat4 right = glm::mat4(1.0f);;
        right = glm::translate(right, Position + Right * scale);
        right = rotateToRightMatrix(right);
        right = glm::scale(right, glm::vec3(glm::length(Right) * scale * 2.0f, 0.5f, 0.5f));
        shader.setMat4("model", right);
        shader.setVec3("color", glm::vec3(0.0, 1.0, 0.0));
        glDrawArrays(GL_TRIANGLES, 0, 36);


        //////////// UP - BLUE
        glm::mat4 up = glm::mat4(1.0f);;
        up = glm::translate(up,Position + Up * scale);
        up = rotateToUpMatrix(up);
        up = glm::scale(up, glm::vec3(0.5f, glm::length(Up) * scale * 2.0f, 0.5f));
        shader.setMat4("model", up);
        shader.setVec3("color", glm::vec3(0.0, 0.0, 1.0));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        
        glBindVertexArray(0);
    }


    glm::mat4 rotateToFrontMatrix(glm::mat4 &model)
    {
        model = glm::rotate(model, -glm::radians(Yaw), glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, glm::radians(Pitch), glm::vec3(0.0, 0.0, 1.0));
        model = glm::rotate(model, glm::radians(Roll), glm::vec3(1.0, 0.0, 0.0));
        return model;
    }


    glm::mat4 rotateToRightMatrix(glm::mat4& model)
    {
        model = glm::rotate(model, -glm::radians(Yaw + 90.0f), glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, glm::radians(Pitch), glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, -glm::radians(Roll), glm::vec3(0.0, 0.0, 1.0));

        return model;
    }

    glm::mat4 rotateToUpMatrix(glm::mat4& model)
    {
        model = glm::rotate(model, -glm::radians(Yaw), glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, glm::radians(Pitch), glm::vec3(0.0, 0.0, 1.0));
        model = glm::rotate(model, glm::radians(Roll), glm::vec3(1.0, 0.0, 0.0));

        return model;
    }

protected:
    // vertex data to represent a cube
    float cube[288] = {
        // positions
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

private:

    // used for drawing
    unsigned int VAO, VBO;
    bool swapY;

    //set VAO, VBO
    void setup()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        //unbind buffers
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

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


        swapY = (Pitch > 90.0f && Pitch < 270.0f) || (Pitch < -90.0f && Pitch > -270.0f);

        if (swapY)
        {
            Right *= -1.0f;
            Up *= -1.0f;
        }


    }


    void constrainAngles()
    {
        if (Pitch > 0.0f)
        {
            Pitch = std::fmod(Pitch, 360.0f);
        }
        else
        {
            Pitch = std::fmod(Pitch, -360.0f);
        }
        if (Yaw > 0.0f)
        {
            Yaw = std::fmod(Yaw, 360.0f);
        }
        else
        {
            Yaw = std::fmod(Yaw, -360.0f);
        }
        if (Roll > 0.0f)
        {
            Roll = std::fmod(Roll, 360.0f);
        }
        else
        {
            Roll = std::fmod(Roll, -360.0f);
        }
    }


};
#endif

