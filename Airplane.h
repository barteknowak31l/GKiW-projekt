#ifndef AIRPLANE_H
#define AIRPLANE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "GameObject.h"
#include "Camera.h"

class Grid;

// attached camera positions
const glm::vec3 thirdPersonCamOffset = glm::vec3(0.0f, 0.0f, -4.0f);
const glm::vec3 firstPersonCamOffset = glm::vec3(0.0f, -.2f, 0.5f);

struct flashLight {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 color;
	float cutOff;
	float outerCutOff;

	//attenaution
	float constant;
	float linear;
	float quadratic;

};

class Airplane : public GameObject
{
public:

	Camera* camera;

	// flashlights
	flashLight leftFlashlight;
	flashLight rightFlashlight;
	float flashlightOffset;

	// used for animating rotation on Z axis during turn
	float yawAnimation = 0.0f;
	
	glm::vec3 resetPosition;

	// constructor of airplane without camera
	Airplane(std::string path,glm::vec3 pos, Grid* grid, float spd, glm::vec3 scale, glm::vec3 flashlightColor, float flashlightOffset, glm::vec3 colSize);

	// constructor with camera - this one is controlled by player
	Airplane(std::string path, Camera* _camera, glm::vec3 pos, Grid* grid, float spd, bool _flipPitch, glm::vec3 scale, glm::vec3 flashlightColor, float flashlightOffset, glm::vec3 colSize);

	~Airplane() { ; };

	// movemment handlers
	void processMovement(Move_direction direction, float deltaTime);
	void onMovementRelease(Move_direction dir);

	// called each frame
	void update(float deltaTime);
	
	// sets default settings
	void reset();

	glm::mat4 calcModelMatrix(glm::mat4 matrix);


	float checkTerrainCollision(Grid* grid, Transform transform);

	Transform GetCollider(int index);

	void onCollision(BoxCollider3D& c);


private:


	// true if this object is controlled by playert
	bool isPlayer;


	// flying speed
	float speed;

	// used for rotating transform when turning around
	//
	float yaw = 0.0f;
	float yawDelta = 60.0f;
	float yawMax = 360.0f;


	// used for rotating when rising and diving
	float pitch = 0.0f;
	bool flipPitch;

	// used to animate rising and diving - used in model matrix rotation (currently in main() function) - todo - function to calculate Model matrix based on pitch/yaw/roll
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

	// collision checker
	bool collision = true;

	// colliders
	Grid* grid;
	Transform collider1;
	float collider1Offset = .7f;
	Transform collider2;
	float collider2Offset = 1.5f;


	// sets variables associated with turning animation
	void handleTurnAnimation(float deltaTime);
	
	// sets rotation & position of the camera while moving
	void handleCamera();

	// updates flashlight position/direction
	void handleFlashlight();
	void setupFlashlights(glm::vec3 color);


	// barycentric interpolation  for collision detection
	float barycentricInterpolation(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos);


	// calculate modulo for float
	float fModulo(float x, float y);

};
#endif


