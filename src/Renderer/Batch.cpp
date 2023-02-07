#include "Batch.hpp"
#include <array>
#include <cstdint>
#include <string>

namespace HyperAPI {
    Batch::Batch() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &IBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                     nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size() * sizeof(uint32_t), nullptr,
                     GL_DYNAMIC_DRAW);

        // coords
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)0);
        glEnableVertexAttribArray(0);

        // color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, color));
        glEnableVertexAttribArray(1);

        // normals
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);

        // texuv
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, texUV));
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 4, GL_INT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, m_BoneIDs));
        glEnableVertexAttribArray(4);

        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, m_Weights));
        glEnableVertexAttribArray(5);

        glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(6);

        glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *)offsetof(Vertex, bitangent));
        glEnableVertexAttribArray(7);

        glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, transformIndex));
        glEnableVertexAttribArray(8);

        // error check
        if (glGetError() != GL_NO_ERROR) {
            HYPER_LOG("Error creating mesh");
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    };

    void Batch::AddMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, Experimental::Transform *transform) {
        transforms.push_back(transform);
        uint32_t offset = 0;
        for (auto &index : this->indices) {
            if (index > offset) {
                offset = index;
            }
        }

        if (offset == 0)
            offset = 0;
        else
            offset++;

        for (auto index : indices) {
            HYPER_LOG(std::string("Index: ") + std::to_string(offset + index))
            this->indices.push_back(offset + index);
        }

        for (auto vertex : vertices) {
            std::cout << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << std::endl;
            vertex.transformIndex = transforms.size() - 1;
            this->vertices.push_back(vertex);
        }
    }

    void Batch::AddTexture(const char *path, const uint32_t slot, const std::string &textureType) {
        textures.push_back(new Texture(path, slot, textureType.c_str()));
    }

    void Batch::Draw(Shader &shader, Camera &cam, Material &mat) {
        int index;
        shader.Bind();
        for (auto &transform : transforms) {
            transform->Update();

            shader.SetUniformMat4((std::string("transforms[") + std::to_string(index) + "]").c_str(), transform->transform);
            index++;
        }

        mat.Bind(shader);
        cam.Matrix(shader, "camera");
        transforms[0]->Update();
        shader.SetUniformMat4("model", transforms[0]->transform);

        if (cam.EnttComp) {
            auto &camTransform =
                Scene::m_Registry.get<Experimental::Transform>(cam.entity);
            shader.SetUniform3f("cameraPosition", camTransform.position.x,
                                camTransform.position.y,
                                camTransform.position.z);
        } else {
            TransformComponent camTransform =
                cam.GetComponent<TransformComponent>();
            shader.SetUniform3f("cameraPosition", camTransform.position.x,
                                camTransform.position.y,
                                camTransform.position.z);
        }
        shader.SetUniform1i("cubeMap", 20);
        shader.SetUniform1ui("u_EntityID", 0);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(),
                        vertices.data());

        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * indices.size(),
                        indices.data());

        glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(indices.size()),
                       GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        shader.Unbind();
    }
} // namespace HyperAPI