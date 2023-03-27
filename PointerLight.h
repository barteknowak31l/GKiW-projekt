#ifndef POINTER_LIGHT
#define POINTER_LIGHT

#include "Light.h"
class PointerLight : public Light
{
    public :
        unsigned int VAO, VBO;
        PointerLight();
        PointerLight(glm::vec3 position, glm::vec3 color, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic);
        PointLight light;
        void draw(Shader& shader);

private:
    void setup();
    

};
#endif