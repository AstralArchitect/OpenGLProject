#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tools/camera.hpp>
#include <tools/shader.hpp>
#include <tools/gltfloader.hpp>

#include "callbacks.hpp"

#include <cstdio>

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 2.7f));

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

    float planVertices[]{
        // positions          // texture coords //normals
        //first triangle
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,      1.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,   4.0f, 0.0f,      1.0f, 0.0f, -1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 4.0f,      1.0f, 0.0f, -1.0f,
        //second triangle
        -0.5f, -0.5f,  0.5f,   0.0f, 4.0f,      1.0f, 0.0f, -1.0f,
         0.5f, -0.5f,  0.5f,   4.0f, 4.0f,      1.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,   4.0f, 0.0f,      1.0f, 0.0f, -1.0f,
    };
    unsigned int planVBO, planVAO;

    glGenVertexArrays(1, &planVAO);
    glGenBuffers(1, &planVBO);

    glBindVertexArray(planVAO);

    glBindBuffer(GL_ARRAY_BUFFER, planVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planVertices), planVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
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
        (char*)"res/bois.jpg",
        (char*)"res/bois_specular.jpg"
    };

    Shader planShader = Shader("./res/shaders/plan/plan.vs", "./res/shaders/plan/plan.fs");

    GLuint planTexts[2] = {loadTexture(texturePaths[0]), loadTexture(texturePaths[1])};
    glm::vec3 pointLightColor = glm::vec3(1.0f);
    planShader.use();
    planShader.setInt("colorMap", 0);
    planShader.setInt("specMap", 1);

    // point light attributes
    planShader.setVec3("ambient", pointLightColor * glm::vec3(0.1));
    planShader.setVec3("diffuse", pointLightColor);
    planShader.setVec3("specular", pointLightColor);
    planShader.setFloat("pointLight.constant", 1.0f);
    planShader.setFloat("pointLight.linear", 0.045);
    planShader.setFloat("pointLight.quadratic", 0.0075);
    
    GltfModel gltf_model = GltfModel::loadWithPath("./res/models/cube.glb");
    Shader gltfshader = Shader("res/shaders/glbModel/vertex.vs", "res/shaders/glbModel/fragment.fs");
    // create the model texture
    unsigned int texture = loadTexture(texturePaths[0]);
    gltfshader.use();
    gltfshader.setInt("tex", 2);
    
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
        processInput(window);

        // render attributes
        // -----------------
        glClearColor(0.003f, 0.003f, 0.003f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture);

        // The plan object
        // ---------------
        planShader.use();
        planShader.setVec3("viewPos", camera.Position);
        planShader.setMat4("projection", projection);
        planShader.setMat4("view", view);

        //set the light positions
        float angle = 3.14;
        glm::vec3 pointLightPosition = {cos(angle + (glfwGetTime() / 1.0f)), 0.5, sin(angle + (glfwGetTime() / 1.0f))};

        planShader.setVec3("lightPos", pointLightPosition);

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(4.0f, 1.0f, 4.0f));
        planShader.setMat4("model", model);

        // render the plan
        glBindVertexArray(planVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // unbind shader and VAO
        glBindVertexArray(0);
        planShader.unuse();

        // gltf model
        // ----------
        gltfshader.use();
        gltfshader.setMat4("projection", projection);
        gltfshader.setMat4("view", view);

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.5f));
        gltfshader.setMat4("model", model);

        gltfshader.setVec3("color", glm::vec3(1.0f, 0.5f, 0.5f));

        // draw
        gltf_model.draw();

        // Light object
        // ------------
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPosition);
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader.setMat4("model", model);
        
        lightShader.setVec3("color", pointLightColor);

        // render
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // unbinde shader & VAO
        glBindVertexArray(0);
        lightShader.unuse();

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