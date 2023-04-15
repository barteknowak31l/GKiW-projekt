#ifndef MESH_H
#define MESH_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include <assimp/include/assimp/Importer.hpp>
#include <assimp/include/assimp/scene.h>
#include <assimp/include/assimp/postprocess.h>

#include "Shader.h"
#include "DirectionalLight.h"
#include "PointerLight.h"



using namespace std;

class Model;

// MAX NUMBER OF POINT LIGHTS SEND TO SHADER
#define MAX_NUM_LIGHTS 10

// Stores values for light calculations - read from model file
struct Material {
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    glm::vec3 Ambient;
    float Shininess;
};

// stores vertex data read from model file
struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

// stores textures data
struct Texture {
	unsigned int id;
	std::string type;
	string path;
};



class Mesh
{
public:

    // pointer to parent model
    Model* model;

    // mesh data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    Material material;


    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, Material material, Model* model);
    
    // draw a mesh
    void Draw(Shader& shader);


private:

    //  render buffers
    unsigned int VAO, VB, IB;

    void setupMesh();


    // shader: TEXTURE NAMING CONVENTION
    // i.e 
    // textureDiffuse1 - texture+Type+number( starting from 1 )
    // textureSpecular1
    // textureHeight2 etc.

    // sends uniform data from Material struct to shader program
    void sendMaterialToShader(Shader& shader);
    // material data convention (in shader)
    // struct material{
    // vec3 ambient
    // vec3 diffuse
    // vec3 specular
    // float shininess
    // }


    // sends light data stored in structs to shader
    void sendLightsToShader(Shader& shader);
    // structs in shader:
    //struct DirLight {
    //    vec3 direction;
    //    vec3 color;
    //    vec3 ambient;
    //    vec3 diffuse;
    //    vec3 specular;
    //};

    //struct PointLight {
    //    vec3 position;
    //    float constant;
    //    float linear;
    //    float quadratic;
    //    vec3 color;
    //    vec3 ambient;
    //    vec3 diffuse;
    //    vec3 specular;
    //};





};
#endif
