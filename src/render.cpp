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
extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

// camera
extern Camera camera;

// timing
extern float deltaTime;
extern float lastFrame;

extern const double PI;

void renderFrame(GLFWwindow *window, unsigned int *planTexts, Shader &planShader, unsigned int &planVAO, Shader &gltfshader, GltfModel &gltf_model, Shader &light)
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
        planShader.use();
        planShader.setVec3("viewPos", camera.Position);
        planShader.setMat4("projection", projection);
        planShader.setMat4("view", view);

        //set the light positions
        float angle = 3.14;
        glm::vec3 pointLightPosition = {cos(angle + (glfwGetTime() / 1.0f)), 0.5, sin(angle + (glfwGetTime() / 1.0f))};

        setPointLight(pointLightPosition, planShader);

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
        model = glm::scale(model, glm::vec3(0.25f));
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

void setPointLight(glm::vec3 const& lightPos, Shader const& lightingShader)
{
    // directional light
    lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light
    lightingShader.setVec3("light.pos", lightPos);
    lightingShader.setVec3("light.ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("light.constant", 1.0f);
    lightingShader.setFloat("light.linear", 0.045f);
    lightingShader.setFloat("light.quadratic", 0.075f);
}