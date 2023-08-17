#pragma once
#include "MathfFunctions.hpp"

namespace HyperAPI::CsharpScriptEngine::Functions {
    float Deg2Rad(float value) {
        return glm::radians(value);
    }
    float Rad2Deg(float value) {
        return glm::degrees(value);
    }
    float Abs(float value) {
        return glm::abs(value);
    }
    float Acos(float value) {
        return glm::acos(value);
    }
    float Asin(float value) {
        return glm::asin(value);
    }
    float Atan(float value) {
        return glm::atan(value);
    }
    float Atan2(float x, float y) {
        return std::atan2(x, y);
    }
    float Ceil(float value) {
        return glm::ceil(value);
    }
    float Clamp(float value, float min, float max) {
        return std::clamp(value, min, max);
    }
    float Cos(float value) {
        return glm::cos(value);
    }
    float Sin(float value) {
        return glm::sin(value);
    }
    float Sqrt(float value) {
        return glm::sqrt(value);
    }
    float Tan(float value) {
        return glm::tan(value);
    }
    float Round(float value) {
        return glm::round(value);
    }
    float Pow(float x, float y) {
        return std::pow(x, y);
    }
    float Log(float value) {
        return std::log(value);
    }
    float Log10(float value) {
        return std::log10(value);
    }
    float Max(float x, float y) {
        return std::max(x, y);
    }
    float Min(float x, float y) {
        return std::min(x, y);
    }
    float Exp(float x) {
        return std::exp(x);
    }
    float Lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

} // namespace HyperAPI::CsharpScriptEngine::Functions