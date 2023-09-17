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
    float cpp_DeltaTime();
    float cpp_Ambient(bool toGet, float value);
    int cpp_RandomInt(int min, int max);
    void ExitProgram();
} // namespace HyperAPI::CsharpScriptEngine::Functions