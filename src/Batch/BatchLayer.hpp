#pragma once
#include "../Renderer/Base.hpp"
#include <cstdint>
#include <libs.hpp>
#include "../Renderer/Material.hpp"
#include "../Renderer/Camera.hpp"
#include "../Renderer/Shader.hpp"

#define BATCH_INDEX_AMOUNT 5000

namespace HyperAPI {
    class BatchLayer {
    public:
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;

        uint32_t VBO, IBO, VAO;

        BatchLayer(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
        void Draw(Shader &shader, Camera &camera, Material &material);
    };
} // namespace HyperAPI