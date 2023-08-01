#pragma once
#include <dllapi.hpp>
#include <libs.hpp>

namespace HyperAPI::Timestep {
    DLL_API extern float deltaTime;
    DLL_API extern float lastFrame;
    DLL_API extern float currentFrame;
} // namespace HyperAPI::Timestep