#include "BatchLayer.hpp"
#include "../Debugging/GLError.hpp"
#include "../Components/Transform.hpp"
#include "glm/fwd.hpp"
#include <cstdint>

namespace HyperAPI {
    BatchLayer::BatchLayer(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices) {
        m_vertices = vertices;
        m_indices = indices;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &IBO);
        glBindVertexArray(VAO);
        glCheckError();

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
                     nullptr, GL_DYNAMIC_DRAW);
        glCheckError();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     BATCH_INDEX_AMOUNT * sizeof(uint32_t), nullptr,
                     GL_DYNAMIC_DRAW);
        glCheckError();

        // coords
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);

        // color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));
        glEnableVertexAttribArray(1);

        // normals
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);

        // texuv
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texUV));
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 4, GL_INT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_BoneIDs));
        glEnableVertexAttribArray(4);

        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_Weights));
        glEnableVertexAttribArray(5);

        glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(6);

        glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));
        glEnableVertexAttribArray(7);

        glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, transformIndex));
        glEnableVertexAttribArray(8);
    }

    void BatchLayer::Draw(Shader &shader, Camera &camera, Material &material) {
        material.Bind(shader);
        camera.Matrix(shader, "camera");

        Experimental::Transform transform;
        transform.position = glm::vec3(0, 0, 0);
        transform.rotation = glm::vec3(0, 0, 0);
        transform.scale = glm::vec3(1, 1, 1);
        transform.Update();

        shader.SetUniformMat4("model", transform.transform);

        glm::mat4 trans = glm::mat4(1);
        glm::mat4 rot = glm::mat4(1);
        glm::mat4 sca = glm::mat4(1);

        shader.SetUniformMat4("translation", trans);
        shader.SetUniformMat4("rotation", rot);
        shader.SetUniformMat4("scale", sca);

        for (int i = 0; i < Scene::PointLights.size(); i++) {
            shader.SetUniform3f(
                ("pointLights[" + std::to_string(i) + "].lightPos").c_str(),
                Scene::PointLights[i]->lightPos.x,
                Scene::PointLights[i]->lightPos.y,
                Scene::PointLights[i]->lightPos.z);
            shader.SetUniform3f(
                ("pointLights[" + std::to_string(i) + "].color").c_str(),
                Scene::PointLights[i]->color.x, Scene::PointLights[i]->color.y,
                Scene::PointLights[i]->color.z);
            shader.SetUniform1f(
                ("pointLights[" + std::to_string(i) + "].intensity").c_str(),
                Scene::PointLights[i]->intensity);
        }
        if (Scene::PointLights.size() == 0) {
            for (int i = 0; i < 100; i++) {
                shader.SetUniform3f(
                    ("pointLights[" + std::to_string(i) + "].lightPos").c_str(),
                    0, 0, 0);
                shader.SetUniform3f(
                    ("pointLights[" + std::to_string(i) + "].color").c_str(), 0,
                    0, 0);
                shader.SetUniform1f(
                    ("pointLights[" + std::to_string(i) + "].intensity")
                        .c_str(),
                    0);
            }
        }

        for (int i = 0; i < Scene::SpotLights.size(); i++) {
            // Scene::SpotLights[i]->scriptComponent.OnUpdate();
            shader.SetUniform3f(
                ("spotLights[" + std::to_string(i) + "].lightPos").c_str(),
                Scene::SpotLights[i]->lightPos.x,
                Scene::SpotLights[i]->lightPos.y,
                Scene::SpotLights[i]->lightPos.z);
            shader.SetUniform3f(
                ("spotLights[" + std::to_string(i) + "].color").c_str(),
                Scene::SpotLights[i]->color.x, Scene::SpotLights[i]->color.y,
                Scene::SpotLights[i]->color.z);
            // shader.SetUniform1f(("spotLights[" + std::to_string(i) +
            // "].outerCone").c_str(), Scene::SpotLights[i]->outerCone);
            // shader.SetUniform1f(("spotLights[" + std::to_string(i) +
            // "].innerCone").c_str(), Scene::SpotLights[i]->innerCone);
            shader.SetUniform3f(
                ("spotLights[" + std::to_string(i) + "].angle").c_str(),
                Scene::SpotLights[i]->angle.x, Scene::SpotLights[i]->angle.y,
                Scene::SpotLights[i]->angle.z);
        }
        if (Scene::SpotLights.size() == 0) {
            for (int i = 0; i < 100; i++) {
                shader.SetUniform3f(
                    ("spotLights[" + std::to_string(i) + "].lightPos").c_str(),
                    0, 0, 0);
                shader.SetUniform3f(
                    ("spotLights[" + std::to_string(i) + "].color").c_str(), 0,
                    0, 0);
                shader.SetUniform1f(
                    ("spotLights[" + std::to_string(i) + "].outerCone").c_str(),
                    0);
                shader.SetUniform1f(
                    ("spotLights[" + std::to_string(i) + "].innerCone").c_str(),
                    0);
            }
        }

        for (int i = 0; i < Scene::DirLights.size(); i++) {
            // Scene::DirLights[i]->scriptComponent.OnUpdate();
            shader.SetUniform3f(
                ("dirLights[" + std::to_string(i) + "].lightPos").c_str(),
                Scene::DirLights[i]->lightPos.x,
                Scene::DirLights[i]->lightPos.y,
                Scene::DirLights[i]->lightPos.z);
            shader.SetUniform3f(
                ("dirLights[" + std::to_string(i) + "].color").c_str(),
                Scene::DirLights[i]->color.x, Scene::DirLights[i]->color.y,
                Scene::DirLights[i]->color.z);
            shader.SetUniform1f(
                ("dirLights[" + std::to_string(i) + "].intensity").c_str(),
                Scene::DirLights[i]->intensity);
        }
        if (Scene::DirLights.size() == 0) {
            for (int i = 0; i < 100; i++) {
                shader.SetUniform3f(
                    ("dirLights[" + std::to_string(i) + "].lightPos").c_str(),
                    0, 0, 0);
                shader.SetUniform3f(
                    ("dirLights[" + std::to_string(i) + "].color").c_str(), 0,
                    0, 0);
                shader.SetUniform1f(
                    ("dirLights[" + std::to_string(i) + "].intensity").c_str(),
                    0);
            }
        }

        for (int i = 0; i < Scene::Lights2D.size(); i++) {
            shader.SetUniform2f(
                ("light2ds[" + std::to_string(i) + "].lightPos").c_str(),
                Scene::Lights2D[i]->lightPos.x, Scene::Lights2D[i]->lightPos.y);
            shader.SetUniform3f(
                ("light2ds[" + std::to_string(i) + "].color").c_str(),
                Scene::Lights2D[i]->color.x, Scene::Lights2D[i]->color.y,
                Scene::Lights2D[i]->color.z);
            shader.SetUniform1f(
                ("light2ds[" + std::to_string(i) + "].range").c_str(),
                Scene::Lights2D[i]->range);
        }
        if (Scene::Lights2D.size() == 0) {
            for (int i = 0; i < 100; i++) {
                shader.SetUniform2f(
                    ("light2ds[" + std::to_string(i) + "].lightPos").c_str(), 0,
                    0);
                shader.SetUniform3f(
                    ("light2ds[" + std::to_string(i) + "].color").c_str(), 0, 0,
                    0);
                shader.SetUniform1f(
                    ("light2ds[" + std::to_string(i) + "].range").c_str(), 0);
            }
        }

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_vertices.size(),
                        m_vertices.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * BATCH_INDEX_AMOUNT, m_indices.data());

        glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(m_indices.size()),
                       GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        material.Unbind(shader);
    }
} // namespace HyperAPI