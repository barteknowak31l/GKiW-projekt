#pragma once
#include "GameObject.h"

#include "PointerLight.h"


class Skull :
    public GameObject
{

public:


    PointerLight light;
    Shader* lightShader;

    Skull(const char* path, glm::vec3 pos, Shader* ls, glm::vec3 colSize) : GameObject(path, pos,colSize) { lightShader = ls; };
    ~Skull() { ; };

    void update(float deltaTime);
    void reset() { GameObject::defaultReset(); }
    void onCollision(BoxCollider3D& c) { GameObject::defaultOnCollision(c); }


    void drawLight(glm::mat4 p, glm::mat4 v);

    void setLightData(Shader *shader);

};

