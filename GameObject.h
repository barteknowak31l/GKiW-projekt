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


// Interface for representing 3d objects
// Handles local transform and model data separately
// 

class GameObject
{

public:
	Transform transform;
	Model* model;

	GameObject(std::string path, glm::vec3 pos)
	{
		transform.Position = pos;
		model = new Model(path);
	}

	virtual void update(float deltaTime) = 0;
	virtual void reset() = 0;

protected:
	// default virtuals
	void defaultUpdate(float deltaTime)
	{
		return;
	}

	void defaultReset()
	{
		return;
	}

};

#endif
