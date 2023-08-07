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
    void Discord_SetPresence(MonoString *mono_details,
                             MonoString *mono_state,
                             MonoString *mono_largeImageKey,
                             MonoString *mono_largeImageText,
                             MonoString *mono_smallImageKey,
                             MonoString *mono_smallImageText);

    void Discord_InitRPC(MonoString *client_id);
} // namespace HyperAPI::CsharpScriptEngine::Functions