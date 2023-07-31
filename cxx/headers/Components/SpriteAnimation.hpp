#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"
#include "../Renderer/Timestep.hpp"

#include "SpriteRenderer.hpp"
#include "GameObject.hpp"

namespace HyperAPI::Experimental {
    struct SpriteAnimation : public BaseComponent {
        Mesh *currMesh;
        std::vector<m_AnimationData> anims;
        char currAnim[499] = "";

        struct CustomShader {
            bool usingCustomShader = false;
            Shader *shader = nullptr;
        } customShader;

        SpriteAnimation() {
            currMesh = nullptr;
            for (auto &gameObject : (*Scene::m_GameObjects)) {
                if (gameObject->ID == ID) {
                    std::map<std::string, int> m_CurrFrames;
                    Scene::currFrames[gameObject->ID] = m_CurrFrames;

                    std::map<std::string, float> m_CurrDelays;
                    Scene::currDelays[gameObject->ID] = m_CurrDelays;
                }
            }
        }

        void DeleteComp() override {
            for (auto &gameObject : (*Scene::m_GameObjects)) {
                if (gameObject->ID == ID) {
                    Scene::currFrames.erase(gameObject->ID);
                    Scene::currDelays.erase(gameObject->ID);
                }
            }

            for (auto &anim : anims) {
                for (auto &frame : anim.frames) {
                    if (frame.mesh)
                        delete frame.mesh;
                }
                anim.frames.clear();
            }
        }

        void GUI() override {
            if (ImGui::TreeNode("Sprite Animation")) {
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

                ImGui::TreePop();
            }
        }

        void Play() {
            for (auto &animation : anims) {
                if (std::string(animation.name) == std::string(currAnim)) {
                    Scene::currDelays[ID][std::string(animation.name)] +=
                        Timestep::deltaTime;
                    if (Scene::currDelays[ID][std::string(animation.name)] >=
                        animation.delay) {
                        Scene::currFrames[ID][std::string(animation.name)] += 1;
                        if (Scene::currFrames[ID][std::string(
                                animation.name)] >= animation.frames.size()) {
                            if (animation.loop) {
                                Scene::currFrames[ID]
                                                 [std::string(animation.name)] =
                                                     0;
                            } else {
                                Scene::currFrames[ID]
                                                 [std::string(animation.name)] =
                                                     animation.frames.size() -
                                                     1;
                            }
                        }
                        Scene::currDelays[ID][std::string(animation.name)] =
                            0.0f;
                    }
                    currMesh = animation
                                   .frames[Scene::currFrames[ID][std::string(
                                       animation.name)]]
                                   .mesh;

                    break;
                }
            }
        }
    };
} // namespace HyperAPI::Experimental