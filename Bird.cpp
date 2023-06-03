#include "Bird.h"
float birdSpeed = 5.0f;

void Bird::drawLight(glm::mat4 p, glm::mat4 v)
{
    lightShader->use();
    lightShader->setMat4("projection", p);
    lightShader->setMat4("view", v);
    light.light.position = transform.Position + 3.0f*transform.Up;
    light.draw(*lightShader);
}

void Bird::setLightData(Shader* shader)
{
    shader->use();
    string prefix = "pointLights[" + to_string(0) + "].";
    shader->setVec3(prefix + "position", light.light.position);
    shader->setVec3(prefix + "color", light.light.color);
    shader->setVec3(prefix + "ambient", light.light.ambient);
    shader->setVec3(prefix + "diffuse", light.light.diffuse);
    shader->setVec3(prefix + "specular", light.light.specular);
    shader->setFloat(prefix + "constant", light.light.constant);
    shader->setFloat(prefix + "linear", light.light.linear);
    shader->setFloat(prefix + "quadratic", light.light.quadratic);
}


void Bird::update(float deltaTime)
{
    float rotFactor = 3.14f;
    //transform.Move(transform.Front * deltaTime * birdSpeed);
    //transform.Rotate(glm::vec3(0.0, std::cos(deltaTime),std::cos(deltaTime)));
    //transform.Rotate(glm::vec3(0.0, std::cos(deltaTime),0.0));

}