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

    Shader *shader;
public:

    //(à Nautilus-1021)Je te laisse faire cette fonction en Rust. J'ai rajouté la fonction `void Paths(const char* vertexPath, const char* fragmentPath)` dans la classe Shader qui est la même que le constructeur.
//    Model(const char* vertexShaderPath, const char* fragmentShaderPath, const char* modelPath);
    //default constructor
    Model(const char* vertexShaderPath, const char* fragmentShaderPath, const unsigned int new_VAO, const unsigned int new_VBO, unsigned int new_nbTRiangles, const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT, char* texturePaths[], unsigned int new_nbTextures);
    ~Model();

    //render the model
    void render();

    //transformations functions
    //------------------------------------------------------------------------
    void translate(glm::vec3 newPos);
    //------------------------------------------------------------------------
    void scale(glm::vec3 scaleVector);
    //------------------------------------------------------------------------
    void rotate(float angle, glm::vec3 rotateVector);

    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const;
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const;
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const;
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec2(const std::string &name, float x, float y) const;
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
};