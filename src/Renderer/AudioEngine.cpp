#include "AudioEngine.hpp"

namespace HyperAPI::AudioEngine {
    void PlaySound(const std::string &path, float volume, bool loop,
                   int channel) {
        Mix_Chunk *chunk = Mix_LoadWAV(path.c_str());

        Mix_VolumeChunk(chunk, volume * 128);
        Mix_PlayChannel(channel, chunk, loop ? -1 : 0);
    }

    void StopSound(int channel) { Mix_HaltChannel(channel); }

    void PlayMusic(const std::string &path, float volume, bool loop) {
        // generate chunk
        Mix_Music *music = Mix_LoadMUS(path.c_str());
        if (music == NULL) {
            HYPER_LOG("Failed to load music: " + path)
            return;
        }

        // set volume, the volume scale is 0 - 1
        Mix_VolumeMusic(volume * 128);

        Mix_PlayMusic(music, loop ? -1 : 0);
    }

    void StopMusic() { Mix_HaltMusic(); }
} // namespace HyperAPI::AudioEngine