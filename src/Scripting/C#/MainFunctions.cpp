#include "MainFunctions.hpp"

namespace HyperAPI::CsharpScriptEngine::Functions {
    float cpp_DeltaTime() {
        return Timestep::deltaTime;
    }
    void ExitProgram() {
#ifdef GAME_BUILD
        exit(0)
#else
        HyperAPI::isRunning = false;
#endif
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions