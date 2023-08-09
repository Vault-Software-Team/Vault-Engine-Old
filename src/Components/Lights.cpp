#pragma once
#include "Lights.hpp"

namespace HyperAPI::Experimental {
    void c_PointLight::GUI() {
        if (ImGui::TreeNode("Point Light")) {
            ImGui::ColorEdit4("Color", &color.x, 0);
            ImGui::DragFloat("Intensity", &intensity, 0.01f);
            ImGui::Checkbox("Render Shadows", &light->renderShadows);
            ImGui::DragFloat("Far Plane", &light->farPlane, 0.01f);

            ImGui::NewLine();
            if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                Scene::PointLights.erase(
                    std::remove(Scene::PointLights.begin(),
                                Scene::PointLights.end(), light),
                    Scene::PointLights.end());
                delete light;
                Scene::m_Registry.remove<c_PointLight>(entity);
            }

            ImGui::TreePop();
        }
    }

    c_PointLight::c_PointLight() {
        if (light) {
            light->renderShadows = false;
            light->farPlane = 100.0f;
        }
        ShadowMapping();
    };
} // namespace HyperAPI::Experimental