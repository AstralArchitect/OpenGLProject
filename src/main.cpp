#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tools/camera.hpp>
#include <tools/shader.hpp>
#include <tools/gltfloader.hpp>
#include <tools/object.hpp>

#include "callbacks.hpp"
#include "render.hpp"

#include <cstdio>

// settings
unsigned int SCR_WIDTH  = 1920;
unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 2.7f));

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting info
// -------------
glm::vec3 lightPos(1.0/*cos(0)*/, 1.0, 0.0f/*sin(0)*/);
glm::vec3 pointLightColor = glm::vec3(1.0f, 0.9f, 0.8f);

// background strength
glm::vec3 backgroundColor(0.15f, 0.15f, 0.15f);

int main()
{
    // glfw window creation
    // --------------------
    char error;
    GLFWwindow* window = createContextAndWindows(SCR_WIDTH, SCR_HEIGHT, (char*)"Horloge", &error);
    if (window == NULL)
    {
        if (error == 1)
        {
            printf("Failed to create GLFW window");
        }
        else if (error == 2)
        {
            printf("Failed to initialize GLAD");
        }
        return -1;
    }

    float planVertices[]{
        // positions          // normal            // texture coords
        //first triangle
        -0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  4.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  4.0f,  4.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  4.0f,  0.0f,
        // second triangle
         0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  4.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  4.0f
    };

    float lightCubeVertices[]{
        // positions
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    Object lightCube(lightCubeVertices, sizeof(lightCubeVertices), true, false, false, "./res/shaders/light_cube/vertex.vs", "./res/shaders/light_cube/fragment.fs");

    std::vector<GLuint> planText = {loadTexture((char*)"res/textures/bois.jpg", true), loadTexture((char*)"res/textures/bois_specular.jpg", true)};
    Object plan(planVertices, sizeof(planVertices), true, true, true, "res/shaders/plan/plan.vs", "res/shaders/plan/plan.fs", planText);
    plan.shader->use();
    plan.shader->setInt("colorMap", 0);
    plan.shader->setInt("specularMap", 1);
    plan.shader->setInt("shadowMap", 2);

    Object gltfObj("./res/models/test-model.glb", "res/shaders/glbModel/vertex.vs", "res/shaders/glbModel/fragment.fs", "res/shaders/glbModel/depth.vs", "res/shaders/glbModel/depth.fs");
    // create the model texture
    gltfObj.shader->use();
    gltfObj.shader->setInt("tex", 0);

    // uniform buffer
    // --------------
    unsigned int light_index = glGetUniformBlockIndex(plan.shader->ID, "Light");
    glUniformBlockBinding(plan.shader->ID, light_index, 0);

    unsigned int uboLight;
    glGenBuffers(1, &uboLight);

    glBindBuffer(GL_UNIFORM_BUFFER, uboLight);
    glBufferData(GL_UNIFORM_BUFFER, 28, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboLight, 0, 28);
    
    struct Light {
        glm::vec3 color      = pointLightColor;
        float lightConstant  = 1.0f;
        float lightLinear    = .7f;
        float lightQuadratic = 1.8f;   
    } light;

    glBindBuffer(GL_UNIFORM_BUFFER, uboLight);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 28, &light);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // shadows
    // -------
    Callback::Shadow_info shadow_info = {4096, 4096, 0, 0}; 
    shadow_info.init();
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        Callback::processInput(window);

        // render
        // ------
        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 0.1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. render depth of scene to texture (from light's perspective)
        // --------------------------------------------------------------
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = .1f, far_plane = 7.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        glViewport(0, 0, shadow_info.SHADOW_WIDTH, shadow_info.SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, shadow_info.depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            Render::renderScene(window, plan, gltfObj, lightCube, lightSpaceMatrix);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 2. render scene as normal using the generated depth/shadow map  
        // --------------------------------------------------------------
        Render::renderFrame(window, plan, gltfObj, lightCube, lightSpaceMatrix, shadow_info.depthMap);

        GLenum err = 1;
        while (err != 0) {
            if (err != 0 && err != 1) {
                std::cout << "OpenGL Error occured: " << err << std::endl;
            }

            err = glGetError();
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}