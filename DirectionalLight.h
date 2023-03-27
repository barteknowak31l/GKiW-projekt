#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT


#include "Light.h"
class DirectionalLight : public Light
{
public:
    DirLight light;
    DirectionalLight() {
        ;
    }
    DirectionalLight(glm::vec3 direction, glm::vec3 color, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
    {
        type = DIRECTIONAL;

        light.direction = direction;
        light.color = color;
        light.ambient = ambient;
        light.diffuse = diffuse;
        light.specular = specular;
    }

    void draw(Shader& shader)
    {
        ;
    }


};
#endif
