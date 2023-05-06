#include "Skull.h"

void Skull::drawLight(glm::mat4 p , glm::mat4 v)
{
    lightShader->use();
    lightShader->setMat4("projection", p);
    lightShader->setMat4("view", v);
	light.draw(*lightShader);
}

void Skull::setLightData(Shader* shader)
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


void Skull::update(float deltaTime)
{
    float rotFactor = 3.14f;
    glm::vec3 rotation = glm::vec3(deltaTime ,0.0f , 0.0f);
    transform.Rotate(rotation);

}