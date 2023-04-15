#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
#include "Light.h"

class DirectionalLight : public Light
{
public:

    // struct (defined in light.h) for storing light data
    DirLight light;

    // just for compilator to not throw billion errors
    DirectionalLight() {
        ;
    }

    // actual constructor
    DirectionalLight(glm::vec3 direction, glm::vec3 color, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
    {
        type = DIRECTIONAL;

        light.direction = direction;
        light.color = color;
        light.ambient = ambient;
        light.diffuse = diffuse;
        light.specular = specular;
    }

    // dir light cant be represented as a point in world space so dont draw it
    void draw(Shader& shader)
    {
        ;
    }


};
#endif
