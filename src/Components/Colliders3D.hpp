#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"

#include "MeshRenderer.hpp"

namespace HyperAPI::Experimental {
    struct DLL_API Collider3D : public BaseComponent {
        btCollisionShape *shape = nullptr;
        virtual void CreateShape() {}

        // make Collider3D be shape so for example you can do
        // collider3D->setMargin(0.05f);
        Collider3D() = default;
        Collider3D(const Collider3D &) = default;
        explicit operator const btCollisionShape *() const { return shape; }
    };

    struct DLL_API BoxCollider3D : Collider3D {
        Vector3 size = Vector3(1, 1, 1);

        void CreateShape() override {
            shape =
                new btBoxShape(btVector3((size.x / 2) / 2, (size.y / 2) / 2, (size.z / 2) / 2));
            shape->setMargin(0.05f);
        }

        void GUI() override {
            if (ImGui::TreeNode("Box Collider 3D")) {
                DrawVec3Control("Size", size);

                ImGui::NewLine();
                if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                    Scene::m_Registry.remove<BoxCollider3D>(entity);
                }
                ImGui::TreePop();
            }
        }
    };

    struct DLL_API MeshCollider3D : Collider3D {
        Vector3 size = Vector3(1, 1, 1);

        void CreateShape(MeshRenderer *renderer) {
            if (renderer->m_Mesh) {
                auto mesh = renderer->m_Mesh;

                shape = new btConvexHullShape();
                for (auto &vertex : mesh->vertices) {
                    ((btConvexHullShape *)shape)
                        ->addPoint(btVector3(vertex.position.x,
                                             vertex.position.y,
                                             vertex.position.z));
                }
                // add transform scale to shape
                shape->setLocalScaling(
                    btVector3(size.x / 2, size.y / 2, size.z / 2));
                shape->setMargin(0.05f);
            }
        }

        void GUI() override {
            if (ImGui::TreeNode("Mesh Collider 3D")) {
                DrawVec3Control("Size", size);

                ImGui::NewLine();
                if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                    Scene::m_Registry.remove<MeshCollider3D>(entity);
                }
                ImGui::TreePop();
            }
        }
    };
} // namespace HyperAPI::Experimental