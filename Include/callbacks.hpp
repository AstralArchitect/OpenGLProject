#include "./glad/glad.h"
#include <GLFW/glfw3.h>

namespace Callback {
    void framebuffer_size(GLFWwindow* window, int width, int height);
    void scroll(GLFWwindow* window, double xoffset, double yoffset);
    void mouse(GLFWwindow * window, double xposIn, double yposIn);
    void processInput(GLFWwindow *window);
    void processInput(GLFWwindow *window);
}
unsigned int loadTexture(const char *path, bool gammaCorrection, GLuint mode = GL_MIRRORED_REPEAT);
GLFWwindow *createContextAndWindows(const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT, char *WindowTitle, char *error);