#include "Skybox.hpp"
#include "Shader.hpp"
#include "../Components/Transform.hpp"
#include "Camera.hpp"

namespace HyperAPI {
    Skybox::Skybox(const std::string &right, const std::string &left,
                   const std::string &top, const std::string &bottom,
                   const std::string &front, const std::string &back) {
        shader = new Shader("shaders/skybox.glsl");
        facesCubemap.push_back(right);
        facesCubemap.push_back(left);
        facesCubemap.push_back(top);
        facesCubemap.push_back(bottom);
        facesCubemap.push_back(front);
        facesCubemap.push_back(back);

        HYPER_LOG("Skybox created")

        float skyboxVertices[] = {-1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,
                                  1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
                                  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
                                  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f};

        uint32_t skyboxIndices[] = {// Right
                                    1, 2, 6, 6, 5, 1,
                                    // Left
                                    0, 4, 7, 7, 3, 0,
                                    // Top
                                    4, 5, 6, 6, 7, 4,
                                    // Bottom
                                    0, 3, 2, 2, 1, 0,
                                    // Back
                                    0, 1, 5, 5, 4, 0,
                                    // Front
                                    3, 7, 6, 6, 2, 3};

        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glGenBuffers(1, &skyboxEBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices,
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices),
                     &skyboxIndices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                              (void *)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glGenTextures(1, &cubemapTexture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                        GL_CLAMP_TO_EDGE);

        for (uint32_t i = 0; i < 6; i++) {
            int width, height, nrChannels;

            unsigned char *data = stbi_load(facesCubemap[i].c_str(), &width,
                                            &height, &nrChannels, 0);
            if (data) {
                stbi_set_flip_vertically_on_load(false);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB,
                             width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            } else {
                std::cout << "Cubemap texture failed to load at path: "
                          << facesCubemap[i] << std::endl;
                stbi_image_free(data);
            }
        }

        shader->Bind();
        shader->SetUniform1i("skybox", 20);
    }

    void Skybox::Draw(Camera &camera, int width, int height) {
        glDepthFunc(GL_LEQUAL);

        shader->Bind();
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        if (camera.EnttComp) {
            auto &transform =
                Scene::m_Registry.get<Experimental::Transform>(camera.entity);
            view = glm::mat4(glm::mat3(glm::lookAt(
                transform.position, transform.position + transform.rotation,
                camera.Up)));
            projection =
                glm::perspective(glm::radians(45.0f),
                                 (float)width / (float)height, 0.1f, 5000.0f);
        } else {
            TransformComponent transform =
                camera.GetComponent<TransformComponent>();
            view = glm::mat4(glm::mat3(glm::lookAt(
                transform.position, transform.position + transform.rotation,
                camera.Up)));
            projection =
                glm::perspective(glm::radians(45.0f),
                                 (float)width / (float)height, 0.1f, 5000.0f);
        }

        shader->SetUniformMat4("view", view);
        shader->SetUniformMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE20);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDepthFunc(GL_LESS);
    }
} // namespace HyperAPI