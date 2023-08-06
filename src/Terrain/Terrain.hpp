#pragma once
#include <glm/ext.hpp>
#include "../Renderer/Shader.hpp"
#include "../Renderer/Material.hpp"

namespace HyperAPI {
    class Terrain {
    private:
        uint32_t NUM_STRIPS, NUM_VERTS_PER_STRIP;
        Material material;

    public:
        uint32_t terrainVAO, terrainVBO, terrainEBO;

        Terrain(const char *height_map, glm::vec4 color = glm::vec4(1, 1, 1, 1));
        void Draw(Shader &shader, Camera &camera, const glm::mat4 &matrix);
    };
} // namespace HyperAPI