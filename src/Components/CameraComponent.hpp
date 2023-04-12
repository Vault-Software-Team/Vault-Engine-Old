#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"

#include "GameObject.hpp"
#include "Transform.hpp"
#include "../Renderer/Camera.hpp"
#include "scene.hpp"

namespace HyperAPI::Experimental {
    struct CameraComponent : public BaseComponent {
        Camera *camera = nullptr;
        GameObject *m_GameObject = nullptr;
        bool depthCamera = false;

        CameraComponent() = default;

        ~CameraComponent() {
            Scene::cameras.erase(std::remove(Scene::cameras.begin(),
                                             Scene::cameras.end(), camera),
                                 Scene::cameras.end());
        }

        void Init() {
            auto &transform =
                Scene::m_Registry.get<Experimental::Transform>(entity);
            transform.rotation = glm::vec3(0.0f, 0.0f, -1.0f);

            camera = new Camera(false, 1280, 720, glm::vec3(0, 0, 0), entity);
            Scene::cameras.push_back(camera);

            for (auto &gameObject : Scene::m_GameObjects) {
                if (gameObject->ID == ID) {
                    m_GameObject = gameObject;
                    break;
                }
            }
        }

        void GUI();
    };

} // namespace HyperAPI::Experimental