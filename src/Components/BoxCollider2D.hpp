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

        void GUI();

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