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
    GltfModel *model;
    
    unsigned int numVertices;
    // mode
    bool gltf;
public:
    // shader
    Shader *shader;
    unsigned int VAO;

    // constructors / destructors
    Object(GltfModel *new_model, Shader *new_shader);
    Object(unsigned int new_VAO, unsigned int numVertices, Shader *new_shader);

    // draw func
    void draw();
};

#endif