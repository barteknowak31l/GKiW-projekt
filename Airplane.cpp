#include "Airplane.h"
#include <cmath>
#include <algorithm>

Airplane::Airplane(std::string path,glm::vec3 pos, float spd) :GameObject(path, pos)
{
	//model = new Model(path);
	transform.Position = pos;
	speed = spd;
	isPlayer = false;
	yaw = 0.0f;
	flipPitch = false;

	reset();
}

Airplane::Airplane(std::string path, Camera* _camera, glm::vec3 pos, float spd, bool fp, bool _flipPitch):GameObject(path,pos)
{
	camera = _camera;
	speed = spd;
	firstPerson = fp;
	if (fp)
		cameraOffset = firstPersonCamOffset;
	else
		cameraOffset = thirdPersonCamOffset;


	flipPitch = _flipPitch;

	isPlayer = true;
	yaw = 0.0f;

	camera->SetPosition(transform.Position + cameraOffset.x * transform.Right + cameraOffset.y * transform.Up + cameraOffset.z * transform.Front);
	reset();
}


void Airplane::processMovement(Move_direction direction, float deltaTime)
{
	//THIS PART SHOULD WORK ALSO FOR NON PLAYER AIRPLANES  - need to be tested
	
	// check if there's rotation input
	isRotatePressed =  (direction == M_RIGHT || direction == M_LEFT);		


	float velocity = speed * deltaTime;

	// move airplane
	glm::vec3 movement = glm::vec3(0, 0, 0);
	if (direction == M_FORWARD)
		movement += transform.Front * velocity;
	if (direction == M_BACKWARD)
		movement -= transform.Front * velocity;





	transform.Move(movement);

	// alter yaw data
	if (direction == M_RIGHT)
	{
		yawAnimation += yawAnimationDelta * deltaTime;
		yaw += yawDelta * deltaTime;
	}
	if (direction == M_LEFT)
	{
			yawAnimation -= yawAnimationDelta * deltaTime;
			yaw -= yawDelta * deltaTime;
	}

	if (yawAnimation > yawAnimationMax)
		yawAnimation = yawAnimationMax;
	if (yawAnimation < -yawAnimationMax)
		yawAnimation = -yawAnimationMax;


	// alter pitch data
	if (direction == M_DIVE)
	{
		//yawAnimation += yawAnimationDelta * deltaTime;
		pitch += yawDelta * deltaTime;
	}
	if (direction == M_RISE)
	{
		//yawAnimation -= yawAnimationDelta * deltaTime;
		pitch -= yawDelta * deltaTime;
	}

	float max = 360.0f;
	if (pitch > max)
	{
		pitch = 0;
	}
	if (pitch < -max)
	{
		pitch = 0;
	}



	// THIS IS FOR PLAYER ONLY - involves camera movement
	if(isPlayer)
		handleCamera();


	// finally, after all calculations apply rotation to transform
	if (flipPitch)
	{
		transform.SetRotation(glm::vec3(PITCH - pitch, YAW + yaw, ROLL));
	}
	else
		transform.SetRotation(glm::vec3(PITCH + pitch, YAW + yaw, ROLL));




}

void Airplane::onMovementRelease(Move_direction dir)
{
	if (!isPlayer) return;

	// if no rotation input - set variables to apply back-to-normal-state animation
	if (dir == M_LEFT || dir == M_RIGHT)
	{
		isRotatePressed = false;
		changeYaw = true;

		// these two are complementary - one set to true decides about animation direction
		decrementYaw = yawAnimation > 0;
		incrementYaw = yawAnimation < 0;
	}
}

void Airplane::update(float deltaTime)
{
	handleTurnAnimation(deltaTime);
}


void Airplane::handleTurnAnimation(float deltaTime)
{

	// happens when airplane is still rotated around Z and no rotation input from keyboard
	if (changeYaw && !isRotatePressed)
	{
		// at the end, yawAnimation should be 0, so decrement or increment if necessary
		//

		if (decrementYaw)
		{
			yawAnimation -= yawAnimationDelta * deltaTime;

			// check if animation should stop
			decrementYaw = yawAnimation > 0;
			changeYaw = yawAnimation > 0;
		}

		if (incrementYaw)
		{
			yawAnimation += yawAnimationDelta * deltaTime;
			
			// check if animation should stop
			incrementYaw = yawAnimation < 0;
			changeYaw = yawAnimation < 0;
		}

		// first person camera depends on yawAnimation, so update it too
		if (firstPerson)
		{
			camera->setRotation(transform.Pitch, transform.Yaw, yawAnimation);
			camera->parentPosition = camera->transform.Position + camera->transform.Front;
		}

	}
	// to avoid floating point accuracy problems, when close to 0, just set it to 0
	if (abs(yawAnimation) <= 0.1)
	{
		yawAnimation = 0;
		incrementYaw = false;
		decrementYaw = false;
	}
}

void Airplane::handleCamera()
{
	// THIS IS FOR PLAYER ONLY - involves camera movement
	if (isPlayer)
	{
		if (camera->enableMovement)
			return;

		// turning camera together with airplane - gives effect of rolling head
		if (firstPerson)
		{
			camera->setRotation(transform.Pitch, transform.Yaw, yawAnimation);
			camera->parentPosition = camera->transform.Position + camera->transform.Front;
		}
		else // third person camera
		{
			// flip camera Up vector to avoid screen rotating while making barrel roll
			if (transform.Pitch >= 90.0f && transform.Pitch <= 270.0f)
			{
				camera->flipY = true;
			}
			else if (transform.Pitch >= -270.0f && transform.Pitch <= -90.0f)
			{
				camera->flipY = true;
			}
			else
			{
				camera->flipY = false;
			}

			// update camera LookAt 2nd argument - point to look at
			camera->parentPosition = glm::vec3(transform.Position.x, transform.Position.y, transform.Position.z);
		}

		// update camera position
		camera->SetPosition(transform.Position + cameraOffset.x * transform.Right + cameraOffset.y * transform.Up + cameraOffset.z * transform.Front);

	}
}

void Airplane::reset()
{

	// reset vars responsible for turning around
	yawAnimation = 0;
	yaw = 0;
	pitch = 0;


	// make sure camera Up vector is not flipped
	camera->flipY = false;

	// place AirPlane on given position with no rotation
	transform.SetPosition(resetPosition);
	transform.SetRotation(glm::vec3(PITCH, YAW, ROLL));

	if (isPlayer)
	{
		// adjust camera to follow the object
		if (firstPerson)
		{
			camera->setRotation(transform.Pitch, transform.Yaw, yawAnimation);
			camera->parentPosition = camera->transform.Position + camera->transform.Front;
		}
		else // third person
		{
			camera->parentPosition = glm::vec3(transform.Position.x, transform.Position.y, transform.Position.z);
			camera->setRotation(PITCH, YAW, ROLL);
		}

		// set camera position
		camera->SetPosition(transform.Position + cameraOffset.x * transform.Right + cameraOffset.y * transform.Up + cameraOffset.z * transform.Front);
	}
}

glm::mat4 Airplane::calcModelMatrix(glm::mat4 model, float scale)
{
	float rollRot = std::clamp(-glm::radians(YAW + yawAnimation + 90.0f), glm::radians(-30.0f), glm::radians(30.0f));
	float pitchRot = transform.Pitch;

	
	model = glm::mat4(1.0f);

	model = glm::translate(model, transform.Position);
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	// turning animation 
	model = glm::rotate(model, -glm::radians(transform.Yaw + 90.0f), glm::vec3(.0f, .0f, 1.0f));
	model = glm::rotate(model, glm::radians(pitchRot), glm::vec3(.0f, 1.0f, 0.0f));

	model = glm::rotate(model, rollRot, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(scale, scale, scale));

	return model;
}