#pragma once
#include <libs.hpp>
#include "Material.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Structures.hpp"

namespace HyperAPI {
    class Mesh : public ComponentSystem {
    public:
        std::string parentType = "None";
        Material material{Vector4(1, 1, 1, 1)};

        uint32_t VBO, VAO, IBO;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<Texture> textures;
        glm::mat4 model = glm::mat4(1.0f);

        Vector4 Color;
        bool physics = true;
        bool empty = false;
        bool modelMesh = false;
        bool batched;

        bool hasMaterial = true;

        Mesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices,
             Material &material, bool empty = false, bool batched = false);

        ~Mesh() {
            vertices.clear();
            indices.clear();
            textures.clear();

            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &IBO);
            glDeleteVertexArrays(1, &VAO);
        }

        uint32_t enttId;

        void Draw(Shader &shader, Camera &camera,
                  glm::mat4 matrix = glm::mat4(1.0f),
                  glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                  glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
    };
} // namespace HyperAPI