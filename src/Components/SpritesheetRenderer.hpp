#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Mesh.hpp"
#include "../Renderer/Structures.hpp"
#include "Bloom.hpp"
#include "../Renderer/OldStuff.hpp"

namespace HyperAPI::Experimental {
    struct DLL_API SpritesheetRenderer : public BaseComponent {
        Mesh *mesh;
        Spritesheet *sp = nullptr;
        Vector2 spritesheetSize = Vector2(512, 512);
        Vector2 spriteSize = Vector2(32, 32);
        Vector2 spriteOffset = Vector2(0, 0);
        Material material{Vector4(1, 1, 1, 1)};

        struct DLL_API CustomShader {
            bool usingCustomShader = false;
            Shader *shader = nullptr;
        } customShader;

        SpritesheetRenderer() {
            Spritesheet sp("", material, spritesheetSize, spriteSize,
                           spriteOffset);
            mesh = std::move(sp.m_Mesh);
        }

        void DeleteComp() override {
            if (mesh) {
                delete mesh;
            }
        }

        void GUI() override;

        void Update() {
            if (mesh == nullptr)
                return;

            if (Scene::m_Registry.has<Bloom>(entity)) {
                auto &bloom = Scene::m_Registry.get<Bloom>(entity);
                mesh->material.bloomColor = bloom.bloomColor;
                mesh->material.bloom_threshold = bloom.bloom_threshold;
                mesh->material.dynamic_bloom = bloom.dynamic_bloom;
            } else {
                mesh->material.bloomColor = Vector3(0, 0, 0);
                mesh->material.bloom_threshold = 0;
                mesh->material.dynamic_bloom = false;
            }

            bool trulyChanged = false;
            for (auto &vertex : mesh->vertices) {
                int index = &vertex - &mesh->vertices[0];
                float xCoord = spriteOffset.x + spriteSize.x;
                float yCoord =
                    (spritesheetSize.y - (spriteOffset.y + spriteSize.y)) +
                    spriteSize.y;

                std::vector<Vector2> texCoords = {
                    Vector2(
                        spriteOffset.x / spritesheetSize.x,
                        (spritesheetSize.y - (spriteOffset.y + spriteSize.y)) /
                            spritesheetSize.y),
                    Vector2(
                        xCoord / spritesheetSize.x,
                        (spritesheetSize.y - (spriteOffset.y + spriteSize.y)) /
                            spritesheetSize.y),
                    Vector2(xCoord / spritesheetSize.x,
                            yCoord / spritesheetSize.y),
                    Vector2(spriteOffset.x / spritesheetSize.x,
                            yCoord / spritesheetSize.y)};

                trulyChanged = !(vertex.texUV.x == texCoords[index].x && vertex.texUV.y == texCoords[index].y);

                vertex.texUV = texCoords[index];
            }

            if (trulyChanged) {
                glBindVertexArray(mesh->VAO);
                glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mesh->vertices.size(),
                                mesh->vertices.data());
            }
        }
    };
} // namespace HyperAPI::Experimental