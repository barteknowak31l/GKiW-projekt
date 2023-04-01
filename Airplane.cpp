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
	isRotatePressed =  (direction == M_RIGHT || direction == M_LEFT);		


	float velocity = speed * deltaTime;

	if (direction == M_FORWARD || direction == M_BACKWARD)
		transform.Move(direction, velocity);

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

	if (yaw > yawMax)
		yaw = 0;
	if (yaw < -yawMax)
		yaw = 0;

	//update camera position - camera.setPosition(position+ offset)

	
	if (isPlayer)
	{


		if(firstPerson)
			camera->setRoll(yaw);

			

		//transform.SetRotation(glm::vec3(PITCH, std::clamp(transform.Yaw,-360.0f,360.0f), ROLL));
		camera->parentPosition = glm::vec3(transform.Position.x, transform.Position.y,transform.Position.z);

		camera->SetPosition(transform.Position + cameraOffset.x * transform.Right + cameraOffset.y * transform.Up + cameraOffset.z * transform.Front);

	}

	transform.SetRotation(glm::vec3(PITCH, YAW + yaw ,ROLL));


}

void Airplane::onMovementRelease(Move_direction dir)
{
	if (!isPlayer) return;


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

	if (changeYaw && !isRotatePressed)
	{

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
	}

	if (abs(yawAnimation) <=0.1)
	{
		yawAnimation = 0;
		incrementYaw = false;
		decrementYaw = false;
	}




}

void Airplane::reset(glm::vec3 offset)
{
	transform.SetPosition(offset);
	transform.SetRotation(glm::vec3(PITCH, YAW, ROLL));
	camera->parentPosition = glm::vec3(transform.Position.x, transform.Position.y, transform.Position.z);
	camera->SetPosition(transform.Position + cameraOffset.x * transform.Right + cameraOffset.y * transform.Up + cameraOffset.z * transform.Front);
	camera->setRotation(PITCH, YAW, ROLL);
	yawAnimation = 0;
	yaw = 0;
}

void Airplane::setOffset(glm::vec3 offset)
{
	transform.SetPosition(offset);
}