#ifndef VAULT_ENGINE_NATIVESCRIPTS_H
#define VAULT_ENGINE_NATIVESCRIPTS_H

#include <string>

#ifndef _WIN32
#include <dlfcn.h>
#endif

namespace HyperAPI {
    namespace NativeScriptEngine {
        void LoadScript(const std::string &scriptName);
    }
}

#endif
