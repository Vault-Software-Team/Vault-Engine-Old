#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"
#include "box2d/b2_fixture.h"
#include <f_GameObject/f_GameObject.hpp>
#include "GameObject.hpp"
#include "Rigidbody2D.hpp"

namespace HyperAPI::Experimental {
    struct DLL_API BoxCollider2D : public BaseComponent {
        Vector2 offset = Vector2(0, 0);
        Vector2 size = Vector2(0.5, 0.5);

        float density = 1.0f;
        float friction = 0.5f;
        float restitution = 0.0f;
        float restitutionThreshold = 0.5f;
        bool trigger = false;
        bool enable_joint = false;
        char joint_body1[256];
        char joint_body2[256];

        void *fixture = nullptr;
        b2Joint *joint = nullptr;
        b2JointDef joint_def;

        BoxCollider2D() = default;

        void GUI();

        void Update() {
            if (fixture != nullptr) {
                b2Fixture *m_fixture = (b2Fixture *)fixture;

                m_fixture->SetDensity(density);
                m_fixture->SetFriction(friction);
                m_fixture->SetRestitution(restitution);
                m_fixture->SetRestitutionThreshold(restitutionThreshold);
            }

            if (joint != nullptr && enable_joint) {
                GameObject *bodyA = f_GameObject::FindGameObjectByName(joint_body1);
                GameObject *bodyB = f_GameObject::FindGameObjectByName(joint_body2);

                if (bodyA && bodyB) {
                    joint_def.bodyA = (b2Body *)bodyA->GetComponent<Rigidbody2D>().body;
                    joint_def.bodyB = (b2Body *)bodyB->GetComponent<Rigidbody2D>().body;
                    joint_def.collideConnected = true;
                }
            }
        }
    };
} // namespace HyperAPI::Experimental