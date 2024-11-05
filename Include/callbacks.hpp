#include "./glad/glad.h"
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
GLFWwindow *createContextAndWindows(const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT, char *WindowTitle, char *error);