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
public:
    // shader
    Shader *shader;
    Shader *depthShader;

    // constructors / destructors
    Object(std::string modelPath, std::string vertexPath, std::string fragmentPath, Shader *new_depthShader);
    Object(unsigned int const& new_VAO, unsigned int const& numVertices, std::string vertexPath, std::string fragmentPath);
    Object(unsigned int const& new_VAO, unsigned int const& numVertices, std::string vertexPath, std::string fragmentPath, GLuint const& new_text);

    // shader's world properties func
    void setWorld(glm::mat4 projection, glm::mat4 view);

    // text func
    GLuint getText();

    // draw funcs
    void draw();
    void drawWithoutTexture();
};

#endif