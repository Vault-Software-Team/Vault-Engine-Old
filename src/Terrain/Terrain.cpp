#include "Terrain.hpp"
#include <stb_image/stb_image.h>
#include <vector>
#include <api.hpp>
#include <libs.hpp>

namespace HyperAPI {
    Terrain::Terrain(const char *height_map, glm::vec4 color) {
        int width, height, nChannels;
        unsigned char *data = stbi_load(height_map, &width, &height, &nChannels, 0);

        std::vector<Vertex> vertices;
        float yScale = 64.0f / 256.0f, yShift = 16.0f; // apply a scale+shift to the height data
        for (uint32_t i = 0; i < height; i++) {
            for (uint32_t j = 0; j < width; j++) {
                // retrieve texel for (i,j) tex coord
                unsigned char *texel = data + (j + width * i) * nChannels;
                // raw height at coordinate
                unsigned char y = texel[0];

                Vertex vertex;

                vertex.position = glm::vec3(-height / 2.0f + i, (int)y * yScale - yShift, -width / 2.0f + j);
                vertex.normal = glm::vec3(0, 1, 0);

                vertex.texUV = glm::vec2(0, 0);
                vertices.push_back(vertex);
            }
        }
        stbi_image_free(data);

        std::vector<uint32_t> indices;
        for (uint32_t i = 0; i < height - 1; i++) // for each row a.k.a. each strip
        {
            for (uint32_t j = 0; j < width; j++) // for each column
            {
                for (uint32_t k = 0; k < 2; k++) // for each side of the strip
                {
                    indices.push_back(j + width * (i + k));
                }
            }
        }
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

        NUM_STRIPS = height - 1;
        NUM_VERTS_PER_STRIP = width * 2;

        glGenVertexArrays(1, &terrainVAO);
        glBindVertexArray(terrainVAO);

        glGenBuffers(1, &terrainVBO);
        glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(vertices), // size of vertices buffer
                     &vertices[0],                       // pointer to first element
                     GL_STATIC_DRAW);

        // position attribute
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

        glGenBuffers(1, &terrainEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size() * sizeof(uint32_t), // size of indices buffer
                     &indices[0],                       // pointer to first element
                     GL_STATIC_DRAW);
    }

    void Terrain::Draw(Shader &shader, Camera &camera, const glm::mat4 &matrix) {
        glBindVertexArray(terrainVAO);
        shader.Bind();
        camera.Matrix(shader, "camera");

        shader.SetUniformMat4("model", matrix);

        static bool unsetted_pl = false;
        static bool unsetted_dl = false;
        static bool unsetted_sl = false;
        static bool unsetted_2dl = false;
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

            unsetted_pl = false;
        }
        if (Scene::PointLights.size() == 0 && !unsetted_pl) {
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
            unsetted_pl = true;
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

            unsetted_sl = false;
        }
        if (Scene::SpotLights.size() == 0 && !unsetted_sl) {
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

            unsetted_sl = true;
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

            unsetted_dl = false;
        }
        if (Scene::DirLights.size() == 0 && !unsetted_dl) {
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

            unsetted_dl = true;
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

            unsetted_2dl = false;
        }
        if (Scene::Lights2D.size() == 0 && !unsetted_2dl) {
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
            unsetted_2dl = true;
        }

        // render the mesh triangle strip by triangle strip - each row at a time
        for (uint32_t strip = 0; strip < NUM_STRIPS; ++strip) {
            glDrawElements(GL_TRIANGLE_STRIP,                                         // primitive type
                           NUM_VERTS_PER_STRIP,                                       // number of indices to render
                           GL_UNSIGNED_INT,                                           // index data type
                           (void *)(sizeof(uint32_t) * NUM_VERTS_PER_STRIP * strip)); // offset to starting index
        }
    }
} // namespace HyperAPI