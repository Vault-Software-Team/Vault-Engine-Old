#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"

namespace HyperAPI::Experimental {
    struct Bloom : public BaseComponent {
        Vector3 bloomColor;

        Bloom() = default;

        void GUI() override {
            if (ImGui::TreeNode("Bloom")) {
                ImGui::ColorEdit3("Bloom Color", &bloomColor.x);

                ImGui::NewLine();
                if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                    Scene::m_Registry.remove<Bloom>(entity);
                }
                ImGui::TreePop();
            }
        }
    };

} // namespace HyperAPI::Experimental