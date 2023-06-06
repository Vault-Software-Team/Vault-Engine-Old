#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"
#include "box2d/b2_fixture.h"
#include "imgui/imgui.h"

namespace HyperAPI::Experimental {
    struct CircleCollider2D : public BaseComponent {
        Vector2 offset = Vector2(0, 0);

        float radius = 1.0f;
        float density = 1.0f;
        float friction = 0.5f;
        float restitution = 0.0f;
        float restitutionThreshold = 0.5f;
        bool trigger = false;

        void *fixture = nullptr;

        CircleCollider2D() = default;

        void GUI() {
            if (ImGui::TreeNode("Circle Collider 2D")) {
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
                ImGui::TreePop();
            }
        }

        void Update() {
            if (fixture != nullptr) {
                b2Fixture *m_fixture = (b2Fixture *)fixture;

                m_fixture->SetDensity(density);
                m_fixture->SetFriction(friction);
                m_fixture->SetRestitution(restitution);
                m_fixture->SetRestitutionThreshold(restitutionThreshold);
            }
        }
    };
} // namespace HyperAPI::Experimental