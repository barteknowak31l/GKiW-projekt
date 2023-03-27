#include "Mesh.h"
#include "Model.h"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, Material material, Model* model)
{
    this->model = model;
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->material = material;

    setupMesh();
    setupLights();
}
void Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);
}


void Mesh::setupLights()
{
    directionalLight.direction = glm::vec3(1.2f, 1.0f, -2.0f);
    directionalLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    directionalLight.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    directionalLight.diffuse = glm::vec3(0.1f, 0.1f, 0.1f);
    directionalLight.specular = glm::vec3(0.1f, 0.1f, 0.1f);

}

void Mesh::sendMaterialToShader(Shader& shader)
{
    shader.setVec3("material.ambient", material.Ambient);
    shader.setVec3("material.diffuse", material.Diffuse);
    shader.setVec3("material.specular", material.Specular);
    shader.setFloat("material.shininess", material.Shininess);
}

void Mesh::sendLightsToShader(Shader& shader)
{
    // directional light
    shader.setVec3("dirLight.direction", directionalLight.direction);
    shader.setVec3("dirLight.color", directionalLight.color);
    shader.setVec3("dirLight.ambient", directionalLight.ambient);
    shader.setVec3("dirLight.diffuse", directionalLight.diffuse);
    shader.setVec3("dirLight.specular", directionalLight.specular);

    // point lights
    shader.setInt("numOfPointLights", model->pointLights.size());
    if (model->pointLights.size() > 0)
    {
        for (int i = 0; i < std::min(MAX_NUM_LIGHTS,(int)model->pointLights.size()); i++)
        {
            string prefix = "pointLights[" + to_string(i) + "].";
            shader.setVec3(prefix + "position", model->pointLights[i].position);
            shader.setVec3(prefix + "color", model->pointLights[i].color);
            shader.setVec3(prefix + "ambient", model->pointLights[i].ambient);
            shader.setVec3(prefix + "diffuse", model->pointLights[i].diffuse);
            shader.setVec3(prefix + "specular", model->pointLights[i].specular);
            shader.setFloat(prefix + "constant", model->pointLights[i].constant);
            shader.setFloat(prefix + "linear", model->pointLights[i].linear);
            shader.setFloat(prefix + "quadratic", model->pointLights[i].quadratic);
        }

    }



}


void Mesh::SetLightData(Light_Types type, DirLight light)
{
    if (type != DIRECTIONAL)
    {
        std::cout << "Failed to set DIRECTIONAL LIGHT data\n";
    }

    directionalLight = light;
}

void Mesh::SetLightData(Light_Types type, static vector<PointerLight> lights)
{
    if (type != POINT)
    {
        std::cout << "Failed to set POINT LIGHT data\n";
    }

    vector<PointLight> _lights;
    for (int i = 0; i < lights.size(); i++)
    {

        _lights.push_back(lights[i].light);
        std::cout<<_lights[i].position.x << " dziala jeszcze\n";
    }

    pointLights = _lights;
    std::cout << "dziala jeszcze\n";
}



void Mesh::SetLightData(Light_Types type, SpotLight light)
{
    if (type != SPOT)
    {
        std::cout << "Failed to set SPOT LIGHT data\n";
    }

    spotLight = light;
}


void Mesh::Draw(Shader& shader)
{
    shader.use();
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if (name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to string
        else if (name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to string

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(VAO);
    sendMaterialToShader(shader);
    sendLightsToShader(shader);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}