#pragma once
#include "CircleCollider2D.hpp"

namespace HyperAPI::Experimental {
    void CircleCollider2D::GUI() {
        if (ImGui::CollapsingHeader("Circle Collider 2D")) {
            DrawVec2Control("Offset", offset);
            ImGui::DragFloat("Radius", &radius);
            ImGui::Checkbox("Trigger", &trigger);
            ImGui::DragFloat("Density", &density, 0.01f);
            ImGui::DragFloat("Friction", &friction, 0.01f);
            ImGui::DragFloat("Restitution", &restitution, 0.01f);
            ImGui::DragFloat("Restitution Threshold", &restitutionThreshold,
                             0.01f);

            ImGui::NewLine();
            if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                Scene::m_Registry.remove<CircleCollider2D>(entity);
            }
        }
    }
}; // namespace HyperAPI::Experimental