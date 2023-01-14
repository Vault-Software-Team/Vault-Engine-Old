#include "OldStuff.hpp"
#include "../Components/Transform.hpp"
#include "../Renderer/Camera.hpp"

namespace HyperAPI {
    Sprite::Sprite(const char *texPath) {
        // square vertex
        std::vector<Vertex> vertices = {
            Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1, 1, 1),
                   glm::vec3(0, 1, 0), glm::vec2(0.0f, 0.0f)},
            Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1, 1, 1),
                   glm::vec3(0, 1, 0), glm::vec2(1.0f, 0.0f)},
            Vertex{glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1, 1, 1),
                   glm::vec3(0, 1, 0), glm::vec2(1.0f, 1.0f)},
            Vertex{glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1, 1, 1),
                   glm::vec3(0, 1, 0), glm::vec2(0.0f, 1.0f)}};

        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

        Material material(Vector4(1, 1, 1, 1),
                          {Texture(texPath, 0, "texture_diffuse")});
        m_Mesh = new Mesh(vertices, indices, material);
    }

    void Sprite::Draw(Shader &shader, Camera &camera, glm::mat4 trans) {
        m_Mesh->Draw(shader, camera, trans);
    }

    Spritesheet::Spritesheet(const char *texPath, Material &mat,
                             Vector2 sheetSize, Vector2 spriteSize,
                             Vector2 spriteCoords) {
        // square vertex
        float xCoord = spriteCoords.x + spriteSize.x;
        float yCoord =
            (sheetSize.y - (spriteCoords.y + spriteSize.y)) + spriteSize.y;
        std::vector<Vector2> texCoords = {
            Vector2(spriteCoords.x / sheetSize.x,
                    (sheetSize.y - (spriteCoords.y + spriteSize.y)) /
                        sheetSize.y),
            Vector2(xCoord / sheetSize.x,
                    (sheetSize.y - (spriteCoords.y + spriteSize.y)) /
                        sheetSize.y),
            Vector2(xCoord / sheetSize.x, yCoord / sheetSize.y),
            Vector2(spriteCoords.x / sheetSize.x, yCoord / sheetSize.y)};

        std::vector<Vertex> vertices = {
            Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1, 1, 1),
                   glm::vec3(0, 1, 0), texCoords[0]},

            Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1, 1, 1),
                   glm::vec3(0, 1, 0), texCoords[1]},

            Vertex{glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1, 1, 1),
                   glm::vec3(0, 1, 0), texCoords[2]},

            Vertex{glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1, 1, 1),
                   glm::vec3(0, 1, 0), texCoords[3]}};

        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

        // Material material(Vector4(1,1,1,1), { Texture(texPath, 0,
        // "texture_diffuse") });
        m_Mesh = new Mesh(vertices, indices, mat);
    }

    void Spritesheet::Draw(Shader &shader, Camera &camera) {
        m_Mesh->Draw(shader, camera);
    }

    Graphic::Graphic(Vector3 rgb) {
        std::vector<HyperAPI::Vertex> vertices = {
            {glm::vec3(-0.5, -0.5, 0), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0),
             glm::vec2(0, 0)},
            {glm::vec3(-0.5, 0.5, 0), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0),
             glm::vec2(0, 1)},
            {glm::vec3(0.5, 0.5, 0), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0),
             glm::vec2(1, 1)},
            {glm::vec3(0.5, -0.5, 0), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0),
             glm::vec2(1, 0)},
        };

        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

        Material material(Vector4(rgb, 1));
        m_Mesh = new Mesh(vertices, indices, material);
    }

    void Graphic::Draw(Shader &shader, Camera &camera) {
        m_Mesh->Draw(shader, camera);
    }
} // namespace HyperAPI