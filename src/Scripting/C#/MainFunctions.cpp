#include "MainFunctions.hpp"

namespace HyperAPI::CsharpScriptEngine::Functions {
    float cpp_DeltaTime() {
        return Timestep::deltaTime;
    }

    float cpp_Ambient(bool toGet, float value) {
        if (!toGet)
            config.ambientLight = value;

        return config.ambientLight;
    }

    int cpp_RandomInt(int min, int max) {
        return rand() % (max - min + 1) + min;
    }

    void ExitProgram() {
#ifdef GAME_BUILD
        exit(0);
#else
        HyperAPI::isRunning = false;
#endif
    }

    MonoString *cpp_ToStringF(float value) {
        return mono_string_new(CsharpVariables::appDomain, std::to_string(value).c_str());
    }

    MonoString *cpp_ToStringD(double value) {
        return mono_string_new(CsharpVariables::appDomain, std::to_string(value).c_str());
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions