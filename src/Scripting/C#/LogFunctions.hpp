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
    void NativeLog(MonoString *text);
    void EditorLog(MonoString *text);
    void EditorWarning(MonoString *text);
    void EditorError(MonoString *text);
    void TestVector(glm::vec3 *parameter);
} // namespace HyperAPI::CsharpScriptEngine::Functions