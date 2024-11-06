#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <tools/shader.hpp>
#include <tools/camera.hpp>
#include <callbacks.hpp>

#include <stb_image.h>

extern Camera camera;

extern float deltaTime;

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
double lastX = SCR_WIDTH / 2.0;
double lastY = SCR_HEIGHT / 2.0;
bool firstMouse = true;

#ifdef _WIN32
#include <windows.h>

void sleep_ms(DWORD milliseconds) {
    Sleep(milliseconds);
}

#else

#include <time.h>

void sleep_ms(unsigned long milliseconds) {
    struct timespec ts;

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;

    nanosleep(&ts, NULL);
}


#endif

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Callback::processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
        camera.ProcessMouseMovement(deltaTime * 500, 0);
    }
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
        camera.ProcessMouseMovement(-(deltaTime) * 500, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
        static bool isFullscreen = false;
        static int windowedWidth, windowedHeight, windowedPosX, windowedPosY;
        if (!isFullscreen) {
            glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
            glfwGetWindowPos(window, &windowedPosX, &windowedPosY);

            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
        } else {
            glfwSetWindowMonitor(window, NULL, windowedPosX, windowedPosY, windowedWidth, windowedHeight, 0);
        }
        isFullscreen = !isFullscreen;
        sleep_ms(500);
    }
    
}

void Callback::mouse(GLFWwindow * window, double xposIn, double yposIn) {
    if (firstMouse)
    {
        lastX = xposIn;
        lastY = yposIn;
        firstMouse = false;
    }

    float xoffset = xposIn - lastX;
    float yoffset = lastY - yposIn; // reversed since y-coordinates go from bottom to top
    lastX = xposIn;
    lastY = yposIn;

    camera.ProcessMouseMovement(xoffset, yoffset, 1);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void Callback::framebuffer_size(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Callback::scroll(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

GLFWwindow *createContextAndWindows(const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT, char *WindowTitle, char *error)
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 128);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(1920, 1080, WindowTitle, NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        *error = (char)1;
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, Callback::framebuffer_size);
    glfwSetScrollCallback(window, Callback::scroll);
    glfwSetCursorPosCallback(window, Callback::mouse);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        *error = (char)2;
        return NULL;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    return window;
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}