#pragma once
#include <libs.hpp>

namespace HyperAPI::AudioEngine {
    void PlaySound(const std::string &path, float volume = 1.0f,
                   bool loop = false, int channel = -1);

    void StopSound(int channel = -1);

    void PlayMusic(const std::string &path, float volume = 1.0f,
                   bool loop = false);

    void StopMusic();
} // namespace HyperAPI::AudioEngine