#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tools/camera.hpp>
#include <tools/object.hpp>

#include "render.hpp"

// settings
extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;

// camera
extern Camera camera;

// timing
extern float deltaTime;
extern float lastFrame;

extern glm::vec3 lightPos;
extern glm::vec3 pointLightColor;

// background strength
extern glm::vec3 backgroundColor;

extern bool hardMode;

void Render::renderFrame(GLFWwindow *window, Object &plan, Object &gltf_model, Object &light, glm::mat4 lightSpaceMatrix, GLuint depthMap)
{
    // view/projection/world transformations
    // -------------------------------
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 model;

    // The plan object
    // ---------------
    // update the light positions
    lightPos = {cos(glfwGetTime() / 2), 1.0f, sin(glfwGetTime() / 2)};

    plan.shader->use();
    plan.shader->setVec3("viewPos", camera.Position);
    plan.shader->setVec3("lightPos", lightPos);
    plan.shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

    // world transformation
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(4.0f, 1.0f, 4.0f));
    plan.shader->setMat4("model", model);
    plan.shader->setMat4("view", view);
    plan.shader->setMat4("projection", projection);

    if (hardMode)
    {
        plan.shader->setBool("hardShadows", true);
    }
    else
    {
        plan.shader->setBool("hardShadows", false);
    }
    

    plan.shader->setVec3("ambientStrength", backgroundColor);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, plan.getText());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);

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
    gltf_model.shader->setMat4("view", view);
    gltf_model.shader->setMat4("projection", projection);

    // draw
    gltf_model.draw();

    // Light object
    // ------------
    light.shader->use();
    light.shader->setMat4("projection", projection);
    light.shader->setMat4("view", view);

    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    light.shader->setMat4("model", model);
        
    light.shader->setVec3("color", pointLightColor);

    light.draw();
}

void Render::renderScene(GLFWwindow *window, Object &plan, Object &gltf_model, Object &light, glm::mat4 const& lightSpaceMatrix)
{
    glm::mat4 model = glm::mat4(1.0f);
    // gltf model
    model = glm::scale(model, glm::vec3(0.25f));
    gltf_model.depthShader->use();
    gltf_model.depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    gltf_model.depthShader->setMat4("model", model);
    gltf_model.drawWithoutTexture();
}