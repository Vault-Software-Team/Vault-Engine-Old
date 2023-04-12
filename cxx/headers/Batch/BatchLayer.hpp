#pragma once
#include "../Renderer/Base.hpp"
#include <cstdint>
#include <libs.hpp>
#include <memory>
#include "../Renderer/Material.hpp"
#include "../Renderer/Camera.hpp"
#include "../Renderer/Shader.hpp"
#include "../Components/GameObject.hpp"

#define BATCH_INDEX_AMOUNT 5000

namespace HyperAPI {
    // fuck batch rendering literally omg i hate DOING THIS THIS IS FUCKING TORTURE WHY CANT SHIT BE EASY
    class BatchLayer {
    public:
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;
        std::vector<Experimental::Transform *> transforms;

        uint32_t VBO, IBO, VAO;

        BatchLayer(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
        void AddToBatch(Mesh *mesh, Experimental::Transform *obj);
        void Draw(Shader &shader, Camera &camera, Material &material);

        // for some retarded reason the batch renderer wont draw it until a mesh is drawn first
        static inline std::vector<BatchLayer *> layers;
    };
} // namespace HyperAPI