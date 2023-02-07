#include "SoundSource.hpp"
#include "AL/al.h"
#include "glm/gtc/type_ptr.hpp"

namespace HyperAPI {
    SoundSource::SoundSource() {
        alGenSources(1, &source);
        alSourcef(source, AL_PITCH, pitch);
        alSourcef(source, AL_GAIN, volume);
        alSource3f(source, AL_POSITION, positions.x, positions.y, positions.z);
        alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
        alSourcef(source, AL_LOOPING, loop);
        alSourcei(source, AL_BUFFER, buffer);
        alSourcef(source, AL_ROLLOFF_FACTOR, 1);
        alSourcef(source, AL_REFERENCE_DISTANCE, 6);
        alSourcef(source, AL_MAX_DISTANCE, max_distance);
    }

    SoundSource::~SoundSource() {
        alDeleteSources(1, &source);
    }

    void SoundSource::SetPosition(glm::vec3 pos) {
        positions = pos;
    }

    void SoundSource::SetVelocity(glm::vec3 vel) {
        velocity = vel;
    }

    void SoundSource::SetVolume(float m_volume) {
        volume = m_volume;
    }

    void SoundSource::SetPitch(float m_pitch) {
        pitch = m_pitch;
    }

    void SoundSource::Loop(bool m_loop) {
        loop = m_loop;
    }

    void SoundSource::SetMaxDistance(float max) {
        max_distance = max;
    }

    void SoundSource::Update() {
        alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
        alSourcef(source, AL_MAX_DISTANCE, max_distance);
        alSourcef(source, AL_PITCH, pitch);
        alSourcef(source, AL_GAIN, volume);
        alSourcef(source, AL_LOOPING, loop);
        alSource3f(source, AL_POSITION, positions.x, positions.y, positions.z);
    }

    void SoundSource::Play(const ALuint buffer) {
        if (buffer != this->buffer) {
            this->buffer = buffer;
            alSourcei(source, AL_BUFFER, this->buffer);
        }

        alSourcePlay(source);
    }
} // namespace HyperAPI