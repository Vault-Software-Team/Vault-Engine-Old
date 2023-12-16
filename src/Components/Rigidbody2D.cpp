#pragma once
#include "Rigidbody2D.hpp"

float clip_maxmin(float n, float lower, float upper) {
    return std::max(lower, std::min(n, upper));
}

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

    void Rigidbody2D::MoveToPosition(glm::vec2 target, float velocity) {
        if (Scene::m_Registry.has<Transform>(entity)) {
            transform = &Scene::m_Registry.get<Transform>(entity);
        }

        if (transform == nullptr)
            return;
        float x = (target.x - transform->position.x) * velocity;
        float y = (target.y - transform->position.y) * velocity;

        SetVelocity(clip_maxmin(x, -velocity, velocity), clip_maxmin(y, -velocity, velocity));
    }
} // namespace HyperAPI::Experimental