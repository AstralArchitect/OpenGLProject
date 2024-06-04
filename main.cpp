#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <camera.h>
#include <shader.h>
#include <rustlib/main.hpp>

#include "functions.h"

#include <cstdio>

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 1.2f);

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

    //init functions from functions.cpp
    initFunctions(&camera, &deltaTime);

    // build and compile plan and lightCube shader
    Shader Plan("shaders/Plan/plan.vs", "shaders/Plan/plan.fs");
    Shader lightCubeShader("shaders/LightCube/lightCube.vs", "shaders/LightCube/lightCube.fs");

    float planVertices[]{
        // positions          // texture coords //normals
         0.5f, -0.5f,  0.5f,   4.0f, 4.0f,      1.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,   4.0f, 0.0f,      1.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,      1.0f,  0.0f, -1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 4.0f,      1.0f,  0.0f, -1.0f
    };
    unsigned int planIndices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int planVBO, planVAO, planEBO;

    bindPlanVertices(&planVAO, &planVBO, &planEBO, planVertices, planIndices);

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

    //load texture
    unsigned int diffuseMap = loadTexture("res/bois.jpg");
    unsigned int specularMap = loadTexture("res/bois_specular.jpg");

    //shader configuration
    Plan.use();
    Plan.setInt("material.diffuse", 0);
    Plan.setInt("material.specular", 1);

    printtest(80);

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Plan.use();
        Plan.setVec3("light.position", lightPos);
        Plan.setVec3("viewPos", camera.Position);

        // light properties
        Plan.setVec3("light.ambient", 0.7f, 0.7f, 0.7f);
        Plan.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        Plan.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // material properties
        Plan.setFloat("material.shininess", 16.0f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        Plan.setMat4("projection", projection);
        Plan.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(4.0f, 1.0f, 4.0f));
        Plan.setMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // render the plan
        glBindVertexArray(planVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, -1.0f, 0.0f));
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // render the cube
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //spin the light
        lightPos.x = sin(glfwGetTime());
        lightPos.z = cos(glfwGetTime());

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