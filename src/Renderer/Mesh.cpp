#include "Mesh.hpp"
#include "../Components/Transform.hpp"
#include "../Renderer/Camera.hpp"
#include "../Debugging/GLError.hpp"

namespace HyperAPI {
    Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices,
               Material &material, bool empty, bool batched) {

        TransformComponent component;
        component.position = Vector3(0, 0, 0);
        component.scale = Vector3(1, 1, 1);
        this->Components.push_back(component);

        this->vertices = vertices;
        this->indices = indices;
        this->material = material;
        this->ID = uuid::generate_uuid_v4();
        this->empty = empty;

        // calculate TBN
        for (int i = 0; i < indices.size(); i += 3) {
            Vertex &v1 = vertices[indices[i]];
            Vertex &v2 = vertices[indices[i + 1]];
            Vertex &v3 = vertices[indices[i + 2]];
            if (v1.tangent.x != -435.0f && v2.tangent.x != -435.0f &&
                v3.tangent.x != -435.0f)
                continue;

            glm::vec3 edge1 = v2.position - v1.position;
            glm::vec3 edge2 = v3.position - v1.position;
            glm::vec2 deltaUV1 = v2.texUV - v1.texUV;
            glm::vec2 deltaUV2 = v3.texUV - v1.texUV;

            float f =
                1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            glm::vec3 tangent;
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

            glm::vec3 bitangent;
            bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

            v1.tangent = tangent;
            v2.tangent = tangent;
            v3.tangent = tangent;

            v1.bitangent = bitangent;
            v2.bitangent = bitangent;
            v3.bitangent = bitangent;
        }

        if (empty) {
            return;
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &IBO);
        glBindVertexArray(VAO);
        glCheckError();

        if (!batched) {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                         nullptr, GL_DYNAMIC_DRAW);
            glCheckError();

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         indices.size() * sizeof(uint32_t), indices.data(),
                         GL_STATIC_DRAW);
            glCheckError();

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

            // glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            // (void*)(11 * sizeof(float))); glEnableVertexAttribArray(4);

            // // add mat4
            // glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            // (void*)(12 * sizeof(float))); glEnableVertexAttribArray(5);
            // glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            // (void*)(16 * sizeof(float))); glEnableVertexAttribArray(6);
            // glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            // (void*)(20 * sizeof(float))); glEnableVertexAttribArray(7);
            // glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            // (void*)(24 * sizeof(float))); glEnableVertexAttribArray(8);
            // //divisors
            // glVertexAttribDivisor(5, 1);
            // glVertexAttribDivisor(6, 1);
            // glVertexAttribDivisor(7, 1);
            // glVertexAttribDivisor(8, 1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                         nullptr, GL_DYNAMIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         indices.size() * sizeof(uint32_t), indices.data(),
                         GL_STATIC_DRAW);

            // coords
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void *)0);
            glEnableVertexAttribArray(0);

            // color
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void *)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            // normals
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void *)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);

            // texuv
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void *)(9 * sizeof(float)));
            glEnableVertexAttribArray(3);

            glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void *)(11 * sizeof(float)));
            glEnableVertexAttribArray(4);

            // add mat4
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void *)(12 * sizeof(float)));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void *)(16 * sizeof(float)));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void *)(20 * sizeof(float)));
            glEnableVertexAttribArray(7);
            glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                  (void *)(24 * sizeof(float)));
            glEnableVertexAttribArray(8);
            // divisors
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);
            glVertexAttribDivisor(7, 1);
            glVertexAttribDivisor(8, 1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        // scriptComponent.componentSystem = this;
    }

    void Mesh::Draw(Shader &shader, Camera &camera, glm::mat4 matrix,
                    glm::vec3 translation, glm::quat rotation,
                    glm::vec3 scale) {
        material.Bind(shader);
        camera.Matrix(shader, "camera");

        if (camera.EnttComp) {
            auto &cameraTransform =
                Scene::m_Registry.get<Experimental::Transform>(camera.entity);
            shader.SetUniform3f("cameraPosition", cameraTransform.position.x,
                                cameraTransform.position.y,
                                cameraTransform.position.z);
        } else {
            TransformComponent cameraTransform =
                camera.GetComponent<TransformComponent>();
            shader.SetUniform3f("cameraPosition", cameraTransform.position.x,
                                cameraTransform.position.y,
                                cameraTransform.position.z);
        }
        shader.SetUniform1i("cubeMap", 20);
        shader.SetUniform1ui("u_EntityID", enttId);

        // scriptComponent.OnUpdate();

        TransformComponent component;
        component.scale = Vector3(1);

        model = glm::translate(glm::mat4(1.0f), component.position) *
                glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.x),
                            glm::vec3(1.0f, 0.0f, 0.0f)) *
                glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.y),
                            glm::vec3(0.0f, 1.0f, 0.0f)) *
                glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.z),
                            glm::vec3(0.0f, 0.0f, 1.0f)) *
                glm::scale(glm::mat4(1.0f), Vector3(component.scale.x * 0.5,
                                                    component.scale.y * 0.5,
                                                    component.scale.z * 0.5));

        model = matrix * model;

        glm::mat4 trans = glm::mat4(1);
        glm::mat4 rot = glm::mat4(1);
        glm::mat4 sca = glm::mat4(1);

        component.transform =
            glm::translate(glm::mat4(1.0f), component.position) *
            glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.x),
                        glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.y),
                        glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(component.rotation.z),
                        glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::scale(glm::mat4(1.0f), component.scale) * matrix;

        model = component.transform;
        trans = glm::translate(trans, translation);
        rot = glm::mat4_cast(rotation);
        sca = glm::scale(sca, scale);

        shader.SetUniformMat4("model", model);
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
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(),
                        vertices.data());
        glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(indices.size()),
                       GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        material.Unbind(shader);
    }
} // namespace HyperAPI