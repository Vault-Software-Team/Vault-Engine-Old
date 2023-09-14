#pragma once
#include <dllapi.hpp>
#include <api.hpp>
#include "mono/metadata/appdomain.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/class.h"
#include "mono/metadata/image.h"
#include "mono/metadata/loader.h"
#include "mono/metadata/object-forward.h"
#include "mono/metadata/object.h"
#include <cstddef>
#include <cstring>
#include <experimental/bits/fs_ops.h>
#include <functional>
#include <sstream>
#include <string>

namespace HyperAPI::CsharpScriptEngine::Functions {
    float Deg2Rad(float value);
    float Rad2Deg(float value);
    float Abs(float value);
    float Acos(float value);
    float Asin(float value);
    float Atan(float value);
    float Atan2(float x, float y);
    float Ceil(float value);
    float Clamp(float value, float max, float min);
    float Cos(float value);
    float Sin(float value);
    float Sqrt(float value);
    float Tan(float value);
    float Round(float value);
    float Pow(float x, float y);
    float Log(float value);
    float Log10(float value);
    float Max(float x, float y);
    float Min(float x, float y);
    float Exp(float x);
    float Lerp(float a, float b, float t);
    int RandomRange(int min, int max);
} // namespace HyperAPI::CsharpScriptEngine::Functions