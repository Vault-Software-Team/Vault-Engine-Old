#pragma once
#include "Base.hpp"
#include "Structures.hpp"
#include "Texture.hpp"
#include "scene.hpp"
#include "Mesh.hpp"
#include <cstdint>
#include <libs.hpp>

#include "../Components/Transform.hpp"

namespace HyperAPI {
    class Batch {
    public:
        uint32_t VBO, VAO, IBO;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        std::vector<Texture *> textures;
        std::vector<Experimental::Transform *> transforms;

        Batch();

        void AddTexture(const char *path, const uint32_t slot, const std::string &textureType);
        void AddMesh(Mesh &mesh, Experimental::Transform *transform);
        void Draw(Shader &shader);
    };
} // namespace HyperAPI