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

        void GUI();

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