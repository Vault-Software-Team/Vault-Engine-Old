#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"
#include "../Renderer/Timestep.hpp"

#include "Bloom.hpp"

namespace HyperAPI::Experimental {
    struct m_SpritesheetAnimationData {
        struct Frame {
            Vector2 size = Vector2(0.0f, 0.0f);
            Vector2 offset = Vector2(0.0f, 0.0f);
        };

        char name[499] = "anim_name";
        std::string id = uuid::generate_uuid_v4();
        float delay = 0.1f;
        float delay_counter = 0.0f;

        std::vector<Frame> frames;

        bool loop = false;
    };

    std::vector<m_SpritesheetAnimationData>
    GetAnimationsFromXML(const char *texPath, float delay, Vector2 sheetSize,
                         const std::string &xmlFile);

    struct c_SpritesheetAnimation : public BaseComponent {
        Material mat{Vector4(1, 1, 1, 1)};
        Spritesheet sheet{"", mat, Vector2(0, 0), Vector2(0, 0), Vector2(0, 0)};
        Mesh *mesh;
        char currAnim[499] = "";
        std::vector<m_SpritesheetAnimationData> anims;
        Vector2 spritesheetSize;
        m_SpritesheetAnimationData::Frame currFrame;

        struct CustomShader {
            bool usingCustomShader = false;
            Shader *shader = nullptr;
        } customShader;

        c_SpritesheetAnimation() { mesh = sheet.m_Mesh; }

        void DeleteComp() override {
            for (auto &anim : anims) {
                anim.frames.clear();
            }
            anims.clear();

            if (mesh) {
                if (mesh)
                    delete mesh;
            }
        }

        void GUI() override {
            if (ImGui::TreeNode("Spritesheet Animation")) {
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
                        int index = &animation - &anims[0];

                        if (ImGui::TreeNode(animation.name)) {
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
                        currFrame =
                            animation.frames[Scene::currFrames[ID][std::string(
                                animation.name)]];
                        Scene::currDelays[ID][std::string(animation.name)] =
                            0.0f;
                    }
                } else if (!animation.loop) {
                    Scene::currFrames[ID][std::string(animation.name)] = 0;
                }
            }
        }

        void Update() {
            if (Scene::m_Registry.has<Bloom>(entity)) {
                auto &bloom = Scene::m_Registry.get<Bloom>(entity);
                mesh->material.bloomColor = bloom.bloomColor;
            } else {
                mesh->material.bloomColor = Vector3(0, 0, 0);
            }

            for (auto &vertex : mesh->vertices) {
                m_SpritesheetAnimationData::Frame currFrame;
                for (auto &animation : anims) {
                    if (std::string(animation.name) == std::string(currAnim)) {
                        currFrame =
                            animation.frames[Scene::currFrames[ID][std::string(
                                animation.name)]];
                        break;
                    }
                }
                int index = &vertex - &mesh->vertices[0];
                float xCoord = currFrame.offset.x + currFrame.size.x;
                float yCoord = (spritesheetSize.y -
                                (currFrame.offset.y + currFrame.size.y)) +
                               currFrame.size.y;

                std::vector<Vector2> texCoords = {
                    Vector2(currFrame.offset.x / spritesheetSize.x,
                            (spritesheetSize.y -
                             (currFrame.offset.y + currFrame.size.y)) /
                                spritesheetSize.y),
                    Vector2(xCoord / spritesheetSize.x,
                            (spritesheetSize.y -
                             (currFrame.offset.y + currFrame.size.y)) /
                                spritesheetSize.y),
                    Vector2(xCoord / spritesheetSize.x,
                            yCoord / spritesheetSize.y),
                    Vector2(currFrame.offset.x / spritesheetSize.x,
                            yCoord / spritesheetSize.y)};
                vertex.texUV = texCoords[index];
            }
        }
    };
} // namespace HyperAPI::Experimental