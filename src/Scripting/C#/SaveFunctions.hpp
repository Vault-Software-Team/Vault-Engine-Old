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
    MonoString *Save_GetVariable(MonoString *ms_save_file, MonoString *ms_variable_name);
    bool Save_SaveVariable(MonoString *ms_save_file, MonoString *ms_variable_name, MonoString *ms_value);
    bool Save_DeleteVariable(MonoString *ms_save_file, MonoString *ms_variable_name);
} // namespace HyperAPI::CsharpScriptEngine::Functions