#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tools/camera.h>
#include <tools/shader.h>
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

//light mode
char mode = 0;

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
         0.5f, -0.5f,  0.5f,   4.0f, 4.0f,      0.0f,  1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   4.0f, 0.0f,      0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,      0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 4.0f,      0.0f,  1.0f, 0.0f
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

    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f, 0.2f, 2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3( 0.0f, 0.0f, -3.0f)
    };
    glm::vec3 pointLightColors[3];

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        Plan.use();
        Plan.setVec3("viewPos", camera.Position);
        Plan.setFloat("material.shininess", 32.0f);

        /*
           Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index 
           the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
           by defining light types as classes and set their values in there, or by using a more efficient uniform approach
           by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
        */
        float linear, quadratic;
        //directionnal light direction
        Plan.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        if (mode == 0)
        {
            glClearColor(0.003f, 0.003f, 0.003f, 1.0f);

            for (int i = 0; (unsigned long long)i < sizeof(pointLightColors) / sizeof(glm::vec3) + 1; i++)
            {
                pointLightColors[i] = glm::vec3(1.0f, 1.0f, 1.0f);
            }

            linear = 0.09;
            quadratic = 0.032;

            // directional light
            Plan.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
            Plan.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
            Plan.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        }
        else if (mode == 1)
        {
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

            pointLightColors[0] = glm::vec3(0.2f, 0.2f, 0.6f);
            pointLightColors[1] = glm::vec3(0.3f, 0.3f, 0.7f);
            pointLightColors[2] = glm::vec3(0.0f, 0.0f, 0.3f);
            pointLightColors[3] = glm::vec3(0.4f, 0.4f, 0.4f);

            linear = 0.09;
            quadratic = 0.032;

            // directional light
            Plan.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.1f);
            Plan.setVec3("dirLight.diffuse", 0.2f, 0.2f, 0.7f);
            Plan.setVec3("dirLight.specular", 0.7f, 0.7f, 0.7f);
        }
        else if (mode == 2)
        {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            pointLightColors[0] = glm::vec3(0.1f, 0.1f, 0.1f);
            pointLightColors[1] = glm::vec3(0.1f, 0.1f, 0.1f);
            pointLightColors[2] = glm::vec3(0.1f, 0.1f, 0.1f);
            pointLightColors[3] = glm::vec3(0.3f, 0.1f, 0.1f);

            linear = 0.14;
            quadratic = 0.07;

            // directional light
            Plan.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
            Plan.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.05f);
            Plan.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);
        }
        else if (mode == 3)
        {
            glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

            pointLightColors[0] = glm::vec3(0.4f, 0.7f, 0.1f);
            pointLightColors[1] = glm::vec3(0.4f, 0.7f, 0.1f);
            pointLightColors[2] = glm::vec3(0.4f, 0.7f, 0.1f);
            pointLightColors[3] = glm::vec3(0.4f, 0.7f, 0.1f);

            linear = 0.07;
            quadratic = 0.017;

            // directional light
            Plan.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
            Plan.setVec3("dirLight.diffuse", 1.0f, 1.0f, 1.0f);
            Plan.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
        }
        else if (mode == 4)
        {
            glClearColor(0.75f, 0.52f, 0.3f, 1.0f);

            pointLightColors[0] = glm::vec3(1.0f, 0.6f, 0.0f);
            pointLightColors[1] = glm::vec3(1.0f, 0.0f, 0.0f);
            pointLightColors[2] = glm::vec3(1.0f, 1.0, 0.0);
            pointLightColors[3] = glm::vec3(0.2f, 0.2f, 1.0f);

            linear = 0.09;
            quadratic = 0.032;

            // directional light
            Plan.setVec3("dirLight.ambient", 0.3f, 0.24f, 0.14f);
            Plan.setVec3("dirLight.diffuse", 0.7f, 0.42f, 0.26f);
            Plan.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        }
        
        // point light 1
        Plan.setVec3("pointLights[0].position", pointLightPositions[0]);
        Plan.setVec3("pointLights[0].ambient", pointLightColors[0] * glm::vec3(0.1, 0.1, 0.1));
        Plan.setVec3("pointLights[0].diffuse", pointLightColors[0]);
        Plan.setVec3("pointLights[0].specular", pointLightColors[0]);
        Plan.setFloat("pointLights[0].constant", 1.0f);
        Plan.setFloat("pointLights[0].linear", linear);
        Plan.setFloat("pointLights[0].quadratic", quadratic);
        // point light 2
        Plan.setVec3("pointLights[1].position", pointLightPositions[1]);
        Plan.setVec3("pointLights[1].ambient", pointLightColors[1] * glm::vec3(0.1, 0.1, 0.1));
        Plan.setVec3("pointLights[1].diffuse", pointLightColors[1]);
        Plan.setVec3("pointLights[1].specular", pointLightColors[1]);
        Plan.setFloat("pointLights[1].constant", 1.0f);
        Plan.setFloat("pointLights[1].linear", linear);
        Plan.setFloat("pointLights[1].quadratic", quadratic);
        // point light 3
        Plan.setVec3("pointLights[2].position", pointLightPositions[2]);
        Plan.setVec3("pointLights[2].ambient", pointLightColors[2] * glm::vec3(0.1, 0.1, 0.1));
        Plan.setVec3("pointLights[2].diffuse", pointLightColors[2]);
        Plan.setVec3("pointLights[2].specular", pointLightColors[2]);
        Plan.setFloat("pointLights[2].constant", 1.0f);
        Plan.setFloat("pointLights[2].linear", linear);
        Plan.setFloat("pointLights[2].quadratic", quadratic);
        // point light 4
        Plan.setVec3("pointLights[3].position", pointLightPositions[3]);
        Plan.setVec3("pointLights[3].ambient", pointLightColors[3] * glm::vec3(0.1, 0.1, 0.1));
        Plan.setVec3("pointLights[3].diffuse", pointLightPositions[3]);
        Plan.setVec3("pointLights[3].specular", pointLightPositions[3]);
        Plan.setFloat("pointLights[3].constant", 1.0f);
        Plan.setFloat("pointLights[3].linear", linear);
        Plan.setFloat("pointLights[3].quadratic", quadratic);

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

        // also draw the lamp object(s)
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        const int nb_lampes = 4;

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < nb_lampes; i++)
        {
            lightCubeShader.setVec3("color", pointLightColors[i]);
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightCubeShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
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