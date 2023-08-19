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
    MonoString *GameObject_GetIDByName(MonoString *name);
    MonoString *GameObject_GetIDByTag(MonoString *tag);
    void GameObject_AddGameObject(MonoString *name, MonoString *tag, MonoString **m_id);
    void GameObject_RemoveGameObject(MonoString *id);
    MonoString *GameObject_InstantiatePrefab(MonoString *prefab_path);
    MonoString *GameObject_InstantiatePrefabWithProperties(MonoString *prefab_path, float px, float py, float pz, float rx, float ry, float rz, MonoString *parent_id, MonoString *object_name, MonoString *object_tag, MonoString *custom_uuid);
} // namespace HyperAPI::CsharpScriptEngine::Functions