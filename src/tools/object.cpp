#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <tools/object.hpp>

Object::Object(GltfModel &new_model, Shader new_shader)
{
    model = new_model;
    shader = new_shader;
    VAO = NULL;
    numVertices = NULL;
}

Object::Object(unsigned int new_VAO, unsigned int numVertices, Shader new_shader)
{
    VAO = new_VAO;
    shader = new_shader;
    model = GltfModel();
}

void Object::draw()
{
    if (VAO == NULL && numVertices == NULL)
    {
        model.draw();
        return;
    }
    else
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
    }

    // unbinde shader & VAO
    glBindVertexArray(0);
    shader.unuse();
}