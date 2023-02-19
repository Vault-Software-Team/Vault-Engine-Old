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
    void Entity_GetID(MonoString **result);
    void Entity_GetDataFromID(MonoString *id, MonoString **result);
    bool Entity_GetEnabled(MonoString *id);
    void Entity_SetEnabled(MonoString *id, bool value);
    void Entity_AddComponent(MonoString *id, MonoString *type);
} // namespace HyperAPI::CsharpScriptEngine::Functions