#pragma once
#include <libs.hpp>
#include "CameraComponent.hpp"
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"

namespace HyperAPI::Experimental {
    void CameraComponent::GUI() {
        if (ImGui::CollapsingHeader("Camera")) {
            if (camera != nullptr) {
                // multiple selectable combo boxes for Scene::layers
                if (ImGui::BeginCombo("Layers", "Select Layers")) {
                    for (auto &layer : Scene::layers) {
                        bool isSelected = false;
                        for (auto &camLayer : camera->layers) {
                            if (camLayer == layer.first) {
                                isSelected = true;
                                if (ImGui::Selectable(
                                        std::string(ICON_FA_CHECK +
                                                    std::string(" ") +
                                                    layer.first)
                                            .c_str())) {
                                    camera->layers.erase(
                                        std::remove(camera->layers.begin(),
                                                    camera->layers.end(),
                                                    layer.first),
                                        camera->layers.end());
                                }
                            }
                        }

                        if (!isSelected) {
                            if (ImGui::Selectable(layer.first.c_str())) {
                                camera->layers.push_back(layer.first);
                            }
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::DragFloat("FOV", &camera->cam_fov, 0.01f);
                ImGui::DragFloat("Near", &camera->cam_near, 0.01f);
                ImGui::DragFloat("Far", &camera->cam_far, 0.01f);

                ImGui::Checkbox("Main Camera", &camera->main_camera);
                ImGui::Checkbox("2D Mode", &camera->mode2D);
                ImGui::Checkbox("Depth Camera", &depthCamera);

                if (Scene::mainCamera == camera) {
                    if (ImGui::Button(ICON_FA_CAMERA
                                      " Unselect as Scene Camera")) {
                        Scene::mainCamera = Scene::scene_camera;
                    }
                } else {
                    if (ImGui::Button(ICON_FA_CAMERA
                                      " Select as Scene Camera")) {
                        Scene::mainCamera = camera;
                    }
                }
            }

            ImGui::NewLine();

            ImVec2 winSize = ImGui::GetWindowSize();
            if (ImGui::Button(ICON_FA_TRASH " Delete",
                              ImVec2(winSize.x, 0))) {
                // delete camera;
                // camera = nullptr;
                // Scene::cameras.erase(std::remove(Scene::cameras.begin(),
                // Scene::cameras.end(), camera), Scene::cameras.end());
                m_GameObject->RemoveComponent<CameraComponent>();
            }
        }
    }

    void CameraComponent::Init() {
        auto &transform =
            Scene::m_Registry.get<Experimental::Transform>(entity);
        transform.rotation = glm::vec3(0.0f, 0.0f, -1.0f);

        camera = new Camera(false, 1280, 720, glm::vec3(0, 0, 0), entity);
        Scene::cameras.push_back(camera);

        for (auto &gameObject : (*Scene::m_GameObjects)) {
            if (gameObject->ID == ID) {
                m_GameObject = gameObject;
                break;
            }
        }
    }
} // namespace HyperAPI::Experimental