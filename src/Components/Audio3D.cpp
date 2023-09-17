#pragma once
#include "Audio3D.hpp"

namespace HyperAPI::Experimental {
    void Audio3D::GUI() {
        if (ImGui::CollapsingHeader("3D Audio")) {
            ImGui::Text("NOTE: The audio file must be mono for it to be 3D!");
            ImGui::Selectable(("File: " + file).c_str(), false);
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload =
                        ImGui::AcceptDragDropPayload("file")) {
                    dirPayloadData.erase(0, cwd.length() + 1);

                    file = dirPayloadData;
                }

                ImGui::EndDragDropTarget();
            }
            ImGui::NewLine();

            DrawVec3Control("Velocity", src.velocity);
            ImGui::Checkbox("Loop", &src.loop);
            ImGui::DragFloat("Volume", &src.volume, 0.1, 0, 1);
            ImGui::DragFloat("Pitch", &src.pitch, 0.1);
            ImGui::DragFloat("Max Distance", &src.max_distance, 0.1);
            ImGui::Checkbox("Play On Start", &on_start);

            ImGui::NewLine();
            if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                Scene::m_Registry.remove<Audio3D>(entity);
            }
        }
    }

    void Audio3D::Update() {
        if (m_src == nullptr)
            Start();

        m_src->pitch = src.pitch;
        m_src->volume = src.volume;
        m_src->max_distance = src.max_distance;
        m_src->loop = src.loop;
        m_src->positions = src.positions;
        m_src->velocity = src.velocity;

        auto &transform = Scene::m_Registry.get<Transform>(entity);
        m_src->SetPosition(transform.position);
        m_src->Update();
    }
} // namespace HyperAPI::Experimental