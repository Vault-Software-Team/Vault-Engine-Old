#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"

#include "Transform.hpp"

namespace HyperAPI::Experimental {
    inline Shader *shadowCubeMapShader = nullptr;
    struct DLL_API c_PointLight : public BaseComponent {
        glm::vec3 lightPos = glm::vec3(0, 0, 0);
        glm::vec3 color = glm::vec3(1, 1, 1);
        float intensity = 1.0f;

        PointLight *light =
            new PointLight(Scene::PointLights, lightPos, color, intensity);

        c_PointLight();

        void BindCubemap(Shader &shader) {
            shader.Bind();
            glActiveTexture(GL_TEXTURE12);
            glBindTexture(GL_TEXTURE_CUBE_MAP, light->depthCubemap);
            shader.SetUniform1i("shadow_cubemap_buffer", 12);
            if (light) {
                shader.SetUniform1i("shadow_cubemap_set", light->renderShadows);
            }
            shader.SetUniform3f("pointLightPos", lightPos.x, lightPos.y, lightPos.z);
            shader.SetUniform1f("farPlane", light->farPlane);
        }

        void ShadowMapping();

        void SetShadowMapValues() {
            glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, light->farPlane);
            glm::mat4 shadowMatrices[] =
                {
                    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
                    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
                    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
                    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
                    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
                    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))};

            shadowCubeMapShader->Bind();
            shadowCubeMapShader->SetUniform1f("farPlane", light->farPlane);
            shadowCubeMapShader->SetUniform3f("lightPos", lightPos.x, lightPos.y, lightPos.z);
            shadowCubeMapShader->SetUniformMat4("shadowMatrices[0]", shadowMatrices[0]);
            shadowCubeMapShader->SetUniformMat4("shadowMatrices[1]", shadowMatrices[1]);
            shadowCubeMapShader->SetUniformMat4("shadowMatrices[2]", shadowMatrices[2]);
            shadowCubeMapShader->SetUniformMat4("shadowMatrices[3]", shadowMatrices[3]);
            shadowCubeMapShader->SetUniformMat4("shadowMatrices[4]", shadowMatrices[4]);
            shadowCubeMapShader->SetUniformMat4("shadowMatrices[5]", shadowMatrices[5]);
        }

        void GUI();

        void Update() {
            auto &transform = Scene::m_Registry.get<Transform>(entity);
            lightPos = transform.position;

            light->lightPos = lightPos;
            light->color = color;
            light->intensity = intensity;
        }
    };

    struct DLL_API c_Light2D : public BaseComponent {
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

    struct DLL_API c_SpotLight : public BaseComponent {
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

    struct DLL_API c_DirectionalLight : public BaseComponent {
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