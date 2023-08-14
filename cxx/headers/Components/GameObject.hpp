#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"

#include "Transform.hpp"
#include "Lights.hpp"
#include "imgui/imgui.h"
#include "scene.hpp"

namespace HyperAPI::Experimental {
    class DLL_API GameObject : public ComponentEntity {
    public:
        bool enabled = true;
        bool schedule = false;
        bool setted_schedule = false;
        bool keyDown = false;
        bool isFolder = false;
        std::string NODE_ID = uuid::generate_uuid_v4();
        std::string layer = "Default";
        std::string type = "Entity";
        char layerData[32] = "Default";

        GameObject() { ID = uuid::generate_uuid_v4(); }

        void SetActive(bool active) {
            enabled = active;
        }

        void Update() {
            for (auto &childObject : (*Scene::m_GameObjects)) {
                if (childObject->parentID == ID) {
                    auto &transform = GetComponent<Transform>();
                    auto &childTransform =
                        childObject->GetComponent<Transform>();

                    childTransform.parentTransform = &transform;
                }
            }
        }

        void UpdateEnabled() {
            for (auto &childObject : (*Scene::m_GameObjects)) {
                if (!childObject)
                    continue;
                if (childObject->parentID != ID)
                    continue;

                if (enabled) {
                    if (childObject->setted_schedule) {
                        childObject->SetActive(childObject->schedule);
                        childObject->setted_schedule = false;
                    }
                }

                if (!childObject->enabled && enabled) {
                    childObject->SetActive(false);
                } else if (!enabled) {
                    if (!childObject->setted_schedule) {
                        childObject->schedule = childObject->enabled;
                        childObject->setted_schedule = true;
                    }
                    childObject->SetActive(false);
                } else if (childObject->enabled && enabled) {
                    childObject->SetActive(true);
                }
            }
        }

        void DeleteGameObject();

        void GUI() {
            bool item;
            bool hasChildren = false;
            for (auto &gameObject : (*Scene::m_GameObjects)) {
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
                        for (auto &gameObject : (*Scene::m_GameObjects)) {
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

                for (auto &gameObject : (*Scene::m_GameObjects)) {
                    if (gameObject->parentID == ID) {
                        gameObject->GUI();
                    }
                }

                ImGui::TreePop();
            }
        }
    };
} // namespace HyperAPI::Experimental