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
    void SpriteRenderer_GetKey(MonoString *key, MonoString *id, MonoString **result);
    void SpriteRenderer_SetTexture(MonoString *texture, MonoString *id);
    void SpriteRenderer_SetColor(float x, float y, float z, MonoString *id);
} // namespace HyperAPI::CsharpScriptEngine::Functions