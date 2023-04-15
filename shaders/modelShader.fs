#version 330 core
#define NR_POINT_LIGHTS 10  

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

in vec3 Normal; 
in vec3 FragPos;
in vec2 TexCoords;


//textures
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

//lights
uniform DirLight dirLight;
uniform int numOfPointLights;
uniform PointLight pointLights[NR_POINT_LIGHTS];

//materials
uniform Material material;

//function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, sampler2D texDiff, sampler2D texSpec); 
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, sampler2D texDiff, sampler2D texSpec);

//front vector of camera
uniform vec3 viewPos;


void main()
{    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);


    //TEXTURE SET 1:
        // light calcutactions:
        // phase 1: Directional lighting
        vec3 result = CalcDirLight(dirLight, norm, viewDir, texture_diffuse1, texture_specular1);

        // phase 2: Point lights
        if(numOfPointLights > 0)
        {
            for(int i = 0; i < numOfPointLights; i++)
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, texture_diffuse1, texture_specular1);    
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

    // not including material info - loaded models have very low values there idk why
    //ambient*=material.ambient;
    //diffuse*=material.diffuse;
    //specular*=material.specular;

    return (ambient + diffuse + specular);
}


// Point Light
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, sampler2D texDiff, sampler2D texSpec)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.color * light.ambient  * vec3(texture(texDiff, TexCoords));
    vec3 diffuse  = light.color * light.diffuse  * diff * vec3(texture(texDiff, TexCoords));
    vec3 specular = light.color * light.specular * spec * vec3(texture(texSpec, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    // not including material info - loaded models have very low values there idk why
    //ambient*=material.ambient;
    //diffuse*=material.diffuse;
    //specular*=material.specular;

    return (ambient + diffuse + specular);
} 
