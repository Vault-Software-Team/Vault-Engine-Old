#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"

#include "Transform.hpp"
#include "Lights.hpp"
#include "imgui/imgui.h"
#include "scene.hpp"

namespace HyperAPI::Experimental {
    class DLL_API GameObject : public ComponentEntity {
    public:
        bool enabled = true;
        bool schedule = false;
        bool schedule_deletion = false;
        bool setted_schedule = false;
        bool keyDown = false;
        bool isFolder = false;
        std::string NODE_ID = uuid::generate_uuid_v4();
        std::string layer = "Default";
        std::string type = "Entity";
        char layerData[32] = "Default";

        GameObject() { ID = uuid::generate_uuid_v4(); }

        void SetActive(bool active) {
            enabled = active;
        }

        void Update() {
            for (auto &childObject : (*Scene::m_GameObjects)) {
                if (childObject->parentID == ID) {
                    auto &transform = GetComponent<Transform>();
                    auto &childTransform =
                        childObject->GetComponent<Transform>();

                    childTransform.parentTransform = &transform;
                }
            }
        }

        void UpdateEnabled() {
            for (auto &childObject : (*Scene::m_GameObjects)) {
                if (!childObject)
                    continue;
                if (childObject->parentID != ID)
                    continue;

                if (enabled) {
                    if (childObject->setted_schedule) {
                        childObject->SetActive(childObject->schedule);
                        childObject->setted_schedule = false;
                    }
                }

                if (!childObject->enabled && enabled) {
                    childObject->SetActive(false);
                } else if (!enabled) {
                    if (!childObject->setted_schedule) {
                        childObject->schedule = childObject->enabled;
                        childObject->setted_schedule = true;
                    }
                    childObject->SetActive(false);
                } else if (childObject->enabled && enabled) {
                    childObject->SetActive(true);
                }
            }
        }

        void DeleteGameObject();

        void GUI();
    };
} // namespace HyperAPI::Experimental