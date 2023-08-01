#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"

namespace HyperAPI::Experimental {
    struct DLL_API Rigidbody2D : public BaseComponent {
        b2BodyType type = b2_staticBody;
        bool fixedRotation = false;
        float gravityScale = 1.0f;
        void *body = nullptr;

        void GUI() {
            if (ImGui::TreeNode("Rigidbody 2D")) {
                ImGui::Text("Type");
                ImGui::RadioButton("Static", (int *)&type, 0);
                ImGui::RadioButton("Dynamic", (int *)&type, 1);
                ImGui::RadioButton("Kinematic", (int *)&type, 2);
                ImGui::Checkbox("Fixed Rotation", &fixedRotation);
                ImGui::DragFloat("Gravity Scale", &gravityScale, 0.01f);

                if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                    Scene::m_Registry.remove<Rigidbody2D>(entity);
                }

                ImGui::TreePop();
            }
        }

        void SetVelocity(float x, float y) {
            if (body == nullptr)
                return;
            b2Body *b = (b2Body *)body;
            b->SetLinearVelocity(b2Vec2(x, y));
        }

        void SetAngularVelocity(float velocity) {
            if (body == nullptr)
                return;
            b2Body *b = (b2Body *)body;
            b->SetAngularVelocity(velocity);
        }

        void SetPosition(float x, float y) {
            if (body == nullptr)
                return;
            b2Body *b = (b2Body *)body;
            b->SetTransform(b2Vec2(x, y), b->GetAngle());
        }

        void Force(float x, float y) {
            if (body == nullptr)
                return;
            b2Body *b = (b2Body *)body;
            b->ApplyForceToCenter(b2Vec2(x, y), true);
        }

        void Torque(float torque) {
            if (body == nullptr)
                return;
            b2Body *b = (b2Body *)body;
            b->ApplyTorque(torque, true);
        }
    };
} // namespace HyperAPI::Experimental