#ifndef MESH_H
#define MESH_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>



#include <iostream>
#include <vector>
#include <assimp/include/assimp/Importer.hpp>
#include <assimp/include/assimp/scene.h>
#include <assimp/include/assimp/postprocess.h>
#include "Shader.h"

#include "DirectionalLight.h"
#include "PointerLight.h"

class Model;

// NUMBER OF POINT LIGHTS SEMD TO SHADER
#define MAX_NUM_LIGHTS 4

// Stores values for light calculations - read from model file
struct Material {
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    glm::vec3 Ambient;
    float Shininess;
};

using namespace std;

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
    // mesh data
    Model* model;
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    Material material;

    unsigned int VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, Material material, Model* model);
    void Draw(Shader& shader);


private:
    //  render data
    unsigned int VBO, EBO;

    void setupMesh();

    // sends uniform data from Material struct to shader program
    // material data convention (in shader)
    // struct material{
    // vec3 ambient
    // vec3 diffuse
    // vec3 specular
    // float shininess
    // }
    void sendMaterialToShader(Shader& shader);

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
