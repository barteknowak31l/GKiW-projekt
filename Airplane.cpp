#include "Airplane.h"
#include <cmath>
#include <algorithm>
#include "Grid.h"

extern float deltaTime;

Airplane::Airplane(std::string path,glm::vec3 pos, Grid* _grid, float spd, glm::vec3 scale, glm::vec3 flashlightColor, float _flashlightOffset, glm::vec3 colSize) :GameObject(path, pos,colSize)
{
	transform.setScale(scale);
	speed = spd;
	grid = _grid;
	isPlayer = false;
	yaw = 0.0f;
	flipPitch = false;

	flashlightOffset = _flashlightOffset;
	setupFlashlights(flashlightColor);
	
	reset();
}

Airplane::Airplane(std::string path, Camera* _camera, glm::vec3 pos, Grid* _grid, float spd, bool _flipPitch, glm::vec3 scale, glm::vec3 flashlightColor, float _flashlightOffset, glm::vec3 colSize): GameObject(path,pos,colSize)
{
	transform.setScale(scale);
	camera = _camera;
	speed = spd;
	grid = _grid;


	cameraOffset = thirdPersonCamOffset;


	flipPitch = _flipPitch;

	isPlayer = true;
	yaw = 0.0f;

	camera->SetPosition(transform.Position + cameraOffset.x * transform.Right + cameraOffset.y * transform.Up + cameraOffset.z * transform.Front);

	flashlightOffset = _flashlightOffset;
	setupFlashlights(flashlightColor);

	resetPosition = pos;
	reset();

	collider1 = transform.Position - transform.Up;
	collider2 = transform.Position + transform.Front;
}


void Airplane::processMovement(Move_direction direction, float deltaTime)
{

	// check for collision with terrain	
	float h1 = checkTerrainCollision(grid, collider1);
	float h2 = checkTerrainCollision(grid, collider2);

	bool h1collision;
	bool h2collision;


	// if a plane is under terrain (h1) or goes directly into terrain (h2) theres a collision
	if (transform.Position.y <= h1 && transform.Position.y <= h2)
	{
		collision = true;
		h1collision = true;
		h2collision = true;
	}
	else if (transform.Position.y <= h1)
	{
		collision = true;
		h1collision = true;
	}
	else if (transform.Position.y <= h2)
	{
		collision = true;
		h2collision = true;
	}
	else
	{
		collision = false;
		h1collision = false;
		h2collision = false;
	}


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

	// if there is collision - set airplane y to terrain y position
	if (collision)
	{
		if(h1collision)
		transform.SetPosition(glm::vec3(transform.Position.x, h1 + collider1Offset, transform.Position.z));
		else
			transform.SetPosition(glm::vec3(transform.Position.x, h2 + collider2Offset, transform.Position.z));

	}

	// alter yaw data - turn left or right
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

	// constrain the yaw animation - turning left/right
	if (yawAnimation > yawAnimationMax)
		yawAnimation = yawAnimationMax;
	if (yawAnimation < -yawAnimationMax)
		yawAnimation = -yawAnimationMax;


	// alter pitch data - rising - diving
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


	// constrain pitch
	float max = 360.0f;
	if (pitch > max)
	{
		pitch = 0;
	}
	if (pitch < -max)
	{
		pitch = 0;
	}



	handleCamera();


	// move colliders
	collider1 = transform.Position - transform.Up * collider1Offset;
	collider2 = transform.Position + transform.Front * collider2Offset;



	// finally, after all calculations finished, apply rotation to transform
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

		// these two are complementary - one set to true decides about animation direction (left to middle or right to middle)
		decrementYaw = yawAnimation > 0;
		incrementYaw = yawAnimation < 0;
	}
}

void Airplane::update(float deltaTime)
{
	handleTurnAnimation(deltaTime);
	handleFlashlight();
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

		// update camera LookAt 2nd argument - point to look at should equal this airplane position
		camera->parentPosition = glm::vec3(transform.Position.x, transform.Position.y, transform.Position.z);

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
		camera->parentPosition = glm::vec3(transform.Position.x, transform.Position.y, transform.Position.z);
		camera->setRotation(PITCH, YAW, ROLL);

		// set camera position
		camera->SetPosition(transform.Position + cameraOffset.x * transform.Right + cameraOffset.y * transform.Up + cameraOffset.z * transform.Front);
	}


	// move colliders
	collider1 = transform.Position - transform.Up * collider1Offset;
	collider2 = transform.Position + transform.Front * collider2Offset;

}

glm::mat4 Airplane::calcModelMatrix(glm::mat4 model)
{
	float rollRot = std::clamp(-glm::radians(YAW + yawAnimation), glm::radians(-yawAnimationMax), glm::radians(yawAnimationMax));
	float pitchRot = transform.Pitch;

	model = glm::translate(model, transform.Position);
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, -glm::radians(transform.Yaw + 90.0f), glm::vec3(.0f, .0f, 1.0f));

	// turning animation 
	model = glm::rotate(model, glm::radians(pitchRot), glm::vec3(.0f, 1.0f, 0.0f));
	model = glm::rotate(model, rollRot, glm::vec3(1.0f, 0.0f, 0.0f));

	model = glm::scale(model, glm::vec3(transform.scale.x, transform.scale.y, transform.scale.z));

	return model;
}

void Airplane::handleFlashlight()
{
	// set flashlights position related to airplane position
	leftFlashlight.position = transform.Position + transform.Front * 1.0f + transform.Right * -flashlightOffset;
	leftFlashlight.direction = transform.Front;

	rightFlashlight.position = transform.Position + transform.Front * 1.0f + transform.Right * flashlightOffset;
	rightFlashlight.direction = transform.Front;

}

void Airplane::setupFlashlights(glm::vec3 color)
{
	handleFlashlight();

	leftFlashlight.color = color;
	leftFlashlight.cutOff = glm::cos(glm::radians(6.5f));
	leftFlashlight.outerCutOff = glm::cos(glm::radians(10.5f));
	leftFlashlight.constant = 1.0f;
	leftFlashlight.linear = 0.022f;
	leftFlashlight.quadratic = 0.0019f;

	rightFlashlight.color = color;
	rightFlashlight.cutOff = glm::cos(glm::radians(6.5f));
	rightFlashlight.outerCutOff = glm::cos(glm::radians(10.5f));
	rightFlashlight.constant = 1.0f;
	rightFlashlight.linear = 0.022f;
	rightFlashlight.quadratic = 0.0019f;

}

float Airplane::checkTerrainCollision(Grid* grid, Transform collider)
{

	// pozycja colliderow szybowca
	float posX = collider.Position.x;
	float posZ = collider.Position.z;

	// rozmiar pojedynczego kwadratu grida
	float gridSquareSize = grid->WorldScale;

	// dla danych posX, posZ znajdz odpowiadajacy im kwadrat na gridzie
	int gridX = (int)std::floor(posX / gridSquareSize);
	int gridZ = (int)std::floor(posZ / gridSquareSize);

	// sprawdz czy znaleziony kwadrat na pewno nalezy do grida:
	if (gridX > grid->Width - 1 || gridX < 0 || gridZ > grid->Depth - 1 || gridZ < 0)
	{
		// szybowiec znajduje sie poza terenem
		return 0.0f;
	}

	// znajdz dokladna pozycje szybowca na wyznaczonym kwadracie: <0;1>
	float xCoord = fModulo(posX,gridSquareSize) / gridSquareSize;
	float zCoord = fModulo(posZ, gridSquareSize) / gridSquareSize;


	// ustal indeksy wierzcholkow tego kwadratu (kompatybilne z wektorem wierzcholkow w grid)
	int bottomLeftIndex = gridZ * grid->Width + gridX;
	int topLeftIndex = (gridZ + 1) * grid->Width + gridX;
	int topRightIndex = (gridZ + 1) * grid->Width + gridX + 1;
	int bottomRightIndex = (gridZ)*grid->Width + gridX + 1;



	// znajdz pozycje na kwadracie, ustal ktory to trojkat:
	float height;
	glm::vec3 bottomLeft;
	glm::vec3 topLeft;
	glm::vec3 topRight;
	glm::vec3 bottomRight;
	glm::vec2 planePos;

	if (xCoord < zCoord)
	{
		// top left triangle - wspolrzedne: (0,0) (0,1), (1,1)
		// barycentric dla wierzcholkow top left trojkata
		
		bottomLeft = grid->GetGridVertex(bottomLeftIndex).Pos;
		topLeft = grid->GetGridVertex(topLeftIndex).Pos;
		topRight = grid->GetGridVertex(topRightIndex).Pos;
		
		planePos = glm::vec2(xCoord, zCoord);
		height = barycentricInterpolation(glm::vec3(0,bottomLeft.y,0), glm::vec3(0,topLeft.y,1), glm::vec3(1,topRight.y,1), planePos);
		
	}
	else
	{
		// diagonal or bottom right triangle - wspolrzedne: (0,0) (1,1), (1,0)
		// barycentric dla wierzcholkow bottom right tr�jk�ta

		bottomLeft = grid->GetGridVertex(bottomLeftIndex).Pos;
		topRight = grid->GetGridVertex(topRightIndex).Pos;
		bottomRight = grid->GetGridVertex(bottomRightIndex).Pos;

		planePos = glm::vec2(xCoord, zCoord);
		height = barycentricInterpolation(glm::vec3(0,bottomLeft.y,0), glm::vec3(1,topRight.y,1), glm::vec3(1,bottomRight.y,0), planePos);
		
	}

	return height;

}

float Airplane::barycentricInterpolation(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos) {

	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.y + l2 * p2.y + l3 * p3.y;

}


float Airplane::fModulo(float x, float z)
{
	if (z > x) return x;

	while (x > z)
	{
		x -= z;
	}
	return x;
}

Transform Airplane::GetCollider(int index)
{
	if (index == 1)
	{
		return collider1;
	}
	if (index == 2)
	{
		return collider2;
	}
	else
		return Transform(glm::vec3(0),glm::vec3(0),0,0,0);
}
void Airplane::onCollision(BoxCollider3D& c)
{

	// REVERSE LAST MOVEMENT
	processMovement(M_BACKWARD, deltaTime);

	// PUSHING A BIRD
	glm::vec3 newBirdPos = glm::lerp(c.transform->Position, c.transform->Position + transform.Front, deltaTime);
	c.transform->SetPosition(newBirdPos);


}