#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"

namespace HyperAPI::Experimental {
    struct DLL_API Transform : public BaseComponent {
        Transform *parentTransform = nullptr;
        glm::mat4 transform = glm::mat4(1.0f);
        glm::vec3 position = glm::vec3(0, 0, 0);
        glm::vec3 rotation = glm::vec3(0, 0, 0);
        glm::vec3 scale = glm::vec3(1, 1, 1);
        // forward
        glm::vec3 forward;
        glm::vec3 up;
        glm::vec3 right;

        void GUI();

        void Update() {
            transform = glm::translate(glm::mat4(1.0f), position) *
                        glm::toMat4(glm::quat(rotation)) *
                        glm::scale(glm::mat4(1.0f),
                                   Vector3(scale.x * 0.5, scale.y * 0.5,
                                           scale.z * 0.5));

            glm::mat4 inverted = glm::inverse(transform);
            forward = normalize(glm::vec3(inverted[2]));

            right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
            up = glm::normalize(glm::cross(right, forward));
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