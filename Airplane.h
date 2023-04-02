#ifndef AIRPLANE_H
#define AIRPLANE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "Model.h"
#include "Camera.h"
#include "Transform.h"

// attached camera positions
const glm::vec3 thirdPersonCamOffset = glm::vec3(0.0f, 0.0f, -4.0f);
const glm::vec3 firstPersonCamOffset = glm::vec3(0.0f, -.2f, 0.5f);

class Airplane
{
public:

	Transform transform;
	Model* model;
	Camera* camera;


	// used for animating rotation on Z axis during turn
	float yawAnimation = 0.0f;
	
	// flying speed
	float speed;


	// constructor of airplane without camera
	Airplane(std::string path,glm::vec3 pos, float spd);

	// constructor with camera - this one is controlled by player
	Airplane(std::string path, Camera* _camera, glm::vec3 pos, float spd,bool fp, bool _flipPitch);

	// movemment handlers
	void processMovement(Move_direction direction, float deltaTime);
	void onMovementRelease(Move_direction dir);

	// called each frame
	void update(float deltaTime);
	
	// sets default settings
	void reset(glm::vec3 offset);


private:

	bool isPlayer;
	bool firstPerson = false;

	// used for rotating transform when turning around
	//
	float yaw = 0.0f;
	float yawDelta = 60.0f;
	float yawMax = 360.0f;


	// used for rotating when rising and diving
	float pitch = 0.0f;
	bool flipPitch;

	// used to animate that turning - used in model matrix rotation (currently in main() function)
	//
	// maximum angle of rotation when turning (animation only)
	float yawAnimationMax = 30.0f;		
	
	// animation speed
	float yawAnimationDelta = 25.0f;
	
 	// flags to control animation
	bool decrementYaw = false;
	bool incrementYaw = false;
	bool changeYaw = false;
	bool isRotatePressed = false;

	// current offset (first/third person)
	glm::vec3 cameraOffset;

	// as name says
	void handleTurnAnimation(float deltaTime);
	void handleCamera();


};
#endif


