#include "PointerLight.h"

PointerLight::PointerLight()
{
    setup();
}
PointerLight::PointerLight(glm::vec3 position, glm::vec3 color, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic) {

    type = POINT;

    light.position = position;
    light.color = color;
    light.ambient = ambient;
    light.diffuse = diffuse;
    light.specular = specular;
    light.constant = constant;
    light.linear = linear;
    light.quadratic = quadratic;

    setup();


}

void PointerLight::setup()
{

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


}

void PointerLight::PointerLight::draw(Shader& shader)
{
    shader.use();
    glBindVertexArray(VAO);
    glm::mat4 model;
    model = glm::mat4(1.0f);
    model = glm::translate(model, light.position);
    model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
    shader.setMat4("model", model);
    shader.setVec3("color", light.color);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
