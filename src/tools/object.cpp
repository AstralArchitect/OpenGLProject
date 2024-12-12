#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <tools/object.hpp>

#include <iostream>

Object::Object(GltfModel *new_model, Shader *new_shader)
{ 
    model = new_model;
    shader = new_shader;
    gltf = true;

    VAO = 0;
    numVertices = 0;
}

Object::Object(unsigned int new_VAO, unsigned int const& new_numVertices, Shader *new_shader)
{
    VAO = new_VAO;
    numVertices = new_numVertices;
    shader = new_shader;
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

void Object::setWorld(glm::mat4 projection, glm::mat4 view)
{
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
}