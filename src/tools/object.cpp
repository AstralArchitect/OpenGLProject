#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <tools/object.hpp>

#include <iostream>

Object::Object(unsigned int const& new_VAO, unsigned int const& new_numVertices, std::string vertexPath, std::string fragmentPath)
{
    VAO = new_VAO;
    numVertices = new_numVertices;
    shader = new Shader(vertexPath, fragmentPath);
    depthShader = nullptr;
    gltf = false;
}

Object::Object(unsigned int const& new_VAO, unsigned int const& new_numVertices, std::string vertexPath, std::string fragmentPath, GLuint const& new_text)
{
    VAO = new_VAO;
    numVertices = new_numVertices;
    shader = new Shader(vertexPath, fragmentPath);
    depthShader = nullptr;
    texture = new_text;

    gltf = false;
}

Object::Object(std::string modelPath, std::string vertexPath, std::string fragmentPath, Shader *new_depthShader)
{
    model = new GltfModel((const char *)modelPath.c_str());
    shader = new Shader(vertexPath, fragmentPath);

    depthShader = new_depthShader;
    gltf = true;
}

void Object::draw()
{
    if (gltf)
    {
        model->draw();
        shader->unuse();
        return;
    }
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    // unbinde shader & VAO
    glBindVertexArray(0);
    shader->unuse();
}

void Object::drawWithoutTexture()
{
    if (gltf)
    {
        model->drawWithoutTextures();
        depthShader->unuse();
        return;
    }
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    // unbinde shader & VAO
    glBindVertexArray(0);
    depthShader->unuse();
}

GLuint Object::getText()
{
    return texture;
}