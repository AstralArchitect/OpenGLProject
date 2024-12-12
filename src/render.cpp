#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tools/camera.hpp>
#include <tools/object.hpp>

#include "render.hpp"

// settings
extern const unsigned int SCR_WIDTH = 1920;
extern const unsigned int SCR_HEIGHT = 1080;

// camera
extern Camera camera;

// timing
extern float deltaTime;
extern float lastFrame;

extern const double PI;

extern glm::vec3 lightPos;

void Render::renderFrame(GLFWwindow *window, std::vector<GLuint> planTexts, Object &plan, Object &gltf_model, Object &light, glm::mat4 lightSpaceMatrix)
{
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
    plan.shader->setVec3("lightPos", lightPos);
    plan.shader->setMat4("projection", projection);
    plan.shader->setMat4("view", view);
    plan.shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

    //set the light positions
    float angle = 3.14;
    glm::vec3 pointLightPosition = {cos(angle + (glfwGetTime() / 1.0f)), 0.5, sin(angle + (glfwGetTime() / 1.0f))};

    // world transformation
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(4.0f, 1.0f, 4.0f));
    plan.shader->setMat4("model", model);

    plan.draw();

    // gltf model
    // ----------
    gltf_model.shader->use();
    gltf_model.shader->setMat4("projection", projection);
    gltf_model.shader->setMat4("view", view);

    // world transformation
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.25f));
    gltf_model.shader->setMat4("model", model);

    gltf_model.shader->setVec3("color", glm::vec3(1.0f, 0.5f, 0.5f));

    // draw
    gltf_model.draw();

    // Light object
    // ------------
    light.shader->use();
    light.shader->setMat4("projection", projection);
    light.shader->setMat4("view", view);

    model = glm::mat4(1.0f);
    model = glm::translate(model, pointLightPosition);
    model = glm::scale(model, glm::vec3(0.2f));
    light.shader->setMat4("model", model);
        
    glm::vec3 pointLightColor = glm::vec3(1.0f, 0.9f, 0.8f);
    light.shader->setVec3("color", pointLightColor);

    light.draw();
}