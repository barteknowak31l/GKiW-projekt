#pragma once
#include "GameObject.h"

#include "PointerLight.h"


class Bird :
    public GameObject
{

public:


    PointerLight light;
    Shader* lightShader;

    Bird(const char* path, glm::vec3 pos, Shader* ls) : GameObject(path, pos) { lightShader = ls; };
    ~Bird() { ; };

    void update(float deltaTime);
    void reset() { GameObject::defaultReset(); }
    void drawLight(glm::mat4 p, glm::mat4 v);

    void setLightData(Shader* shader);

};

