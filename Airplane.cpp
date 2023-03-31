#include "Airplane.h"




Airplane::Airplane(Model* _model,glm::vec3 pos, float spd)
{
	model = *_model;
	transform.Position = pos;
	speed = spd;
	isPlayer = false;
	yaw = 0.0f;
;


}

Airplane::Airplane(Model* _model, Camera* _camera, glm::vec3 pos, float spd, bool fp)
{
	model = *_model;
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

}


void Airplane::processMovement(Move_direction direction, float deltaTime)
{
	//changeYaw = false;
	//decrementYaw = false;
	//incrementYaw = false;

	float velocity = speed * deltaTime;

	if(direction == M_FORWARD || direction == M_BACKWARD)
	transform.Move(direction, velocity);


	if (direction == M_RIGHT)
		yaw += yawDelta * deltaTime;
	if (direction == M_LEFT)
		yaw -= yawDelta * deltaTime;

	if (yaw > yawMax)
		yaw = yawMax;
	if (yaw < -yawMax)
		yaw = -yawMax;


	//update camera position - camera.setPosition(position+ offset)

	
	if (isPlayer)
	{
		transform.SetRotation(glm::vec3(PITCH, YAW, ROLL));

		if(firstPerson)
		camera->setRoll(yaw);
		
		camera->SetPosition(transform.Position + cameraOffset.x * transform.Right + cameraOffset.y * transform.Up + cameraOffset * transform.Front);

	}


	transform.SetRotation(glm::vec3(PITCH, YAW + yaw, ROLL));

}

void Airplane::onMovementRelease(Move_direction dir)
{
	if (!isPlayer) return;

	if (dir == M_LEFT || dir == M_RIGHT)
	{
		changeYaw = true;

		decrementYaw = yaw > 0;
		incrementYaw = yaw < 0;

		// transform.SetRotation(glm::vec3(PITCH, YAW, ROLL));
		// camera->SetPosition(transform.Position + cameraOffset.x * transform.Right + cameraOffset.y * transform.Up + cameraOffset * transform.Front);
	}
}

void Airplane::update(float deltaTime)
{
	if (changeYaw)
	{

		if (decrementYaw)
		{
			yaw -= yawDelta * deltaTime;
			decrementYaw = yaw > 0;
			changeYaw = yaw > 0;
		}

		if (incrementYaw)
		{
			yaw += yawDelta * deltaTime;
			incrementYaw = yaw < 0;
			changeYaw = yaw < 0;
		}

		if (isPlayer)
		{
			transform.SetRotation(glm::vec3(PITCH, YAW, ROLL));
			if (firstPerson)
				camera->setRoll(yaw);
			camera->SetPosition(transform.Position + cameraOffset.x * transform.Right + cameraOffset.y * transform.Up + cameraOffset * transform.Front);
		}
		transform.SetRotation(glm::vec3(PITCH, YAW + yaw, ROLL));

	}
}