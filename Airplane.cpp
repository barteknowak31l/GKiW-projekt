#include "Airplane.h"


#include <cmath>

Airplane::Airplane(std::string path,glm::vec3 pos, float spd)
{
	model = new Model(path);
	transform.Position = pos;
	speed = spd;
	isPlayer = false;
	yaw = 0.0f;
	flipPitch = false;

	reset(transform.Position);
}

Airplane::Airplane(std::string path, Camera* _camera, glm::vec3 pos, float spd, bool fp, bool _flipPitch)
{
	model = new Model(path);
	camera = _camera;
	transform.Position = pos;
	speed = spd;
	firstPerson = fp;
	if (fp)
		cameraOffset = firstPersonCamOffset;
	else
		cameraOffset = thirdPersonCamOffset;


	flipPitch = _flipPitch;

	isPlayer = true;
	yaw = 0.0f;

	camera->SetPosition(transform.Position + cameraOffset.x * transform.Right + cameraOffset.y * transform.Up + cameraOffset * transform.Front);
	reset(transform.Position);
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

	//float angle = acos(glm::dot(transform.Position, transform.Position+cameraOffset) / (glm::length(transform.Position) * glm::length(transform.Position + cameraOffset) ) );
	//angle = glm::degrees(angle);

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
	handleCamera();

	if (flipPitch)
	{
		transform.SetRotation(glm::vec3(PITCH - pitch, YAW + yaw, ROLL));
	}
	else
		transform.SetRotation(glm::vec3(PITCH + pitch, YAW + yaw, ROLL));




}

void Airplane::onMovementRelease(Move_direction dir)
{
	//if (!isPlayer) return;

	// if no rotation input - set variables to start back-to-normal-state animation
	if (dir == M_LEFT || dir == M_RIGHT)
	{
		isRotatePressed = false;
		changeYaw = true;
		decrementYaw = yawAnimation > 0;
		incrementYaw = yawAnimation < 0;
	}
}

void Airplane::update(float deltaTime)
{
	std::cout << pitch << std::endl;
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
			decrementYaw = yawAnimation > 0;
			changeYaw = yawAnimation > 0;
		}

		if (incrementYaw)
		{
			yawAnimation += yawAnimationDelta * deltaTime;
			incrementYaw = yawAnimation < 0;
			changeYaw = yawAnimation < 0;

		}

		// fp camera depends on yawAnimation, so update it too
		if (firstPerson)
		{
			camera->setRotation(transform.Pitch, transform.Yaw, yawAnimation);
			camera->parentPosition = camera->transform.Position + camera->transform.Front;
		}

	}
	// to avoid floating point accuracy problems
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
		// turning camera together with airplane - gives effect of rolling head(?)
		if (firstPerson)
		{
			camera->setRotation(transform.Pitch, transform.Yaw, yawAnimation);
			camera->parentPosition = camera->transform.Position + camera->transform.Front;
		}
		else
		{
	
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

			camera->parentPosition = glm::vec3(transform.Position.x, transform.Position.y, transform.Position.z);
		}
		camera->SetPosition(transform.Position + cameraOffset.x * transform.Right + cameraOffset.y * transform.Up + cameraOffset.z * transform.Front);

	}
}

void Airplane::reset(glm::vec3 pos)
{

	// reset vars responsible for turning around
	yawAnimation = 0;
	yaw = 0;
	pitch = 0;

	// place AirPlane on given position with no rotation
	transform.SetPosition(pos);
	transform.SetRotation(glm::vec3(PITCH, YAW, ROLL));
	

	if (isPlayer)
	{
		// adjust camera to follow the object
		if (firstPerson)
		{
			camera->setRotation(transform.Pitch, transform.Yaw, yawAnimation);
			camera->parentPosition = camera->transform.Position + camera->transform.Front;
		}
		else
		{
			camera->parentPosition = glm::vec3(transform.Position.x, transform.Position.y, transform.Position.z);
			camera->setRotation(PITCH, YAW, ROLL);
		}
		camera->SetPosition(transform.Position + cameraOffset.x * transform.Right + cameraOffset.y * transform.Up + cameraOffset.z * transform.Front);
	}



}
