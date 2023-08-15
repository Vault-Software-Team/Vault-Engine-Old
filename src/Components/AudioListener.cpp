#pragma once
#include "AudioListener.hpp"

namespace HyperAPI::Experimental {
    void AudioListener::GUI() {
        if (ImGui::CollapsingHeader("Audio Listener")) {
            if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                Scene::m_Registry.remove<AudioListener>(entity);
            }
        }
    };
} // namespace HyperAPI::Experimental