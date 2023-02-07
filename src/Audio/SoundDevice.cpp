#include "SoundDevice.hpp"
#include "AL/alc.h"
#include "AL/al.h"
#include "libs.hpp"

namespace HyperAPI {
    SoundDevice *SoundDevice::get() {
        static SoundDevice *snd_device = new SoundDevice();
        return snd_device;
    }

    SoundDevice::SoundDevice() {
        device = alcOpenDevice(nullptr);
        if (!device)
            throw("Failed to get sound device");

        ctx = alcCreateContext(device, nullptr);
        if (!ctx)
            throw("Failed to create context");

        if (!alcMakeContextCurrent(ctx))
            throw("Failed to make context current");

        const ALCchar *name = nullptr;
        if (alcIsExtensionPresent(device, "ALC_ENUMERATE_ALL_EXT")) {
            name = alcGetString(device, ALC_ALL_DEVICES_SPECIFIER);
        }

        if (!name || alcGetError(device) != AL_NO_ERROR) {
            name = alcGetString(device, ALC_DEVICE_SPECIFIER);
        }

        HYPER_LOG(std::string("ALC Opened ") + name);

        std::vector<float> ori = {
            0,
            0,
            -1,
            0,
            1,
            0,
        };
        alListenerfv(AL_ORIENTATION, ori.data());
        alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
    }

    void SoundDevice::SetPosition(glm::vec3 pos) {
        alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
    }

    SoundDevice::~SoundDevice() {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(ctx);
        alcCloseDevice(device);
    }
} // namespace HyperAPI