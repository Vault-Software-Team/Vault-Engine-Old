#pragma once
#include "SpritesheetAnimation.hpp"

namespace HyperAPI::Experimental {
    void c_SpritesheetAnimation::GUI() {
        if (ImGui::CollapsingHeader("Spritesheet Animation")) {
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
                }

                ImGui::TreePop();
            }
            ImGui::ColorEdit4("Color", &mesh->material.baseColor.x);
            DrawVec2Control("Sheet Size", spritesheetSize);
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
            ImGui::InputText("Current Animation", currAnim, 499);

            if (ImGui::TreeNode("Animations")) {
                for (auto &animation : anims) {
                    static const std::string m_id = uuid::generate_uuid_v4();
                    int index = &animation - &anims[0];

                    if (ImGui::TreeNode(m_id.c_str(), "%s", animation.name)) {
                        ImGui::InputText("Name", animation.name, 499);
                        ImGui::DragFloat("Delay", &animation.delay, 0.01f);
                        ImGui::Checkbox("Loop", &animation.loop);

                        if (ImGui::TreeNode("Frames")) {
                            for (auto &frame : animation.frames) {
                                int index = &frame - &animation.frames[0];
                                if (ImGui::TreeNode(
                                        std::to_string(index).c_str())) {
                                    DrawVec2Control("Size", frame.size);
                                    DrawVec2Control("Offset", frame.offset);
                                    ImGui::NewLine();
                                    if (ImGui::Button(ICON_FA_TRASH
                                                      " Remove Frame")) {
                                        animation.frames.erase(
                                            animation.frames.begin() +
                                            index);
                                    }
                                    ImGui::TreePop();
                                }
                            }
                            ImGui::TreePop();
                        }
                        ImGui::NewLine();

                        if (ImGui::Button(ICON_FA_PLUS " New Frame")) {
                            m_SpritesheetAnimationData::Frame frame;
                            animation.frames.push_back(frame);
                        }

                        if (ImGui::Button(ICON_FA_TRASH
                                          " Remove Animation")) {
                            anims.erase(anims.begin() + index);
                        }

                        ImGui::TreePop();
                    }
                }

                ImGui::NewLine();

                if (ImGui::Button(ICON_FA_PLUS " New Animation")) {
                    m_SpritesheetAnimationData anim;
                    anims.push_back(anim);
                }

                ImGui::TreePop();
            }
            if (ImGui::Button("Load XML File")) {
                ImGuiFileDialog::Instance()->OpenDialog(
                    "ChooseXML", "Choose XML File", ".xml", ".");
            }

            if (ImGuiFileDialog::Instance()->Display("ChooseXML")) {
                // action if OK
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    std::string filePathName =
                        ImGuiFileDialog::Instance()->GetFilePathName();
                    // remove cwd from filePathName
                    filePathName.erase(0, cwd.length() + 1);
                    std::string filePath =
                        ImGuiFileDialog::Instance()->GetCurrentPath();

                    anims = GetAnimationsFromXML("", 0.1, spritesheetSize,
                                                 filePathName.c_str());

                    HYPER_LOG(std::string("Loaded XML File: ") +
                              filePathName);
                }

                ImGuiFileDialog::Instance()->Close();
            }

            ImGui::NewLine();
            if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                Scene::m_Registry.remove<c_SpritesheetAnimation>(entity);
            }
        }
    }

} // namespace HyperAPI::Experimental