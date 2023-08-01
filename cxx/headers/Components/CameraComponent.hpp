#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"

#include "GameObject.hpp"
#include "Transform.hpp"
#include "../Renderer/Camera.hpp"
#include "scene.hpp"

namespace HyperAPI::Experimental {
    struct DLL_API CameraComponent : public BaseComponent {
        Camera *camera = nullptr;
        GameObject *m_GameObject = nullptr;
        bool depthCamera = false;

        CameraComponent() = default;

        ~CameraComponent() {
            Scene::cameras.erase(std::remove(Scene::cameras.begin(),
                                             Scene::cameras.end(), camera),
                                 Scene::cameras.end());
        }

        void Init();

        void GUI();
    };

} // namespace HyperAPI::Experimental