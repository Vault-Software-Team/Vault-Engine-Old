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
    void Transform_GetKey(MonoString *key, MonoString *id, MonoString **result);
    void Transform_SetKey(MonoString *key, MonoString *id, float x, float y, float z);
    void Transform_LookAt(MonoString *id, float x, float y, float z);
} // namespace HyperAPI::CsharpScriptEngine::Functions