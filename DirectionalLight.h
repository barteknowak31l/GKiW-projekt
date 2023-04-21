#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
#include "Light.h"
#include <cmath>

class DirectionalLight : public Light
{
public:

    // struct (defined in light.h) for storing light data
    DirLight light;

    // just for compilator to not throw billion errors
    DirectionalLight() {
        type = DIRECTIONAL;

        light.direction = glm::vec3(0);
        light.color = glm::vec3(0);
        light.ambient = glm::vec3(0);
        light.diffuse = glm::vec3(0);
        light.specular = glm::vec3(0);
    }

    // actual constructor
    DirectionalLight(glm::vec3 direction, glm::vec3 color, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
    {
        type = DIRECTIONAL;

        //light.direction = direction;
        //light.color = color
        light.ambient = ambient;
        light.diffuse = diffuse;
        light.specular = specular;

        light.direction = up;
        light.color = fullSun;
    }

    // dir light cant be represented as a point in world space so dont draw it
    void draw(Shader& shader)
    {
        ;
    }

    // simulate day night cycle - interpolate between color and position states
    void dayNight(float deltaTime)
    {

        glm::vec3 newColor;
        glm::vec3 newDirection;

        progressCounter += deltaTime * sunSpeed;


        if (progressCounter > 1.0f)
        {
            progressCounter = 1.0f;
        }


        switch (state)
        {
        case 0: // fullsun -> fullsun
        {
            light.color = fullSun;
            newDirection.x = std::lerp(up.x, mid.x, progressCounter);
            newDirection.y = std::lerp(up.y, mid.y, progressCounter);
            newDirection.z = std::lerp(up.z, mid.z, progressCounter);
            light.direction = newDirection;
            break;
        }
        case 1: // fullsun -> dawn
        {
            newColor.x = std::lerp(fullSun.x, dawn.x, progressCounter);
            newColor.y = std::lerp(fullSun.y, dawn.y, progressCounter);
            newColor.z = std::lerp(fullSun.z, dawn.z, progressCounter);
            light.color = newColor;

            newDirection.x = std::lerp(mid.x, horizon.x, progressCounter);
            newDirection.y = std::lerp(mid.y, horizon.y, progressCounter);
            newDirection.z = std::lerp(mid.z, horizon.z, progressCounter);
            light.direction = newDirection;

            break;
        }
        case 2: //  dawn -> darkness
        {
            newColor.x = std::lerp(dawn.x, darkness.x, progressCounter);
            newColor.y = std::lerp(dawn.y, darkness.y, progressCounter);
            newColor.z = std::lerp(dawn.z, darkness.z, progressCounter);
            light.color = newColor;

            light.direction = horizon;

            break;
        }
        case 3: //  darkness -> moon
        {
            newColor.x = std::lerp(darkness.x, moon.x, progressCounter);
            newColor.y = std::lerp(darkness.y, moon.y, progressCounter);
            newColor.z = std::lerp(darkness.z, moon.z, progressCounter);
            light.color = newColor;

            newDirection.x = std::lerp(horizon.x, mid.x, progressCounter);
            newDirection.y = std::lerp(horizon.y, mid.y, progressCounter);
            newDirection.z = std::lerp(horizon.z, mid.z, progressCounter);
            light.direction = newDirection;

            break;
        }
        case 4: //  moon -> moon
        {
            light.color = moon;
            newDirection.x = std::lerp(mid.x, up.x, progressCounter);
            newDirection.y = std::lerp(mid.y, up.y, progressCounter);
            newDirection.z = std::lerp(mid.z, up.z, progressCounter);
            light.direction = newDirection;
            break;
        }
        case 5: // moon -> darkness
        {
            newColor.x = std::lerp(moon.x, darkness.x, progressCounter);
            newColor.y = std::lerp(moon.y, darkness.y, progressCounter);
            newColor.z = std::lerp(moon.z, darkness.z, progressCounter);
            light.color = newColor;

            newDirection.x = std::lerp(up.x, horizon.x, progressCounter);
            newDirection.y = std::lerp(up.y, horizon.y, progressCounter);
            newDirection.z = std::lerp(up.z, horizon.z, progressCounter);
            light.direction = newDirection;
            break;
        }

        case 6: //  darkness -> dawn
        {
            newColor.x = std::lerp(darkness.x, dawn.x, progressCounter);
            newColor.y = std::lerp(darkness.y, dawn.y, progressCounter);
            newColor.z = std::lerp(darkness.z, dawn.z, progressCounter);
            light.color = newColor;

            newDirection.x = std::lerp(horizon.x, mid.x, progressCounter);
            newDirection.y = std::lerp(horizon.y, mid.y, progressCounter);
            newDirection.z = std::lerp(horizon.z, mid.z, progressCounter);
            light.direction = newDirection;

            break;
        }
        case 7: //  dawn -> fullSun
        {
            newColor.x = std::lerp(dawn.x, fullSun.x, progressCounter);
            newColor.y = std::lerp(dawn.y, fullSun.y, progressCounter);
            newColor.z = std::lerp(dawn.z, fullSun.z, progressCounter);
            light.color = newColor;

            newDirection.x = std::lerp(mid.x, up.x, progressCounter);
            newDirection.y = std::lerp(mid.y, up.y, progressCounter);
            newDirection.z = std::lerp(mid.z, up.z, progressCounter);
            light.direction = newDirection;

            break;
        }
        default:
        {
            break;
        }
        }

        if (progressCounter >= 1.0f)
        {
            progressCounter = 0.0f;
            state++;
        }

        if (state > 7)
        {
            state = 0;
        }

    }

    void setSunSpeed(float speed)
    {
        sunSpeed = speed;
    }

private:
    // day night settings
    // states order: 1. fullSun, 2. dawm, 3. darkness, 4. moon, 5. darkness, 6. dawn 7. repeat
    int state = 0;
    float progressCounter = 0;
    float sunSpeed = 0.3f;
    
    // colors
    glm::vec3 fullSun  = glm::vec3(255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f);
    glm::vec3 dawn = glm::vec3(255.0f / 255.0f, 209.0f / 255.0f, 180.0f / 255.0f);
    glm::vec3 darkness = glm::vec3(30.0f/255.0f, 30.0f / 255.0f, 30.0f / 255.0f);
    glm::vec3 moon = glm::vec3(27.0f / 255.0f, 79.0f / 255.0f, 150.0f / 255.0f);

    // ray directions
    glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 mid = glm::vec3(-0.5f, -0.5f, 0.0f);
    glm::vec3 horizon = glm::vec3(-1.0f, -.5f, 1.0f);



};
#endif
