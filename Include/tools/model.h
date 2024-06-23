#include <tools/camera.h>
#include <tools/shader.h>

#include <glm/glm.hpp>

class Model
{
private:
    unsigned int VAO, VBO;
    
    unsigned int *textures;
    unsigned int nbTextures;
    
    unsigned int nbTriangles;
    
    Camera *camera;

    unsigned int width, height;

    glm::mat4 model;
public:
    Shader *shader;

    //(à Nautilus-1021)Je te laisse faire cette fonction en Rust. J'ai rajouté la fonction `void Paths(const char* vertexPath, const char* fragmentPath)` dans la classe Shader qui est la même que le constructeur.
//    Model(const char* vertexShaderPath, const char* fragmentShaderPath, const char* modelPath);
    //default constructor
    Model(const char* vertexShaderPath, const char* fragmentShaderPath, const unsigned int new_VAO, const unsigned int new_VBO, unsigned int new_nbTRiangles, const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT, char* texturePaths[], unsigned int new_nbTextures);
    ~Model();

    //render the model
    void render();

    //transformations functions
    void translate(glm::vec3 newPos);
    void scale(glm::vec3 scaleVector);
    void rotate(float angle, glm::vec3 rotateVector);
};