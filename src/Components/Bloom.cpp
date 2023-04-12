#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "Bloom.hpp"
#include "imgui/imgui.h"

namespace HyperAPI::Experimental {
    void Bloom::GUI() {
        if (ImGui::TreeNode("Bloom")) {
            ImGui::Checkbox("Dynamic Bloom", &dynamic_bloom);
            ImGui::DragFloat("Bloom Threshold", &bloom_threshold, 0.01);
            ImGui::ColorEdit3("Bloom Color", &bloomColor.x);

            ImGui::NewLine();
            if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                Scene::m_Registry.remove<Bloom>(entity);
            }
            ImGui::TreePop();
        }
    }
} // namespace HyperAPI::Experimental