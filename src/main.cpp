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
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 2.7f));

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

const double PI = 3.14159265358979323846264338328;

void setPointLight(glm::vec3 const& lightPos, Shader const& lightingShader);

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
    unsigned int planVBO, planVAO;

    glGenVertexArrays(1, &planVAO);
    glGenBuffers(1, &planVBO);

    glBindVertexArray(planVAO);

    glBindBuffer(GL_ARRAY_BUFFER, planVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planVertices), planVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

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
    unsigned int LightCubeVBO, lightCubeVAO;

    glGenBuffers(1, &LightCubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, LightCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightCubeVertices), lightCubeVertices, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, LightCubeVBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Shader lightShader = Shader("./res/shaders/light_cube/vertex.vs", "./res/shaders/light_cube/fragment.fs");

    char* texturePaths[2] = {
        (char*)"res/textures/bois.jpg",
        (char*)"res/textures/bois_specular.jpg"
    };

    Shader planShader = Shader("./res/shaders/plan/plan.vs", "./res/shaders/plan/plan.fs");

    std::vector<GLuint> planTexts = {loadTexture(texturePaths[0], true), loadTexture(texturePaths[1], false)};
    planShader.use();
    planShader.setInt("colorMap", 0);
    planShader.setInt("specMap", 1);
    
    GltfModel gltf_model("./res/models/test-model.glb");
    Shader gltfshader("res/shaders/glbModel/vertex.vs", "res/shaders/glbModel/fragment.fs");
    // create the model texture
    gltfshader.use();
    gltfshader.setInt("tex", 0);

    Object plan(planVAO, 6, &planShader);
    Object lightCube(lightCubeVAO, 36, &lightShader);
    Object gltfObj(&gltf_model, &gltfshader);

    // shadows
    // -------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
    SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

        // render attributes
        // -----------------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Render::renderFrame(window, planTexts, plan, gltfObj, lightCube);

        // view/projection/world transformations
    // -------------------------------
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 model;

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, planTexts[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, planTexts[1]);

    // The plan object
    // ---------------
    plan.shader->use();
    plan.shader->setVec3("viewPos", camera.Position);
    plan.setWorld(projection, view);

    //set the light positions
    float angle = 3.14;
    glm::vec3 pointLightPosition = {cos(angle + (glfwGetTime() / 1.0f)), 0.5, sin(angle + (glfwGetTime() / 1.0f))};

    setPointLight(pointLightPosition, *plan.shader);

    // world transformation
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(4.0f, 1.0f, 4.0f));
    plan.shader->setMat4("model", model);

    plan.draw();

    // gltf model
    // ----------
    gltfObj.shader->use();
    gltfObj.setWorld(projection, view);

    // world transformation
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.25f));
    gltfObj.shader->setMat4("model", model);

    gltfObj.shader->setVec3("color", glm::vec3(1.0f, 0.5f, 0.5f));

    // draw
    //gltfObj.draw();
    gltf_model.draw();

    // Light object
    // ------------
    lightCube.shader->use();
    lightCube.shader->setMat4("projection", projection);
    lightCube.shader->setMat4("view", view);

    model = glm::mat4(1.0f);
    model = glm::translate(model, pointLightPosition);
    model = glm::scale(model, glm::vec3(0.2f));
    lightCube.shader->setMat4("model", model);
        
    glm::vec3 pointLightColor = glm::vec3(1.0f, 0.9f, 0.8f);
    lightCube.shader->setVec3("color", pointLightColor);

    lightCube.draw();

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
