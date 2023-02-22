#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"

#include "Transform.hpp"

namespace HyperAPI::Experimental {
    struct c_PointLight : public BaseComponent {
        glm::vec3 lightPos = glm::vec3(0, 0, 0);
        glm::vec3 color = glm::vec3(1, 1, 1);
        float intensity = 1.0f;

        PointLight *light =
            new PointLight(Scene::PointLights, lightPos, color, intensity);

        c_PointLight() = default;

        void GUI() {

            if (ImGui::TreeNode("Point Light")) {
                ImGui::ColorEdit4("Color", &color.x, 0);
                ImGui::DragFloat("Intensity", &intensity, 0.01f);

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

        void Update() {
            auto &transform = Scene::m_Registry.get<Transform>(entity);
            lightPos = transform.position;

            light->lightPos = lightPos;
            light->color = color;
            light->intensity = intensity;
        }
    };

    struct c_Light2D : public BaseComponent {
        glm::vec3 lightPos = glm::vec3(0, 0, 0);
        glm::vec3 color = glm::vec3(1, 1, 1);
        float range = 1.0f;

        Light2D *light =
            new Light2D(Scene::Lights2D, lightPos, Vector4(color, 1.0f), range);

        c_Light2D() = default;

        void GUI() {

            if (ImGui::TreeNode("2D Light")) {
                ImGui::ColorEdit4("Color", &color.x, 0);
                ImGui::DragFloat("Range", &range, 0.01f);

                ImGui::NewLine();
                if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                    Scene::Lights2D.erase(std::remove(Scene::Lights2D.begin(),
                                                      Scene::Lights2D.end(),
                                                      light),
                                          Scene::Lights2D.end());
                    delete light;
                    Scene::m_Registry.remove<c_Light2D>(entity);
                }

                ImGui::TreePop();
            }
        }

        void Update() {
            auto &transform = Scene::m_Registry.get<Transform>(entity);
            lightPos = transform.position;

            light->lightPos = lightPos;
            light->color = color;
            light->range = range;
        }
    };

    struct c_SpotLight : public BaseComponent {
        glm::vec3 lightPos = glm::vec3(0, 0, 0);
        glm::vec3 color = glm::vec3(1, 1, 1);
        float outerCone;
        float innerCone;
        Vector3 angle = Vector3(0.0f, -1.0f, 0.0f);
        SpotLight *light = new SpotLight(Scene::SpotLights, lightPos, color);

        c_SpotLight() = default;
        void Init() override {
            if (Scene::m_Registry.has<Transform>(entity)) {
                auto &transform = Scene::m_Registry.get<Transform>(entity);
                if (transform.rotation.y == 0) {
                    transform.rotation.y = glm::radians(-1.0f);
                }
            }
        }

        void GUI() {
            if (ImGui::TreeNode("Spot Light")) {
                ImGui::ColorEdit4("Color", &color.x, 0);

                ImGui::NewLine();
                if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                    Scene::SpotLights.erase(
                        std::remove(Scene::SpotLights.begin(),
                                    Scene::SpotLights.end(), light),
                        Scene::SpotLights.end());
                    delete light;
                    Scene::m_Registry.remove<c_SpotLight>(entity);
                }
                ImGui::TreePop();
            }
        }

        void Update() {
            auto &transform = Scene::m_Registry.get<Transform>(entity);
            lightPos = transform.position;
            angle = transform.rotation;

            light->lightPos = lightPos;
            light->color = color;
            light->angle = glm::degrees(angle);
        }
    };

    struct c_DirectionalLight : public BaseComponent {
        glm::vec3 lightPos = glm::vec3(0, 0, 0);
        glm::vec3 color = glm::vec3(1, 1, 1);
        float intensity = 1;
        DirectionalLight *light =
            new DirectionalLight(Scene::DirLights, lightPos, color);

        c_DirectionalLight() = default;

        void GUI() {
            auto &transform = Scene::m_Registry.get<Transform>(entity);
            lightPos = transform.position;

            if (ImGui::TreeNode("Directional Light")) {
                ImGui::ColorEdit4("Color", &color.x, 0);
                ImGui::DragFloat("Intensity", &intensity, 0.01f);

                ImGui::NewLine();
                if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                    Scene::DirLights.erase(std::remove(Scene::DirLights.begin(),
                                                       Scene::DirLights.end(),
                                                       light),
                                           Scene::DirLights.end());
                    delete light;
                    Scene::m_Registry.remove<c_DirectionalLight>(entity);
                }
                ImGui::TreePop();
            }

            light->lightPos = lightPos;
            light->color = color;
        }

        void Update() {
            auto &transform = Scene::m_Registry.get<Transform>(entity);
            lightPos = transform.position;

            light->lightPos = lightPos;
            light->color = color;
        }
    };
} // namespace HyperAPI::Experimental