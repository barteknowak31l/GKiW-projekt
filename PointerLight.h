#ifndef POINTER_LIGHT
#define POINTER_LIGHT
#include "Light.h"

class PointerLight : public Light
{
    public :

        // struct storing light data (defined in light.h)
        PointLight light;

        // scale of cube for drawing purposes
        float cubeScale = 1.0f; 

        // just for compilator to not throw billion errors
        PointerLight();

        // actual constructor
        PointerLight(glm::vec3 position, glm::vec3 color, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic);
        
        // draw point light location as colored cube
        void draw(Shader& shader);

private:
    // used for drawing
    unsigned int VAO, VBO;

    //set VAO, VBO
    void setup();
    

};
#endif