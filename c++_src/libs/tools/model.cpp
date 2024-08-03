#include <tools/model.h>

#include <glad/glad.h>
#include <stb_image.h>

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        printf("Texture failed to load at path: %s", path);
        stbi_image_free(data);
    }

    return textureID;
}

Model::Model(const char* vertexShaderPath, const char* fragmentShaderPath, const unsigned int new_VAO, const unsigned int new_VBO, unsigned int new_nbTRiangles, const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT, char* texturePaths[], unsigned int new_nbTextures)
{
    shader = new Shader(vertexShaderPath, fragmentShaderPath);

    VAO = new_VAO;
    VBO = new_VBO;

    nbTriangles = new_nbTRiangles;

    nbTextures = new_nbTextures;

    textures = (unsigned int *)malloc(nbTextures * sizeof(unsigned int));

    for (unsigned int i = 0; i < nbTextures; i++)
    {
        textures[i] = loadTexture(texturePaths[i]);
    }
}

Model::~Model()
{
    glDeleteProgram(shader->ID);
    glDeleteTextures(nbTextures, &textures[0]);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Model::render()
{
    //active and bind texture
    int actualTexture = GL_TEXTURE0;
    for (unsigned int i = 0; i < nbTextures; i++)
    {
        glActiveTexture(actualTexture);
        glBindTexture(GL_TEXTURE_2D, textures[i]);

        actualTexture++;
    }

    //draw object
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, nbTriangles);
}

void Model::translate(glm::vec3 translateVector)
{
    model = glm::translate(model, translateVector);
}

void Model::scale(glm::vec3 scaleVector)
{
    model = glm::scale(model, scaleVector);
}

void Model::rotate(float angle, glm::vec3 rotateVector)
{
    model = glm::rotate(model, angle, rotateVector);
}

// activate the shader
// ------------------------------------------------------------------------
void Model::use() const
{ 
    glUseProgram(shader->ID); 
}

// utility uniform functions
// ------------------------------------------------------------------------
void Model::setBool(const std::string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(shader->ID, name.c_str()), (int)value); 
}
// ------------------------------------------------------------------------
void Model::setInt(const std::string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(shader->ID, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Model::setFloat(const std::string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(shader->ID, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Model::setVec2(const std::string &name, const glm::vec2 &value) const
{ 
    glUniform2fv(glGetUniformLocation(shader->ID, name.c_str()), 1, &value[0]); 
}
void Model::setVec2(const std::string &name, float x, float y) const
{ 
    glUniform2f(glGetUniformLocation(shader->ID, name.c_str()), x, y); 
}
// ------------------------------------------------------------------------
void Model::setVec3(const std::string &name, const glm::vec3 &value) const
{ 
    glUniform3fv(glGetUniformLocation(shader->ID, name.c_str()), 1, &value[0]); 
}
void Model::setVec3(const std::string &name, float x, float y, float z) const
{ 
    glUniform3f(glGetUniformLocation(shader->ID, name.c_str()), x, y, z); 
}
// ------------------------------------------------------------------------
void Model::setVec4(const std::string &name, const glm::vec4 &value) const
{ 
    glUniform4fv(glGetUniformLocation(shader->ID, name.c_str()), 1, &value[0]); 
}
void Model::setVec4(const std::string &name, float x, float y, float z, float w) const
{ 
    glUniform4f(glGetUniformLocation(shader->ID, name.c_str()), x, y, z, w); 
}
// ------------------------------------------------------------------------
void Model::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(shader->ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Model::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(shader->ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Model::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}