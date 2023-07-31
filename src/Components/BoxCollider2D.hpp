#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"
#include "box2d/b2_fixture.h"

namespace HyperAPI::Experimental {
    struct DLL_API BoxCollider2D : public BaseComponent {
        Vector2 offset = Vector2(0, 0);
        Vector2 size = Vector2(0.5, 0.5);

        float density = 1.0f;
        float friction = 0.5f;
        float restitution = 0.0f;
        float restitutionThreshold = 0.5f;
        bool trigger = false;

        void *fixture = nullptr;

        BoxCollider2D() = default;

        void GUI() {
            if (ImGui::TreeNode("Box Collider 2D")) {
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