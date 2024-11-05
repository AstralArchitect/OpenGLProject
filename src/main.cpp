#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tools/camera.h>
#include <tools/model.h>
#include <tools/gltfloader.hpp>

#include "functions.hpp"

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

    Model *lamp = new Model("res/shaders/LightCube/vertex.vs", "res/shaders/LightCube/fragment.fs", lightCubeVAO, LightCubeVBO, 36, SCR_WIDTH, SCR_HEIGHT, NULL, 0);

    char* texturePaths[2] = {
        (char*)"res/bois.jpg",
        (char*)"res/bois_specular.jpg"
    };

    Model *plan = new Model("./res/shaders/Plan/plan.vs", "./res/shaders/Plan/plan.fs", planVAO, planVBO, 6, SCR_WIDTH, SCR_HEIGHT, texturePaths, 2);

    //shader configuration
    plan->use();
    plan->setInt("material.diffuse", 0);
    plan->setInt("material.specular", 1);

    glm::vec3 pointLightPosition;
    glm::vec3 pointLightColor = glm::vec3(1.0f);

    //directionnal light direction
    plan->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        
    // point light
    plan->setVec3("pointLight.position", pointLightPosition);
    plan->setVec3("pointLight.ambient", pointLightColor * glm::vec3(0.1));
    plan->setVec3("pointLight.diffuse", pointLightColor);
    plan->setVec3("pointLight.specular", pointLightColor);
    plan->setFloat("pointLight.constant", 1.0f);
    plan->setFloat("pointLight.linear", 0.045);
    plan->setFloat("pointLight.quadratic", 0.0075);

    // directional light
    plan->setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
    plan->setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
    plan->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    
    GltfModel gltf_model = GltfModel::loadWithPath("./res/models/cube.glb");
    Shader gltfshader = Shader("res/shaders/glbModel/vertex.vs", "res/shaders/glbModel/fragment.fs");
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
        glClearColor(0.003f, 0.003f, 0.003f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        plan->use();
        plan->setVec3("viewPos", camera.Position);
        plan->setFloat("material.shininess", 16.0f);

        //set the light positions
        float angle = 3.14149265;
        pointLightPosition.x = cos(angle + (glfwGetTime() / 1.0f));
        pointLightPosition.z = sin(angle + (glfwGetTime() / 1.0f));
        pointLightPosition.y = 0.5f;

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        plan->setMat4("projection", projection);
        plan->setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(4.0f, 1.0f, 4.0f));
        plan->setMat4("model", model);

        // render the plan
        plan->render();

        // gltf model
        // ----------
        gltfshader.use();
        gltfshader.setMat4("projection", projection);

        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.5f));

        gltfshader.setMat4("model", model);
        gltfshader.setMat4("view", view);
        gltfshader.setVec3("color", glm::vec3(1.0f, 0.5f, 0.5f));

        gltf_model.draw();

        // also draw the lamp object
        lamp->use();
        lamp->setMat4("projection", projection);
        lamp->setMat4("view", view);

        // we now draw as many light cubes as we have point lights.
        glBindVertexArray(lightCubeVAO);
        lamp->setVec3("color", pointLightColor);
        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPosition);
        model = glm::scale(model, glm::vec3(0.2f));
        lamp->setMat4("model", model);
        lamp->render();

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

    delete plan;
    delete lamp;

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}