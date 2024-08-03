#include "./glad/glad.h"
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
void bindPlanVertices(unsigned int *VAO, unsigned int *VBO, unsigned int *EBO, float vertices[20], unsigned int indices[6]);
GLFWwindow *createContextAndWindows(const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT, char *WindowTitle, char *error);