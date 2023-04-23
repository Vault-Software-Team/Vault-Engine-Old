#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"

namespace HyperAPI::Experimental {
    struct Transform : public BaseComponent {
        Transform *parentTransform = nullptr;
        glm::mat4 transform = glm::mat4(1.0f);
        glm::vec3 position = glm::vec3(0, 0, 0);
        glm::vec3 rotation = glm::vec3(0, 0, 0);
        glm::vec3 scale = glm::vec3(1, 1, 1);
        // forward
        glm::vec3 forward;
        glm::vec3 up;
        glm::vec3 right;

        void GUI() {
            ImGui::PushStyleColor(ImGuiCol_Header,
                                  ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
            if (ImGui::TreeNode("Transform")) {
                DrawVec3Control("Position", position);
                rotation = glm::degrees(rotation);
                DrawVec3Control("Rotation", rotation);
                rotation = glm::radians(rotation);
                // move the scale up by a bit
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(),
                                           ImGui::GetCursorPosY() - 0.6f));
                DrawVec3Control("Scale", scale, 1);

                ImGui::TreePop();
            }
            ImGui::PopStyleColor();
        }

        void Update() {
            right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
            up = glm::normalize(glm::cross(right, forward));

            transform = glm::translate(glm::mat4(1.0f), position) *
                        glm::toMat4(glm::quat(rotation)) *
                        glm::scale(glm::mat4(1.0f),
                                   Vector3(scale.x * 0.5, scale.y * 0.5,
                                           scale.z * 0.5));

            const glm::mat4 inverted = glm::inverse(transform);
            forward = normalize(glm::vec3(inverted[2])) * glm::vec3(1, 1, -1);
        }

        void LookAt(glm::vec3 target) {
            glm::vec3 direction = glm::normalize(target - position);
            rotation = glm::eulerAngles(
                glm::quatLookAt(direction, glm::vec3(0, 1, 0)));
        }

        void Translate(glm::vec3 translation) { position += translation; }

        void Rotate(glm::vec3 rotation) { this->rotation += rotation; }

        void Scale(glm::vec3 scale) { this->scale += scale; }
    };
} // namespace HyperAPI::Experimental