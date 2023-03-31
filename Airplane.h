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


const glm::vec3 thirdPersonCamOffset = glm::vec3(0.0f, 1.0f, -4.0f);
const glm::vec3 firstPersonCamOffset = glm::vec3(0.0f, .2f, 0.5f);

class Airplane
{
public:

	Transform transform;
	Model model;
	Camera* camera;
	glm::vec3 cameraOffset;
	bool isPlayer;
	bool firstPerson = false;

	float speed;

	//constructor of airplane without camera
	Airplane(Model* _model,glm::vec3 pos, float spd);

	//constructor with camera - this one is controlled by player
	Airplane(Model* _model, Camera* _camera, glm::vec3 pos, float spd,bool fp);


	void processMovement(Move_direction direction, float deltaTime);
	void onMovementRelease(Move_direction dir);

	void update(float deltaTime);

private:
	float yaw = 0.0f;
	float yawDelta = 150.0f;
	float yawMax = 30.0f;
	bool decrementYaw = false;
	bool incrementYaw = false;
	bool changeYaw = false;

};
#endif


