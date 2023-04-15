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
}

void Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VB);
    glGenBuffers(1, &IB);

    glBindVertexArray(VAO);

    // load data into vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // load data into index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));



    glBindVertexArray(0);
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
    shader.setVec3("dirLight.direction", model->directionalLight.light.direction);
    shader.setVec3("dirLight.color", model->directionalLight.light.color);
    shader.setVec3("dirLight.ambient", model->directionalLight.light.ambient);
    shader.setVec3("dirLight.diffuse", model->directionalLight.light.diffuse);
    shader.setVec3("dirLight.specular", model->directionalLight.light.specular);

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


void Mesh::Draw(Shader& shader)
{
    shader.use();
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    // send textures to shader
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