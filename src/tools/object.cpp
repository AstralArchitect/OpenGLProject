#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <tools/object.hpp>

#include <iostream>

Object::Object(GltfModel new_model, Shader new_shader)
{ 
    model = &new_model;
    shader = &new_shader;
    gltf = true;
}

Object::Object(unsigned int new_VAO, unsigned int numVertices, Shader new_shader)
{
    VAO = new_VAO;
    shader = &new_shader;
    gltf = false;
}

Object::~Object()
{
    if (gltf)
    {
        delete model;
    }
}

void Object::draw()
{
    if (gltf)
    {
        model->draw();
        return;
    }
    else
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
    }

    // unbinde shader & VAO
    glBindVertexArray(0);
    shader->unuse();
}