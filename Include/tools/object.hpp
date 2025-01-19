#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <tools/shader.hpp>
#include <tools/gltfloader.hpp>

class Object
{
private:
    // VAO / model
    unsigned int VAO;
    GltfModel *model;
    
    unsigned int numVertices;
    GLuint texture;
    // mode
    bool gltf;

    // create VAO from vertices, verticesSize must be the size in byte
    void createVAO(float *vertices, unsigned long verticesSize, bool UVs, bool normals, bool texCoords);
public:
    // shader
    Shader *shader;
    Shader *depthShader;

    // constructors
    Object(std::string modelPath, std::string vertexPath, std::string fragmentPath, Shader *new_depthShader);
    Object(float *vertices, unsigned long verticesSizeInByte, bool UVs, bool normals, bool texCoords, std::string vertexPath, std::string fragmentPath);
    Object(float *vertices, unsigned long verticesSizeInByte, bool UVs, bool normals, bool texCoords, std::string vertexPath, std::string fragmentPath, GLuint const& new_text);
    // destructor
    ~Object();

    // shader's world properties func
    void setWorld(glm::mat4 projection, glm::mat4 view);

    // text func
    GLuint getText();

    // draw funcs
    void draw();
    void drawWithoutTexture();
};

#endif