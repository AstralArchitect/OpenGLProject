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

void Render::renderFrame(GLFWwindow *window, Object &plan, GltfModel &horloge, GltfModel &aiguille, GltfModel &light, glm::mat4 lightSpaceMatrix, GLuint depthMap)
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
    model = glm::scale(model, glm::vec3(4.f, 1.f, 4.f));
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

    // horloge
    // -------
    horloge.set_global_uniforms([&] (Shader* shader) {
        shader->use();
        shader->setVec3("viewPos", camera.Position);
        shader->setVec3("lightPos", lightPos);
        shader->setVec3("ambientColor", backgroundColor);
        shader->setInt("shadowMap", 2);
    }, view, projection);

    // draw
    horloge.draw();

    // aiguilles
    // ---------

    // set uniforms
    aiguille.set_global_uniforms([&] (Shader* shader) {
        shader->use();
        shader->setVec3("viewPos", camera.Position);
        shader->setVec3("lightPos", lightPos);
        shader->setVec3("ambientColor", backgroundColor);
        shader->setInt("shadowMap", 2);
    }, view, projection);
    
    // get the actual time
    auto now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    // Convert to local time components
    struct tm* local_time = localtime(&now_time);  // or gmtime() for UTC

    int8 minutes = local_time->tm_min;
    int32 hours = local_time->tm_hour;

    if (hours == 0) {
        hours = 12; // Convert 0 to 12 for midnight/noon
    }
    
    // world transformation
    model = glm::mat4(1.0f);
    //model = glm::rotate(model, (float)glfwGetTime() / 2.f, glm::vec3(0.0, 1.0, 0.0));
    model = glm::scale(model, glm::vec3(1.f/15.f));
    model = glm::translate(model, glm::vec3(.0, 0., 2.375));
    model = glm::rotate(model, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
    // convert the actual time into degrees
    auto min_model = glm::rotate(model, glm::radians(minutes * 6.f), glm::vec3(0.0, -1.0, 0.0));
    auto hours_model = glm::rotate(model, glm::radians(hours * 30.f), glm::vec3(0.0, -1.0, 0.0));
    hours_model = glm::scale(hours_model, glm::vec3(1.f, 1.f, 2.f/3.f));

    // draw minutes
    aiguille.set_global_uniforms(min_model);
    aiguille.draw();

    // draw hours
    aiguille.set_global_uniforms(hours_model);
    aiguille.draw();

    // Light object
    // ------------
    // world transformation
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.075f));

    light.set_global_uniforms([&] (Shader* shader) {}, model, view, projection);    

    //draw
    light.draw();
}

void Render::renderScene(GLFWwindow *window, Object &plan, GltfModel &horloge, GltfModel &aiguille, GltfModel &light, glm::mat4 const& lightSpaceMatrix)
{
    glm::mat4 model = glm::mat4(1.0f);
    // horloge
    model = glm::scale(model, glm::vec3(.5f));
    model = glm::translate(model, glm::vec3(.0, -2, 0.0));
    horloge.set_global_uniforms(model);
    horloge.draw(true, lightSpaceMatrix);

    // aiguilles
    // ---------

    // minutes
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.f/15.f));
    model = glm::translate(model, glm::vec3(.0, 0., 2.375));
    model = glm::rotate(model, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
    // get the actual minutes
    auto now = std::chrono::system_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now - std::chrono::system_clock::time_point{}).count();
    int minutes = seconds % 3600 / 60;
    // then convert it into degrees
    model = glm::rotate(model, glm::radians(minutes * 6.f), glm::vec3(0.0, -1.0, 0.0));

    aiguille.set_global_uniforms(model);

    // draw
    aiguille.draw();

    // heures
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.f/30.f));
    model = glm::translate(model, glm::vec3(.0, 0., 4.75));
    model = glm::rotate(model, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
    auto timePoint = std::chrono::system_clock::to_time_t(now);
    std::tm* timeinfo = std::localtime(&timePoint);
    int hours = timeinfo->tm_hour;
    model = glm::rotate(model, glm::radians(hours / 2.f * 15.f), glm::vec3(0.0, 1.0, 0.0));

    aiguille.set_global_uniforms(model);
    // draw
    aiguille.draw();
}