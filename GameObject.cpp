#include "GameObject.h"
#ifndef GAMEOBJECT_VECTOR
#define GAMEOBJECT_VECTOR
std::vector<GameObject*> GameObject::gameObjects;
#endif

GameObject::GameObject(std::string path, glm::vec3 pos, glm::vec3 colliderSize)
{
	gameObjects.push_back(this);

	transform.Position = pos;

	boxCollider3D.point = pos;
	boxCollider3D.size = colliderSize;
	boxCollider3D.transform = &transform;

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

		// handle collisions - move boxCollider and check if there's a collision
		(*i)->boxCollider3D.point = (*i)->transform.Position;
		checkCollisions(*i);

		// call update on this object
		(*i)->update(deltaTime);
	}
}

void GameObject::checkCollisions(GameObject* g)
{
	std::vector<GameObject*>::iterator i = gameObjects.begin();

	// traverse all gameobjects and check if their boxColliders are intersecting
	for (i; i < gameObjects.end(); i++)
	{
		// skip collision with itself
		if (*i == g) continue;

		if (g->boxCollider3D.collision((*i)->boxCollider3D))
		{
			g->onCollision((*i)->boxCollider3D);
		}


	}
}

