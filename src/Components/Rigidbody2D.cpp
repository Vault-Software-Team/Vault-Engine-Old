#pragma once
#include "Rigidbody2D.hpp"

namespace HyperAPI::Experimental {
    void Rigidbody2D::GUI() {
        if (ImGui::CollapsingHeader("Rigidbody 2D")) {
            ImGui::Text("Type");
            ImGui::RadioButton("Static", (int *)&type, 0);
            ImGui::RadioButton("Kinematic", (int *)&type, 1);
            ImGui::RadioButton("Dynamic", (int *)&type, 2);

            ImGui::Checkbox("Fixed Rotation", &fixedRotation);
            ImGui::DragFloat("Gravity Scale", &gravityScale, 0.01f);

            if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                Scene::m_Registry.remove<Rigidbody2D>(entity);
            }
        }
    }
} // namespace HyperAPI::Experimental