#include <tools/shader.h>
#include <tools/camera.h>

class Model
{
private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int *texture;
    Camera *camera;
public:
    Shader shader;

    //(A Nautilus-1021)Je te laisse faire cette fonction en Rust. J'ai rajouté la fonction `void Paths(const char* vertexPath, const char* fragmentPath)` dans la classe Shader qui est la même que le constructeur.
    Model(const char* vertexShaderPath, const char* fragmentShaderPath, const char* modelPath);
    ~Model();
};