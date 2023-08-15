#pragma once
#include "ParticleSystem.hpp"
namespace HyperAPI::Experimental {
    void ParticleEmitter::GUI() {
        if (ImGui::CollapsingHeader("Particle Emitter")) {
            if (sprite != nullptr) {
                HYPER_LOG("kILL YOURSELF BITCH")
                ImGui::ImageButton((void *)sprite->tex->ID,
                                   ImVec2(128, 128), ImVec2(0, 1),
                                   ImVec2(1, 0));
            } else {
                ImGui::ImageButton((void *)0, ImVec2(128, 128));
            }
            Scene::DropTargetMat(Scene::DRAG_SPRITE_NO_MESH, nullptr, sprite);
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1) &&
                sprite != nullptr) {
                delete sprite;
                sprite = nullptr;
            }

            ImGui::NewLine();
            if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                DeleteComp();
                Scene::m_Registry.remove<ParticleEmitter>(entity);
            }
        }
    }

    void ParticleEmitter::Update() {
    }
} // namespace HyperAPI::Experimental