#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Transform.h"
#include "Model.h"
#include "BoxCollider3D.h"

#include <vector>
#include <algorithm>

// Interface for representing 3d objects
// Handles local transform and model data separately
// 


class GameObject
{

public:
	Transform transform;
	Model* model;
	BoxCollider3D boxCollider3D;
	
	GameObject() { ; };
	GameObject(std::string path, glm::vec3 pos, glm::vec3 colliderSize);
	~GameObject();

	virtual void update(float deltaTime) = 0;
	virtual void reset() = 0;
	virtual void onCollision(BoxCollider3D&) = 0;


	static void UpdateGameObjects(float deltaTime);



protected:

	// statyczny wektor wszystkich obiektow typu gameObject
	static std::vector<GameObject*> gameObjects;

	// default virtuals
	void defaultUpdate(float deltaTime)
	{
		return;
	}

	void defaultReset()
	{
		return;
	}

	void defaultOnCollision(BoxCollider3D& c)
	{
		return;
	}

private:
	static void checkCollisions(GameObject*);

};


#endif
