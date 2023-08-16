#pragma once
#include <lemon/Lemon.hpp>

namespace HyperAPI {
    void LogInfo(const FunctionCallbackInfo<Value> &args);
    void LogError(const FunctionCallbackInfo<Value> &args);
    void LogWarning(const FunctionCallbackInfo<Value> &args);
} // namespace HyperAPI