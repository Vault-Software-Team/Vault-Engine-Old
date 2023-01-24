#pragma once
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
    void Camera_GetKey(MonoString *key, MonoString *id, MonoString **result);
    void Camera_SetKey(MonoString *key, float value, MonoString *id);
    void Camera_Set2D(bool value, MonoString *id);
} // namespace HyperAPI::CsharpScriptEngine::Functions