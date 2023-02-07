#include "Terrain.hpp"
#include "glm/fwd.hpp"
#include "libs.hpp"
#include "scene.hpp"
#include "Mesh.hpp"

namespace HyperAPI {
    // This code is fucking garbage, i made this while i was half asleep stfu
    Terrain::Terrain() {
        float amount = 5, amountY = 5;
        float offset_x = 0;
        float offset_y;
        int ind = 0;
        uint32_t index_offset = 0;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        for (int i = 0; i < amount * amountY; i++) {
            Vector3 pos1(0.0 + offset_x, 0.1 + offset_y, 1);
            Vector3 pos2(0.0 + offset_x, 0.0 + offset_y, 0);
            Vector3 pos3(0.1 + offset_x, 0.0 + offset_y, 0);
            Vector3 pos4(0.1 + offset_x, 0.1 + offset_y, 0);
            offset_x += 0.1;

            uint32_t tri1[3] = {0 + index_offset, 1 + index_offset, 2 + index_offset};
            uint32_t tri2[3] = {0 + index_offset, 3 + index_offset, 2 + index_offset};

            ind++;
            if (ind >= 5) {
                offset_y += 0.1;
                offset_x = 0;
                ind = 0;
            }

            Vertex vertex1 = {
                pos1,
                glm::vec3(0, 0, 0),
                glm::vec3(0, 1, 0),
                glm::vec2(0, 0)};
            Vertex vertex2 = {
                pos2,
                glm::vec3(0, 0, 0),
                glm::vec3(0, 1, 0),
                glm::vec2(0, 0)};
            Vertex vertex3 = {
                pos3,
                glm::vec3(0, 0, 0),
                glm::vec3(0, 1, 0),
                glm::vec2(0, 0)};
            Vertex vertex4 = {
                pos4,
                glm::vec3(0, 0, 0),
                glm::vec3(0, 1, 0),
                glm::vec2(0, 0)};

            vertices.push_back(vertex1);
            vertices.push_back(vertex2);
            vertices.push_back(vertex3);
            vertices.push_back(vertex4);

            indices.push_back(0 + index_offset);
            indices.push_back(1 + index_offset);
            indices.push_back(2 + index_offset);

            indices.push_back(0 + index_offset);
            indices.push_back(3 + index_offset);
            indices.push_back(2 + index_offset);

            index_offset += 4;
        }

        mesh = new Mesh(vertices, indices, material, false, false);
    }
} // namespace HyperAPI