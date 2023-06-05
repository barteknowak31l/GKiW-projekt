#include "Bird.h"
float birdSpeed = 5.0f;

void Bird::drawLight(glm::mat4 p, glm::mat4 v)
{
    lightShader->use();
    lightShader->setMat4("projection", p);
    lightShader->setMat4("view", v);
    light.light.position = transform.Position - 5.0f*transform.Up;
    light.draw(*lightShader);
}




void Bird::update(float deltaTime)
{

    model->pointLights[0].position = transform.Position + transform.Up * -1.0f + transform.Front;
    float rotFactor = 3.14f;
    transform.Move(transform.Front * deltaTime * birdSpeed);
    transform.Rotate(glm::vec3(0.0, std::cos(deltaTime),std::cos(deltaTime)));
    //transform.Rotate(glm::vec3(0.0, std::cos(deltaTime),0.0));

}