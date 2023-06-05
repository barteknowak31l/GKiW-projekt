#version 330 core
#define NR_POINT_LIGHTS 12  


in vec3 Normal; 
in vec3 worldFragPos;
in vec2 TexCoords;

out vec4 FragColor;


struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

 struct DirLight {
    vec3 direction;
  
    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 


// airplane spotLights
struct Spotlight{
    
    vec3 position;
    vec3 direction;
    
    vec3 color;
    float cutOff;
    float outerCutOff;

    //attenaution
    float constant;
    float linear;
    float quadratic;

};



// airplane lights
uniform Spotlight spotLight1;
uniform Spotlight spotLight2;
uniform bool enableAirplaneFlashLight;

// textures
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

// lights
uniform DirLight dirLight;
uniform int numOfPointLights;
uniform PointLight pointLights[NR_POINT_LIGHTS];

// materials
uniform Material material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, sampler2D texDiff, sampler2D texSpec); 
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 wFragPos, vec3 viewDir, sampler2D texDiff, sampler2D texSpec);
vec3 calcSpotLight(Spotlight l);

// positionr of camera
uniform vec3 viewPos;


void main()
{    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - worldFragPos);


    //TEXTURE SET 1:
        // light calcutactions:
        // phase 1: Directional lighting
        vec3 dLight = CalcDirLight(dirLight, norm, viewDir, texture_diffuse1, texture_specular1);
        vec3 pLight = vec3(0);
        vec3 aLight = vec3(0);

        // phase 2: Point lights
        if(numOfPointLights > 0)
        {
            for(int i = 0; i < numOfPointLights; i++)
            pLight += CalcPointLight(pointLights[i], norm, worldFragPos, viewDir, texture_diffuse1, texture_specular1);    
        }

        if(enableAirplaneFlashLight)
        {
            aLight +=calcSpotLight(spotLight1);
            aLight +=calcSpotLight(spotLight2);
        }

    
        vec3 result = dLight + pLight + 0.2 * aLight;


        if(numOfPointLights == 1) // only for a bird
        {
            result = dLight +  2.0 * pLight + 0.2 * aLight;
        }

    //FragColor = texture(texture_diffuse1, TexCoords);
    FragColor = vec4(result,1.0);
}


// lights calculations:

// Directional Light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, sampler2D texDiff, sampler2D texSpec)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.color * light.ambient  * vec3(texture(texDiff,TexCoords));
    vec3 diffuse  = light.color * light.diffuse  * diff * vec3(texture(texDiff,TexCoords));
    vec3 specular = light.color * light.specular * spec * vec3(texture(texSpec,TexCoords));


    return (ambient + diffuse + specular);
}


// Point Light
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 wFragPos, vec3 viewDir, sampler2D texDiff, sampler2D texSpec)
{
    vec3 lightDir = normalize(light.position - wFragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance    = length(light.position - wFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    if(numOfPointLights != 1) // only for a bird
    {
         attenuation = 1.0 / (light.constant + 0.014 * distance + 
  			     0.0007 * (distance * distance)); 
    }

    // combine results
    vec3 ambient  = light.color * light.ambient  * vec3(texture(texDiff, TexCoords));
    vec3 diffuse  = light.color * light.diffuse  * diff * vec3(texture(texDiff, TexCoords));
    vec3 specular = light.color * light.specular * spec * vec3(texture(texSpec, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;


    return (ambient + diffuse + specular) ;

} 


vec3 calcSpotLight(Spotlight l)
{
    vec3 lightDirection = normalize(l.position - worldFragPos);
    
    float theta = dot(lightDirection, normalize(-l.direction));
    float epsilon   = l.cutOff - l.outerCutOff;
    float intensity = clamp((theta - l.outerCutOff) / epsilon, 0.0, 1.0); 
    
    float distance    = length(l.position - worldFragPos);
    float attenuation = 1.0 / (l.constant + l.linear * distance + 
    		    l.quadratic * (distance * distance)); 


        return l.color * intensity * attenuation;

}