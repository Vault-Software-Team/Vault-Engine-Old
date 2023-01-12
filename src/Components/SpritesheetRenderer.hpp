#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Mesh.hpp"
#include "../Renderer/Structures.hpp"
#include "Bloom.hpp"
#include "../Renderer/OldStuff.hpp"

namespace HyperAPI::Experimental {
    struct SpritesheetRenderer : public BaseComponent {
        Mesh *mesh;
        Spritesheet *sp = nullptr;
        Vector2 spritesheetSize = Vector2(512, 512);
        Vector2 spriteSize = Vector2(32, 32);
        Vector2 spriteOffset = Vector2(0, 0);
        Material material{Vector4(1, 1, 1, 1)};

        struct CustomShader {
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

        void GUI() override {
            if (ImGui::TreeNode("Spritesheet Renderer")) {
                if (ImGui::TreeNode("Texture")) {
                    if (mesh->material.diffuse != nullptr) {
                        ImGui::ImageButton((void *)mesh->material.diffuse->ID,
                                           ImVec2(128, 128), ImVec2(0, 1),
                                           ImVec2(1, 0));
                    } else {
                        ImGui::ImageButton((void *)0, ImVec2(128, 128));
                    }
                    Scene::DropTargetMat(Scene::DRAG_SPRITE, mesh, nullptr);
                    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1) &&
                        mesh->material.diffuse != nullptr) {
                        glDeleteTextures(1, &mesh->material.diffuse->ID);
                        delete mesh->material.diffuse;
                        mesh->material.diffuse = nullptr;
                    }

                    ImGui::TreePop();
                }
                ImGui::ColorEdit4("Color", &mesh->material.baseColor.x);
                DrawVec2Control("Sheet Size", spritesheetSize);
                DrawVec2Control("Sprite Size", spriteSize);
                DrawVec2Control("Sprite Offset", spriteOffset);

                ImGui::NewLine();

                if (!customShader.usingCustomShader) {
                    ImGui::Button("Drag Shader Here");
                } else {
                    // set std::experimental::filesystem as fs
                    namespace fs = std::experimental::filesystem;
                    fs::path path = customShader.shader->path;

                    if (ImGui::Button(
                            std::string(
                                std::string("Click to remove shader: ") +
                                path.filename().string())
                                .c_str())) {
                        customShader.usingCustomShader = false;
                        delete customShader.shader;
                        customShader.shader = nullptr;
                    }
                }
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload *payload =
                            ImGui::AcceptDragDropPayload("file")) {
                        if (G_END_WITH(dirPayloadData, ".glsl")) {
                            customShader.usingCustomShader = true;
                            customShader.shader =
                                new Shader(dirPayloadData.c_str());
                        }
                    }
                }

                ImGui::NewLine();
                if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                    Scene::m_Registry.remove<SpritesheetRenderer>(entity);
                }

                ImGui::TreePop();
            }
        }

        void Update() {
            if (mesh == nullptr)
                return;

            if (Scene::m_Registry.has<Bloom>(entity)) {
                auto &bloom = Scene::m_Registry.get<Bloom>(entity);
                mesh->material.bloomColor = bloom.bloomColor;
            } else {
                mesh->material.bloomColor = Vector3(0, 0, 0);
            }

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
                vertex.texUV = texCoords[index];
            }
        }
    };
} // namespace HyperAPI::Experimental