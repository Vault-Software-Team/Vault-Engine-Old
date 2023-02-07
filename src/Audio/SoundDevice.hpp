#pragma once
#include <libs.hpp>
#include <AL/alc.h>

namespace HyperAPI {
    class SoundDevice {
    public:
        static SoundDevice *get();
        void SetPosition(glm::vec3 pos);
        SoundDevice();
        ~SoundDevice();

    private:
        ALCdevice *device;
        ALCcontext *ctx;
    };
} // namespace HyperAPI