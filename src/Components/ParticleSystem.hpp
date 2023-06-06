#pragma once
#include <fstream>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"
#include "Bloom.hpp"
#include "../Renderer/Shapes.hpp"

namespace HyperAPI::Experimental {
    struct Particle {
        glm::vec2 position, velocity;
        glm::vec4 color;
        float life;

        Particle()
            : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) {}
    };

    struct ParticleEmitter : public BaseComponent {
        Texture *sprite = nullptr;

        ParticleEmitter() = default;
        void DeleteComp() override {
        }

        void GUI() override;
        void Update();
    };
} // namespace HyperAPI::Experimental