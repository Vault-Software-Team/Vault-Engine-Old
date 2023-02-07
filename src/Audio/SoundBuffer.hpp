#pragma once
#include <libs.hpp>
#include <AL/alc.h>
#include <AL/al.h>

namespace HyperAPI {
    class SoundBuffer {
    public:
        static SoundBuffer *get();
        ALuint AddSoundEffect(const char *filename);
        bool RemoveSoundEffect(const ALuint &buffer);

    private:
        SoundBuffer();
        ~SoundBuffer();

        std::vector<ALuint> buffers;
    };
} // namespace HyperAPI