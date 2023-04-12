#include "MainFunctions.hpp"

namespace HyperAPI::CsharpScriptEngine::Functions {
    float cpp_DeltaTime() {
        return Timestep::deltaTime;
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions