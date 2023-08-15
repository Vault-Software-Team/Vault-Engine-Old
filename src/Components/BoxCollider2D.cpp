#pragma once
#include "BoxCollider2D.hpp"

namespace HyperAPI::Experimental {
    void BoxCollider2D::GUI() {
        if (ImGui::CollapsingHeader("Box Collider 2D")) {
            DrawVec2Control("Offset", offset);
            DrawVec2Control("Size", size);

            ImGui::Checkbox("Trigger", &trigger);
            ImGui::DragFloat("Density", &density, 0.01f);
            ImGui::DragFloat("Friction", &friction, 0.01f);
            ImGui::DragFloat("Restitution", &restitution, 0.01f);
            ImGui::DragFloat("Restitution Threshold", &restitutionThreshold,
                             0.01f);

            ImGui::NewLine();
            if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                Scene::m_Registry.remove<BoxCollider2D>(entity);
            }
        }
    }
} // namespace HyperAPI::Experimental