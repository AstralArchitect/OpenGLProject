#include <tools/object.hpp>
#include <tools/gltfloader.hpp>

#include <iostream>
#include <vector>

namespace Render {
    void renderFrame(GLFWwindow *window, Object &plan, GltfModel &horloge, GltfModel &aiguille, GltfModel &light, glm::mat4 lightSpaceMatrix, GLuint depthMap);
    void renderScene(GLFWwindow *window, Object &plan, GltfModel &horloge, GltfModel &aiguille, GltfModel &light, glm::mat4 const& lightSpaceMatrix);
};