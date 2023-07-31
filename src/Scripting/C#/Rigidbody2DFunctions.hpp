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
    void Rigidbody2D_GetKey(MonoString *key, MonoString *id, MonoString **result);
    void Rigidbody2D_SetVelocity(float x, float y, MonoString *id);
    void Rigidbody2D_SetAngularVelocity(float velocity, MonoString *id);
    void Rigidbody2D_SetPosition(float x, float y, MonoString *id);
    void Rigidbody2D_Force(float x, float y, MonoString *id);
    void Rigidbody2D_Torque(float torque, MonoString *id);
    void Rigidbody2D_SetType(int type, MonoString *id);
} // namespace HyperAPI::CsharpScriptEngine::Functions