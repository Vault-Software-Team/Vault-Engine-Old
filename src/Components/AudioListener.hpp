#pragma once
#include <dllapi.hpp>
#include <cstdint>
#include <libs.hpp>
#include <memory>
#include "AL/al.h"
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"
#include "../Audio/SoundDevice.hpp"
#include "imgui/imgui.h"
#include "Transform.hpp"
#include "scene.hpp"

namespace HyperAPI::Experimental {
    struct DLL_API AudioListener : BaseComponent {
        std::unique_ptr<SoundDevice> device{new SoundDevice};
        AudioListener() = default;

        void GUI() override;

        void Update() {
            auto &transform = Scene::m_Registry.get<Transform>(entity);
            device->SetPosition(transform.position);
        }
    };
} // namespace HyperAPI::Experimental