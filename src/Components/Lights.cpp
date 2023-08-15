#pragma once
#include "Lights.hpp"

namespace HyperAPI::Experimental {
    void c_PointLight::GUI() {
        if (ImGui::CollapsingHeader("Point Light")) {
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
        }
    }

    void c_PointLight::ShadowMapping() {
        glGenFramebuffers(1, &light->pointShadowMapFBO);

        glGenTextures(1, &light->depthCubemap);

        glBindTexture(GL_TEXTURE_CUBE_MAP, light->depthCubemap);

        for (uint32_t i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindFramebuffer(GL_FRAMEBUFFER, light->pointShadowMapFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, light->depthCubemap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

    c_PointLight::c_PointLight() {
        if (light) {
            light->renderShadows = false;
            light->farPlane = 100.0f;
        }
        ShadowMapping();
    };
} // namespace HyperAPI::Experimental