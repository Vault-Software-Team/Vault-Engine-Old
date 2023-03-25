#include "Terrain.hpp"
#include "Structures.hpp"
#include "glm/fwd.hpp"
#include "libs.hpp"
#include "scene.hpp"
#include "Mesh.hpp"
#include <cstdint>

namespace HyperAPI {
    // This code is fucking garbage, i made this while i was half asleep stfu
    Terrain::Terrain(int div, float width) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        float triangleSide = width / div;
        for (int row = 0; row < div + 1; row++) {
            for (int col = 0; col < div + 1; col++) {
                Vector3 crnt_vec = Vector3(col * triangleSide, 0.0, row * -triangleSide);
                Vertex vert;
                vert.position = crnt_vec;
                vertices.push_back(vert);
            }
        }

        for (int row = 0; row < div; row++) {
            for (int col = 0; col < div; col++) {
                int index = row * (div + 1) + col;

                indices.push_back(index);
                indices.push_back(index + (div + 1) + 1);
                indices.push_back(index + (div + 1));

                indices.push_back(index);
                indices.push_back(index + 1);
                indices.push_back(index + (div + 1) + 1);
            }
        }

        mesh = new Mesh(vertices, indices, material, false, false);
    }
} // namespace HyperAPI