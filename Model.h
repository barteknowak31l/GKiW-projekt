#ifndef MODEL_H
#define MODEL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
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

    DirectionalLight directionalLight;  // stores directionalLight affecting this model
    SpotLight spotLight;                // stores spotLight affecting this model
    vector<PointLight> pointLights;     // stores pointLights that are affecting this model

    string directory;                   // path to model data folder
    bool gammaCorrection;               // idk yet

    //shouldnt be used / use Model(string const& path, bool gamma = false) instead
    Model()
    {
        ;
    }
    // constructor, expects a filepath to a 3D model.
    Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
        setupLights();
    }

    // draws the model, and thus all its meshes
    void Draw(Shader& shader);

    // set directional light data
    void setLightData(Light* light);
    
    // set point light data
    void setLightData(static vector<PointerLight> lights, Shader& shader, bool draw);


private:

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path);

    // intialize light data
    void setupLights();

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);

};
#endif
