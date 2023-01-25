#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"

#include "Transform.hpp"
#include "Lights.hpp"
#include "scene.hpp"

namespace HyperAPI::Experimental {
    class GameObject : public ComponentEntity {
    public:
        bool enabled = true;
        bool keyDown = false;
        bool isFolder = false;
        std::string NODE_ID = uuid::generate_uuid_v4();
        std::string layer = "Default";
        char layerData[32] = "Default";

        GameObject() { ID = uuid::generate_uuid_v4(); }

        void SetActive(bool active) {
            enabled = active;
        }

        void Update() {
            for (auto &childObject : Scene::m_GameObjects) {
                if (childObject->parentID == ID) {
                    auto &transform = GetComponent<Transform>();
                    auto &childTransform =
                        childObject->GetComponent<Transform>();

                    childTransform.parentTransform = &transform;
                }
            }
        }

        void UpdateEnabled() {
            for (auto &childObject : Scene::m_GameObjects) {
                if (childObject->parentID != ID)
                    continue;

                childObject->SetActive(enabled);
            }
        }

        void DeleteGameObject() {
            Scene::m_Object = nullptr;
            keyDown = true;

            if (HasComponent<c_DirectionalLight>()) {
                Scene::DirLights.erase(
                    std::remove(Scene::DirLights.begin(),
                                Scene::DirLights.end(),
                                GetComponent<c_DirectionalLight>().light),
                    Scene::DirLights.end());
                delete GetComponent<c_DirectionalLight>().light;
            }

            if (HasComponent<c_PointLight>()) {
                Scene::PointLights.erase(
                    std::remove(Scene::PointLights.begin(),
                                Scene::PointLights.end(),
                                GetComponent<c_PointLight>().light),
                    Scene::PointLights.end());
                delete GetComponent<c_PointLight>().light;
            }

            if (HasComponent<c_SpotLight>()) {
                Scene::SpotLights.erase(
                    std::remove(Scene::SpotLights.begin(),
                                Scene::SpotLights.end(),
                                GetComponent<c_SpotLight>().light),
                    Scene::SpotLights.end());
                delete GetComponent<c_SpotLight>().light;
            }

            if (HasComponent<c_Light2D>()) {
                Scene::Lights2D.erase(
                    std::remove(Scene::Lights2D.begin(), Scene::Lights2D.end(),
                                GetComponent<c_Light2D>().light),
                    Scene::Lights2D.end());
                delete GetComponent<c_Light2D>().light;
            }

            Scene::m_Object = nullptr;

            Scene::m_Registry.remove(entity);
            Scene::m_Registry.destroy(entity);

            Scene::m_GameObjects.erase(std::remove(Scene::m_GameObjects.begin(),
                                                   Scene::m_GameObjects.end(),
                                                   this),
                                       Scene::m_GameObjects.end());

            for (auto &gameObject : Scene::m_GameObjects) {
                if (gameObject->parentID == ID) {
                    gameObject->parentID = "NO_PARENT";
                    gameObject->DeleteGameObject();
                }
            }
        }

        void GUI() {
            bool item;
            bool hasChildren = false;
            for (auto &gameObject : Scene::m_GameObjects) {
                if (gameObject->parentID == ID) {
                    hasChildren = true;
                    // if enabled is false make the text grey
                    if (!enabled) {
                        ImGui::PushStyleColor(ImGuiCol_Text,
                                              ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                        item = ImGui::TreeNode(
                            NODE_ID.c_str(),
                            std::string(std::string(ICON_FA_CUBE) + " " + name)
                                .c_str());
                        ImGui::PopStyleColor();
                    } else {
                        item = ImGui::TreeNode(
                            NODE_ID.c_str(),
                            std::string(std::string(ICON_FA_CUBE) + " " + name)
                                .c_str());
                    }
                    break;
                }
            }

            if (!hasChildren) {
                ImGui::PushID(NODE_ID.c_str());
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 16,
                                           ImGui::GetCursorPos().y));
                if (!enabled) {
                    ImGui::PushStyleColor(ImGuiCol_Text,
                                          ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                    item = ImGui::Selectable(
                        std::string(std::string(ICON_FA_CUBE) + " " + name)
                            .c_str());
                    ImGui::PopStyleColor();
                } else {
                    item = ImGui::Selectable(
                        std::string(std::string(ICON_FA_CUBE) + " " + name)
                            .c_str());
                }
                ImGui::PopID();
            }

            if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered() &&
                !ImGui::IsMouseDragging(0)) {
                Scene::m_Object = this;
                strncpy(Scene::name, Scene::m_Object->name.c_str(), 499);
                Scene::name[499] = '\0';

                strncpy(Scene::tag, Scene::m_Object->tag.c_str(), 499);
                Scene::tag[499] = '\0';

                strncpy(Scene::layer, Scene::m_Object->layer.c_str(), 32);
                Scene::layer[31] = '\0';
            }

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                dirPayloadData = ID;
                ImGui::SetDragDropPayload("game_object", &dirPayloadData,
                                          dirPayloadData.size());
                ImGui::Text(name.c_str());
                ImGui::EndDragDropSource();
            }

            // drop target
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload =
                        ImGui::AcceptDragDropPayload("game_object")) {
                    if (dirPayloadData != ID) {
                        for (auto &gameObject : Scene::m_GameObjects) {
                            if (gameObject->ID == dirPayloadData) {
                                gameObject->parentID = ID;
                                break;
                            }
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (Input::IsKeyPressed(KEY_DELETE) && Scene::m_Object == this &&
                !keyDown) {
                DeleteGameObject();
            } else if (!Input::IsKeyPressed(KEY_DELETE)) {
                keyDown = false;
            }

            if (item && hasChildren) {

                for (auto &gameObject : Scene::m_GameObjects) {
                    if (gameObject->parentID == ID) {
                        gameObject->GUI();
                    }
                }

                ImGui::TreePop();
            }
        }
    };
} // namespace HyperAPI::Experimental