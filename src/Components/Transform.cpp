#pragma once
#include "Transform.hpp"

namespace HyperAPI::Experimental {
    void Transform::GUI() {
        if (ImGui::CollapsingHeader("Transform")) {
            DrawVec3Control("Position", position);
            rotation = glm::degrees(rotation);
            DrawVec3Control("Rotation", rotation);
            rotation = glm::radians(rotation);
            // move the scale up by a bit
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(),
                                       ImGui::GetCursorPosY() - 0.6f));
            DrawVec3Control("Scale", scale, 1);
        }
    }
} // namespace HyperAPI::Experimental