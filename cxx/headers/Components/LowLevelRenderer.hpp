#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"
#include "box2d/b2_types.h"

namespace HyperAPI::Experimental {
    struct LowLevelRenderer : BaseComponent {
        Mesh *mesh;

        LowLevelRenderer() = default;

        void DeleteComp() override {
            if (mesh)
                delete mesh;
        }

        void GUI() override {
            if (ImGui::TreeNode("Low Level Renderer")) {
                ImGui::Text("This component is only controllable via scripts.");
                ImGui::NewLine();
                if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                    Scene::m_Registry.remove<LowLevelRenderer>(entity);
                }

                ImGui::TreePop();
            }
        }

        void AddVertex(const Vertex &vertex) {
            mesh->vertices.push_back(vertex);
        }

        void AddIndex(const uint32_t &index) {
            mesh->indices.push_back(index);
        }
    };
} // namespace HyperAPI::Experimental