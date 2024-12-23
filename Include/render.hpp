#include <tools/object.hpp>

#include <iostream>
#include <vector>

namespace Render {
    void renderFrame(GLFWwindow *window, Object &plan, Object &gltf_model, Object &light, glm::mat4 lightSpaceMatrix, GLuint depthMap);
    void renderScene(GLFWwindow *window, Object &plan, Object &gltf_model, Object &light, Shader const& shader);
};