#include "GameObject.h"
#ifndef GAMEOBJECT_VECTOR
#define GAMEOBJECT_VECTOR
std::vector<GameObject*> GameObject::gameObjects;
#endif

GameObject::GameObject(std::string path, glm::vec3 pos)
{
	gameObjects.push_back(this);

	transform.Position = pos;
	model = new Model(path);
}
GameObject::~GameObject()
{
	// find and erase this object from gameObjects vector

	std::vector<GameObject*>::iterator it = std::find(gameObjects.begin(), gameObjects.end(), this);

	if (it != gameObjects.cend()) // make sure element is in vector
	{
		//remove this element
		gameObjects.erase(it);
	}

	delete model;
}

void GameObject::UpdateGameObjects(float deltaTime)
{

	if (gameObjects.empty()) return;

	std::vector<GameObject*>::iterator i = gameObjects.begin();

	for (i; i < gameObjects.end(); i++)
	{
		(*i)->update(deltaTime);
	}
}

