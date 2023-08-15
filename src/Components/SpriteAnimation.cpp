#pragma once
#include "SpriteAnimation.hpp"

namespace HyperAPI::Experimental {
    void SpriteAnimation::GUI() {
        if (ImGui::CollapsingHeader("Sprite Animation")) {
            ImGui::InputText("Current Animation", currAnim, 499);

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

            for (auto &animation : anims) {
                static const std::string m_id = uuid::generate_uuid_v4();
                std::cout << m_id << std::endl;

                int index = &animation - &anims[0];

                if (ImGui::TreeNode(m_id.c_str(), "%s", std::string(animation.name + std::string(" ") + std::to_string(index)).c_str())) {
                    ImGui::InputText("Name", animation.name, 499);
                    ImGui::InputFloat("Delay", &animation.delay);
                    ImGui::Checkbox("Loop", &animation.loop);

                    for (auto &frame : animation.frames) {
                        int index = &frame - &animation.frames[0];

                        if (ImGui::TreeNode(
                                std::string("Frame " +
                                            std::to_string(index))
                                    .c_str())) {
                            if (frame.mesh->material.diffuse != nullptr) {
                                ImGui::ImageButton(
                                    (void *)
                                        frame.mesh->material.diffuse->tex->ID,
                                    ImVec2(128, 128), ImVec2(0, 1),
                                    ImVec2(1, 0));
                            } else {
                                ImGui::ImageButton((void *)0,
                                                   ImVec2(128, 128));
                            }
                            Scene::DropTargetMat(Scene::DRAG_SPRITE,
                                                 frame.mesh, nullptr);
                            if (ImGui::IsItemHovered() &&
                                ImGui::IsMouseClicked(1) &&
                                frame.mesh->material.diffuse != nullptr) {
                                delete frame.mesh->material.diffuse;
                            }
                            ImGui::ColorEdit4(
                                "Color", &frame.mesh->material.baseColor.x);

                            if (ImGui::Button(ICON_FA_TRASH
                                              " Remove Frame")) {
                                animation.frames.erase(
                                    animation.frames.begin() + index);
                            }

                            ImGui::TreePop();
                        }
                    }

                    if (ImGui::Button(ICON_FA_PLUS " Add Frame")) {
                        animation.frames.push_back(SpriteRenderer());
                    }

                    ImGui::NewLine();
                    if (ImGui::Button(ICON_FA_TRASH " Remove Animation")) {
                        anims.erase(anims.begin() + index);
                    }
                    ImGui::TreePop();
                }
            }

            if (ImGui::Button(ICON_FA_PLUS " Add Animation")) {
                m_AnimationData anim;
                anims.push_back(anim);
            }

            ImGui::NewLine();
            if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                Scene::m_Registry.remove<SpriteAnimation>(entity);
            }
        }
    }
} // namespace HyperAPI::Experimental