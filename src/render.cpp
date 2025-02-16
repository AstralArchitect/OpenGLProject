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

void Render::renderFrame(GLFWwindow *window, Object &plan, GltfModel &gltf_model, GltfModel &light, glm::mat4 lightSpaceMatrix, GLuint depthMap)
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

    plan.shader.use();
    plan.shader.setVec3("viewPos", camera.Position);
    plan.shader.setVec3("lightPos", lightPos);
    plan.shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    // world transformation
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(4.0f, 1.0f, 4.0f));
    plan.shader.setMat4("model", model);
    plan.shader.setMat4("view", view);
    plan.shader.setMat4("projection", projection);
    
    plan.shader.setVec3("ambientColor", backgroundColor);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, plan.getText(0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, plan.getText(1));
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    plan.draw();

    // gltf model
    // ----------

    // world transformation
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(0.0f, -0.25f, .3f));
    model = glm::scale(model, glm::vec3(1.0f / 10));

    gltf_model.set_global_uniforms([&] (Shader* shader) {
        shader->use();
        shader->setVec3("viewPos", camera.Position);
        shader->setVec3("lightPos", lightPos);
        shader->setVec3("ambientColor", backgroundColor);
        shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        shader->setVec3("ambientColor", backgroundColor);
        shader->setMat4("model", model);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
    });

    // draw
    gltf_model.draw();

    // Light object
    // ------------
    light.set_global_uniforms([&] (Shader* shader) {
        shader->use();
        shader->setMat4("projection", projection);

        shader->setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.075f));
        shader->setMat4("model", model);
            
        shader->setVec3("color", pointLightColor);
    });    

    light.draw();
}

void Render::renderScene(GLFWwindow *window, Object &plan, GltfModel &gltf_model, GltfModel &light, glm::mat4 const& lightSpaceMatrix)
{
    glm::mat4 model = glm::mat4(1.0f);
    // gltf model
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(0.0f, -0.25f, .3f));
    model = glm::scale(model, glm::vec3(1.0f / 10));
    /*gltf_model.depthShader->use();
    gltf_model.depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    gltf_model.depthShader->setMat4("model", model);*/
    gltf_model.draw();
}