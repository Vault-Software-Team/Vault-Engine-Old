#pragma once
#include "Transform.hpp"

namespace HyperAPI::Experimental {
    void Transform::GUI() {
        if (ImGui::CollapsingHeader("Transform")) {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
            DrawVec3Control("Position", position);
            rotation = glm::degrees(rotation);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(),
                                       ImGui::GetCursorPosY() + 4.0f));
            DrawVec3Control("Rotation", rotation);
            rotation = glm::radians(rotation);
            // move the scale up by a bit
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(),
                                       ImGui::GetCursorPosY() - 1.4f));
            DrawVec3Control("Scale", scale, 1);
            ImGui::PopStyleVar();
        }
    }
} // namespace HyperAPI::Experimental