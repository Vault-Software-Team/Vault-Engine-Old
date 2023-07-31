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
    void Rigidbody3D_SetPosition(float x, float y, float z, MonoString *id);
    void Rigidbody3D_AddForce(float x, float y, float z, MonoString *id);
    void Rigidbody3D_AddTorque(float x, float y, float z, MonoString *id);
    void Rigidbody3D_AddForceAtPosition(float x, float y, float z, float px, float py, float pz, MonoString *id);
    void Rigidbody3D_SetVelocity(float x, float y, float z, MonoString *id);
    void Rigidbody3D_SetAngularVelocity(float x, float y, float z, MonoString *id);
} // namespace HyperAPI::CsharpScriptEngine::Functions