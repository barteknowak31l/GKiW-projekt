#ifndef CAMERA_H
#define CAMERA_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Transform.h"


// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float ROLL = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;



class Camera
{
public:


    Transform transform;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // flip Up vector to avoid camera flipping when rotating around X (pitch)
    bool flipY = false;


    glm::vec3 parentPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    bool enableMovement;

    Camera()
    {
        ;
    }
    // constructor with vectors
    Camera(bool movement,glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float roll = ROLL) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        transform.Position = position;
        transform.WorldUp = up;
        transform.SetRotation(pitch, yaw, roll);

        enableMovement = movement;

    }
    // constructor with scalar values
    Camera(bool movement,float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float roll = ROLL) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        transform.Position = glm::vec3(posX, posY, posZ);
        transform.WorldUp = glm::vec3(upX, upY, upZ);
        transform.SetRotation(pitch, yaw, roll);
        enableMovement = movement;

    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {

        if (flipY)
        {
            return glm::lookAt(transform.Position, parentPosition, -transform.Up);
        }
        else
        return glm::lookAt(transform.Position,parentPosition, transform.Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {

        if (!enableMovement) return;

        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            transform.Position += transform.Front * velocity;
        if (direction == BACKWARD)
            transform.Position -= transform.Front * velocity;
        if (direction == LEFT)
            transform.Position -= transform.Right * velocity;
        if (direction == RIGHT)
            transform.Position += transform.Right * velocity;

        //Position.y = 0.0f; // <-- this one-liner keeps the user at the ground level (xz plane)
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        if (!enableMovement) return;

        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        transform.Yaw += xoffset;
        transform.Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (transform.Pitch > 89.0f)
                transform.Pitch = 89.0f;
            if (transform.Pitch < -89.0f)
                transform.Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        transform.SetRotation(transform.EulerAngles);

    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }



    //set camera position directly
    void SetPosition(glm::vec3 pos)
    {
        transform.Position = pos;
    }

    //used to rotate camera with airplane
    void setRoll(float r)
    {
        transform.Roll = ROLL + r;
        transform.SetRotation(transform.EulerAngles);

    }


    void setRotation(float p, float y, float r)
    {
        transform.Pitch = p;
        transform.Yaw = y;
        transform.Roll = r;
        transform.SetRotation(transform.EulerAngles);

    }


};
#endif