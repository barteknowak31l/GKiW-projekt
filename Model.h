#ifndef MODEL_H
#define MODEL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "Mesh.h"
#include "Shader.h"


using namespace std;

// utility function to read texture from file using stb_image library
unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);


class Model
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;             

    DirectionalLight directionalLight;  // stores directionalLight 
    vector<PointLight> pointLights;     // stores pointLights that are affecting this model

    string directory;                   // path to model data folder


    Model(string const& path);

    // draws all this model's meshes
    void Draw(Shader& shader);


    // light data is stored in Model object instead of being stored in every mesh object
    // set directional light data
    void setLightData(Light* light);
    
    // set point light data 
    void setLightData(static vector<PointerLight> lights, Shader& shader);


private:

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path);

    // intialize light data
    void setupLights();

    // processes a node recursively
    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);

};
#endif
