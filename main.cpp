#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <camera.h>
#include <shader.h>

#include "functions.h"

#include <stdio.h>

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

    // build and compile plan shader
    Shader Plan("plan.vs", "plan.fs");

    float planVertices[]{
        // positions          // texture coords
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, // bottom left
        -0.5f, -0.5f,  0.5f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VBO, VAO, EBO;

    bindPlanVertices(&VAO, &VBO, &EBO, planVertices, indices);

    //load texture
    unsigned int texture = loadTexture("res/bois.jpg");

    //shader configuration
    Plan.use();
    Plan.setInt("myTexture", 0);

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
        Plan.setVec3("viewPos", camera.Position);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        Plan.setMat4("projection", projection);
        Plan.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(4.0f, 1.0f, 4.0f));
        Plan.setMat4("model", model);

        //bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // render the cube
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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