#pragma once
#include "Colliders3D.hpp"

namespace HyperAPI::Experimental {
    void BoxCollider3D::GUI() {
        if (ImGui::CollapsingHeader("Box Collider 3D")) {
            DrawVec3Control("Size", size);

            ImGui::NewLine();
            if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                Scene::m_Registry.remove<BoxCollider3D>(entity);
            }
        }
    }

    void MeshCollider3D::GUI() {
        if (ImGui::CollapsingHeader("Mesh Collider 3D")) {
            DrawVec3Control("Size", size);

            ImGui::NewLine();
            if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                Scene::m_Registry.remove<MeshCollider3D>(entity);
            }
        }
    }
} // namespace HyperAPI::Experimental