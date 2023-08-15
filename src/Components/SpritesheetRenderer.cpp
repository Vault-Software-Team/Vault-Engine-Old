#pragma once
#include "SpritesheetRenderer.hpp"
namespace HyperAPI::Experimental {
    void SpritesheetRenderer::GUI() {
        if (ImGui::CollapsingHeader("Spritesheet Renderer")) {
            if (ImGui::TreeNode("Texture")) {
                if (mesh->material.diffuse != nullptr) {
                    ImGui::ImageButton((void *)mesh->material.diffuse->tex->ID,
                                       ImVec2(128, 128), ImVec2(0, 1),
                                       ImVec2(1, 0));
                } else {
                    ImGui::ImageButton((void *)0, ImVec2(128, 128));
                }
                Scene::DropTargetMat(Scene::DRAG_SPRITE, mesh, nullptr);
                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1) &&
                    mesh->material.diffuse != nullptr) {
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
        }
    }
} // namespace HyperAPI::Experimental