#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <tools/object.hpp>

#include <iostream>

Object::Object(GltfModel *new_model, Shader *new_shader, Shader *new_depthShader)
{ 
    model = new_model;
    shader = new_shader;
    depthShader = new_depthShader;
    gltf = true;

    VAO = 0;
    numVertices = 0;
}

Object::Object(unsigned int const& new_VAO, unsigned int const& new_numVertices, Shader *new_shader)
{
    VAO = new_VAO;
    numVertices = new_numVertices;
    shader = new_shader;
    depthShader = nullptr;
    gltf = false;
}

Object::Object(unsigned int const& new_VAO, unsigned int const& new_numVertices, Shader *new_shader, GLuint const& new_text)
{
    VAO = new_VAO;
    numVertices = new_numVertices;
    shader = new_shader;
    depthShader = nullptr;
    texture = new_text;

    gltf = false;
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