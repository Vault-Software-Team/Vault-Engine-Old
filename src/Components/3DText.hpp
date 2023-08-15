#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"
#include "imgui/imgui.h"
#include "../Renderer/Font.hpp"
#include "Bloom.hpp"

namespace HyperAPI::Experimental {
    struct DLL_API Text3D : public BaseComponent {
        char text[999] = "Hello, World!";
        Font *font = new Font("assets/fonts/OpenSans-Bold.ttf", 48);
        Vector3 color = Vector3(1, 1, 1);
        Vector3 bloomColor;
        float scale = 0.01;
        float y_offset = 0.02;

        Text3D() = default;

        void DeleteComp() override {
            for (auto &c : font->Characters) {
                glDeleteTextures(1, &c.second.TextureID);
            }

            delete font;
        }

        void GUI() override;

        void Update() {
            if (Scene::m_Registry.has<Bloom>(entity)) {
                auto &bloom = Scene::m_Registry.get<Bloom>(entity);
                bloomColor = bloom.bloomColor;
            } else {
                bloomColor = Vector3(0, 0, 0);
            }
        }
    };
} // namespace HyperAPI::Experimental