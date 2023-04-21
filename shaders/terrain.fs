#version 330
layout(location = 0) out vec4 FragColor;

in vec4 Color;
in vec2 Tex;
in vec3 Normal;
in vec3 LocalPos;
in vec3 FragPos;

uniform sampler2D terrainTexture;


//height level textures
uniform sampler2D terrainTextureLv0;
uniform sampler2D terrainTextureLv1;
uniform sampler2D terrainTextureLv2;
uniform sampler2D terrainTextureLv3;
//height levels
uniform float lv0;
uniform float lv1;
uniform float lv2;
uniform float lv3;
uniform float lv4;
uniform float lv5;



uniform vec3 lightDir;
uniform vec3 lightColor;


// spotlight
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
uniform Spotlight light1;
uniform Spotlight light2;
uniform bool enableAirplaneFlashLight;

// prototypes
vec4 calcTexture();
vec3 calcSpotLight(Spotlight l);

void main()
{

    // diffuse 
    vec3 Normal_ = normalize(Normal);

    vec3 gReversedLightDir = vec3(lightDir.x, -lightDir.y, lightDir.z);

    float Diffuse = max(dot(Normal_, gReversedLightDir),0);

    Diffuse = max(0.3f, Diffuse);

    vec4 texture = calcTexture();

    if(enableAirplaneFlashLight)
    {
    	FragColor =  Diffuse * vec4(lightColor,1.0) * texture + texture * vec4(calcSpotLight(light1),1.0) + texture *vec4(calcSpotLight(light2),1.0);
    }
    else{
    	FragColor =  Diffuse * vec4(lightColor,1.0) * texture;
    }

}

vec4 calcTexture()
{
    vec4 tex;

    float Height = LocalPos.y;

 if(Height < lv0)
    {
        tex = texture(terrainTextureLv0,Tex);
    }
    else if(Height >= lv0 && Height < lv1)
    {
        vec4 t0 = texture(terrainTextureLv0,Tex);
        vec4 t1 = texture(terrainTextureLv1,Tex);
        
        float length = lv1 - lv0;
        float delta = Height - lv0;
        float fraction = delta/length;


        tex = mix(t0,t1,fraction);
    }
    else if(Height >= lv1 && Height < lv2)
    {
        vec4 t0 = texture(terrainTextureLv1,Tex);
        vec4 t1 = texture(terrainTextureLv2,Tex);
        
        float length = lv2 - lv1;
        float delta = Height - lv1;
        float fraction = delta/length;

        tex = mix(t0,t1,fraction);
    }
    else if(Height >= lv2 && Height < lv3)
    {
        vec4 t0 = texture(terrainTextureLv2,Tex);
        vec4 t1 = texture(terrainTextureLv3,Tex);
        
        float length = lv3 - lv2;
        float delta = Height - lv2;
        float fraction = delta/length;

        tex = mix(t0,t1,fraction);
    }
    else
    {
        tex = texture(terrainTextureLv3,Tex);
    }

    return tex;
}

vec3 calcSpotLight(Spotlight l)
{
    vec3 lightDirection = normalize(l.position - FragPos);
    
    float theta = dot(lightDirection, normalize(-l.direction));
    float epsilon   = l.cutOff - l.outerCutOff;
    float intensity = clamp((theta - l.outerCutOff) / epsilon, 0.0, 1.0); 
    
    float distance    = length(l.position - FragPos);
    float attenuation = 1.0 / (l.constant + l.linear * distance + 
    		    l.quadratic * (distance * distance)); 


        return l.color * intensity * attenuation;

}