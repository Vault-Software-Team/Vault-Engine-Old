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
    void Audio3D_GetKey(MonoString *key, MonoString *id, MonoString **result);
    void Audio3D_SetFloatKey(MonoString *key, MonoString *id, float val);
    void Audio3D_SetVelocity(MonoString *id, float x, float y, float z);
    bool Audio3D_GetLoop(MonoString *id);
    void Audio3D_SetLoop(MonoString *id, bool val);
    void Audio3D_Play(MonoString *id);
    void Audio3D_Stop(MonoString *id);

    void Audio3D_SetClip(MonoString *id, MonoString *path);
    void Audio3D_GetClip(MonoString *id, MonoString **result);
} // namespace HyperAPI::CsharpScriptEngine::Functions