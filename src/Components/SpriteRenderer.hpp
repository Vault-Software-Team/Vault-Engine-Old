#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Mesh.hpp"
#include "../Renderer/Structures.hpp"
#include "Bloom.hpp"

namespace HyperAPI::Experimental {
    struct SpriteRenderer : public BaseComponent {
        Mesh *mesh;
        bool noComponent = false;

        struct CustomShader {
            bool usingCustomShader = false;
            Shader *shader = nullptr;
        } customShader;

        SpriteRenderer() {
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

            Material material(Vector4(1, 1, 1, 1));
            // new Mesh to shared_ptr
            mesh = new Mesh(vertices, indices, material);
        }
        void DeleteComp() override {
            if (mesh) {
                delete mesh;
            }
        }

        void GUI() override {
            if (ImGui::TreeNode("Sprite Renderer")) {
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
                    Scene::m_Registry.remove<SpriteRenderer>(entity);
                }

                ImGui::TreePop();
            }
        }

        void Update() {
            if (Scene::m_Registry.has<Bloom>(entity)) {
                auto &bloom = Scene::m_Registry.get<Bloom>(entity);
                mesh->material.bloomColor = bloom.bloomColor;
            } else {
                mesh->material.bloomColor = Vector3(0, 0, 0);
            }
        }
    };

    struct m_AnimationData {
        char name[499] = "anim_name";
        std::string id = uuid::generate_uuid_v4();
        std::vector<SpriteRenderer> frames;
        float delay = 0.1f;
        float delay_counter = 0.0f;
        bool loop = false;

        m_AnimationData() = default;
    };
} // namespace HyperAPI::Experimental