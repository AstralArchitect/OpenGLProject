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
    unsigned int VAO;
    
    unsigned int numVertices;
    // mode
    bool gltf;
public:
    // shader
    Shader *shader;

    // constructors / destructors
    Object(GltfModel *new_model, Shader *new_shader);
    Object(unsigned int new_VAO, unsigned int const& numVertices, Shader *new_shader);

    // draw func
    void draw();
};

#endif