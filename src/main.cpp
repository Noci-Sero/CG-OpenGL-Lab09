#undef GLFW_DLL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "Libs/Mesh.h"
#include "Libs/Shader.h"
#include "Libs/Window.h"
#include "Libs/stb_image.h"

const GLint WIDTH = 800, HEIGHT = 600;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void CreateTriangle() {
    GLfloat vertices[] =
        {
            // x      y      z    // texture
            -1.0f, -1.0f, 0.0f,    0.0f, 0.0f,
            0.0f, -1.0f, 1.0f,     0.5f, 0.0f,
            1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,      0.5f, 1.0f
        };

    unsigned int indices[] =
        {
            0, 3, 1,
            1, 3, 2,
            2, 3, 0,
            0, 1, 2};

    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 20, 12);
    for (int i = 0; i < 10; i++) {
        meshList.push_back(obj1);
    }
}

void CreateShaders() {
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

int main() {
    mainWindow = Window(WIDTH, HEIGHT, 3, 3);
    mainWindow.initialise();

    CreateTriangle();
    CreateShaders();

    /* Load image */
    unsigned int texture1ID;
    glGenTextures(1, &texture1ID);
    glBindTexture(GL_TEXTURE_2D, texture1ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width1, height1, nrChannels1;
    unsigned char* data1 = stbi_load("Textures/cloth.jpg", &width1, &height1, &nrChannels1, 0);

    if (!data1) {
        std::cout << "Failed to load texture" << std::endl;
    }
    else
    {
        GLenum format = GL_RGB;
        if (nrChannels1 == 1)
            format = GL_RED;
        else if (nrChannels1 == 3)
            format = GL_RGB;
        else if (nrChannels1 == 4)
            format = GL_RGBA;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width1, height1, 0, format, GL_UNSIGNED_BYTE, data1);
        stbi_image_free(data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    /* Load image */
    unsigned int texture2ID;
    glGenTextures(1, &texture2ID);
    glBindTexture(GL_TEXTURE_2D, texture2ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width2, height2, nrChannels2;
    unsigned char* data2 = stbi_load("Textures/paper.png", &width2, &height2, &nrChannels2, 0);

    if (!data2) {
        std::cout << "Failed to load texture" << std::endl;
    }
    else
    {
        GLenum format = GL_RGB;
        if (nrChannels2 == 1)
            format = GL_RED;
        else if (nrChannels2 == 3)
            format = GL_RGB;
        else if (nrChannels2 == 4)
            format = GL_RGBA;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width2, height2, 0, format, GL_UNSIGNED_BYTE, data2);
        stbi_image_free(data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    GLuint uniformModel = 0, uniformView = 0, uniformProjection = 0;

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);
    // glm::mat4 projection = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.1f, 100.0f);

    // Loop until window closed
    while (!mainWindow.getShouldClose()) {
        // Get + Handle user input events
        glfwPollEvents();

        // Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);

        // draw here
        shaderList[0].UseShader();
        uniformModel = shaderList[0].GetUniformLocation("model");
        uniformView = shaderList[0].GetUniformLocation("view");
        uniformProjection = shaderList[0].GetUniformLocation("projection");

        // glm::mat4 view = glm::mat4(1.0f);
        // glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view(1.0f);
        glm::vec3 cameraPos = glm::vec3(1.0f, 0.5f, 2.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, -0.3f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);
        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, up));
        glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

        // glm::mat4 cameraPosMat (1.0f);
        // cameraPosMat[3][0] = -cameraPos.x;
        // cameraPosMat[3][1] = -cameraPos.y;
        // cameraPosMat[3][2] = -cameraPos.z;

        // glm::mat4 cameraRotateMat (1.0f);
        // cameraRotateMat[0] = glm::vec4(cameraRight.x, cameraUp.x, -cameraDirection.x, 0.0f);
        // cameraRotateMat[1] = glm::vec4(cameraRight.y, cameraUp.y, -cameraDirection.y, 0.0f);
        // cameraRotateMat[2] = glm::vec4(cameraRight.z, cameraUp.z, -cameraDirection.z, 0.0f);
        // view = cameraRotateMat * cameraPosMat;
        view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);

        glm::vec3 pyramidPositions[] =
            {
                glm::vec3(0.0f, 0.0f, -2.5f),
                glm::vec3(2.0f, 5.0f, -15.0f),
                glm::vec3(-1.5f, -2.2f, -2.5f),
                glm::vec3(-3.8f, -2.0f, -12.3f),
                glm::vec3(2.4f, -0.4f, -3.5f),
                glm::vec3(-1.7f, 3.0f, -7.5f),
                glm::vec3(1.3f, -2.0f, -2.5f),
                glm::vec3(1.5f, 2.0f, -2.5f),
                glm::vec3(1.5f, 0.2f, -1.5f),
                glm::vec3(-1.3f, 1.0f, -1.5f)};
        // Object
        for (int i = 0; i < 10; i++) {
            glm::mat4 model(1.0f);
            model = glm::translate(model, pyramidPositions[i]);
            model = glm::rotate(model, glm::radians(2.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
            model = glm::scale(model, glm::vec3(0.8f, 0.8f, 1.0f));

            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

            GLint uniformTexture1 = shaderList[0].GetUniformLocation("texture1");
            glUniform1i(uniformTexture1, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture1ID);
            
            GLint uniformTexture2 = shaderList[0].GetUniformLocation("texture2");
            glUniform1i(uniformTexture2, 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture2ID);

            meshList[i]->RenderMesh();
        }

        glUseProgram(0);
        // end draw
        mainWindow.swapBuffers();
    }

    return 0;
}
