#include "AudioFunctions.hpp"
#include "../../Renderer/AudioEngine.hpp"

namespace HyperAPI::CsharpScriptEngine::Functions {
    void AudioPlay(MonoString *file, float volume, bool loop, int channel) {
        using namespace Experimental;
        std::string str = mono_string_to_utf8(file);

        AudioEngine::PlaySound(str, volume, loop, channel);
    }

    void AudioMusic(MonoString *file, float volume, bool loop) {
        using namespace Experimental;
        std::string str = mono_string_to_utf8(file);

        AudioEngine::PlayMusic(str, volume, loop);
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions