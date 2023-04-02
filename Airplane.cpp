#include "Airplane.h"




Airplane::Airplane(std::string path,glm::vec3 pos, float spd)
{
	model = new Model(path);
	transform.Position = pos;
	speed = spd;
	isPlayer = false;
	yaw = 0.0f;

	reset(transform.Position);
}

Airplane::Airplane(std::string path, Camera* _camera, glm::vec3 pos, float spd, bool fp)
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
	if (direction == M_LEFT)
		movement -= transform.Right * velocity;
	if (direction == M_RIGHT)
		movement += transform.Right * velocity;

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


	//probably not best solution 
	//if (yaw >= yawMax)
	//	yaw = 0;
	//if (yaw <= -yawMax)
	//	yaw = 0;

	// THIS IS FOR PLAYER ONLY - involves camera movement
	handleCamera();

	transform.SetRotation(glm::vec3(PITCH, YAW + yaw ,ROLL));


}

void Airplane::onMovementRelease(Move_direction dir)
{
	if (!isPlayer) return;

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
	std::cout << yawAnimation << std::endl;
	//transform.DrawLines();
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
			camera->parentPosition = camera->Position + camera->Front;
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
			camera->parentPosition = camera->Position + camera->Front;
		}
		else
		{
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

	// place AirPlane on given position with no rotation
	transform.SetPosition(pos);
	transform.SetRotation(glm::vec3(PITCH, YAW, ROLL));
	
	// adjust camera to follow the object
	if (firstPerson)
	{
		camera->setRotation(transform.Pitch, transform.Yaw, yawAnimation);
		camera->parentPosition = camera->Position + camera->Front;
	}
	else
	{
		camera->parentPosition = glm::vec3(transform.Position.x, transform.Position.y, transform.Position.z);
		camera->setRotation(PITCH, YAW, ROLL);
	}
	camera->SetPosition(transform.Position + cameraOffset.x * transform.Right + cameraOffset.y * transform.Up + cameraOffset.z * transform.Front);



}
