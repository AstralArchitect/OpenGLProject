#include <tools/object.hpp>

#include <iostream>
#include <vector>

namespace Render {
    void renderFrame(GLFWwindow *window, std::vector<GLuint> planTexts, Object &plan, Object &gltf_model, Object &light);
};