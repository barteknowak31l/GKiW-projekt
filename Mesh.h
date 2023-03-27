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


#define MAX_NUM_LIGHTS 4


struct Material {
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    glm::vec3 Ambient;
    float Shininess;
};

using namespace std;
#define MAX_BONE_INFLUENCE 4
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
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

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

    DirLight directionalLight;
    vector<PointLight> pointLights;
    SpotLight spotLight;

    unsigned int VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, Material material, Model* model);
    void Draw(Shader& shader);
    void SetLightData(Light_Types type, DirLight light);
    void SetLightData(Light_Types type, SpotLight light);
    void SetLightData(Light_Types type, static vector<PointerLight> lights);

private:
    //  render data
    unsigned int VBO, EBO;

    void setupMesh();
    void setupLights();

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


};
#endif
