#pragma once
#include "CsharpScriptManager.hpp"

namespace HyperAPI::Experimental {
    void CsharpScriptManager::Update() {
        for (auto klass : selectedScripts) {
            MonoObject *exception = nullptr;
            MonoScriptClass *behaviour =
                CsharpScriptEngine::instances[klass.first];
            MonoMethod *onUpdate = behaviour->GetMethod("OnUpdate", 0);

            void *params[0] = {};
            // TODO: Get the exception if theres one and print it in the editor console, mono_runtime_invoke(onUpdate, behaviour->f_GetObject(), &params, &exception);
            mono_runtime_invoke(onUpdate, behaviour->f_GetObject(), params, &exception);
        }
    }
} // namespace HyperAPI::Experimental