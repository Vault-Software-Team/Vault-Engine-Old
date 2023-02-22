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
    void BoxCollider2D_GetKey(MonoString *key, MonoString *id, MonoString **result);
    void BoxCollider2D_SetKey(MonoString *key, float value, MonoString *id);
    void BoxCollider2D_SetOffset(float x, float y, MonoString *id);
    void BoxCollider2D_SetSize(float x, float y, MonoString *id);
    bool BoxCollider2D_GetTrigger(MonoString *id);
    void BoxCollider2D_SetTrigger(MonoString *id, bool val);
} // namespace HyperAPI::CsharpScriptEngine::Functions