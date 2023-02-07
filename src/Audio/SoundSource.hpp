#pragma once
#include <libs.hpp>
#include <AL/alc.h>
#include <AL/al.h>

namespace HyperAPI {
    class SoundSource {
    public:
        SoundSource();
        ~SoundSource();

        void Play(const ALuint buffer);
        void SetPosition(glm::vec3 pos);
        void SetVelocity(glm::vec3 vel);
        void SetVolume(float m_volume);
        void SetPitch(float m_pitch);
        void Loop(bool m_loop);
        void SetMaxDistance(float max);
        void Update();

        float pitch = 1;
        float volume = 1;
        float max_distance = 15;
        bool loop;
        glm::vec3 positions{0, 0, 0};
        glm::vec3 velocity{0, 0, 0};

        ALuint GetBuffer() const { return source; }

    private:
        ALuint source;
        ALuint buffer = 0;
    };
} // namespace HyperAPI