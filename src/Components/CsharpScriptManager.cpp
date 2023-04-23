#pragma once
#include "CsharpScriptManager.hpp"
#include "GameObject.hpp"
#include "libs.hpp"
#include "scene.hpp"
#include "../f_GameObject/f_GameObject.hpp"

namespace HyperAPI::Experimental {
    void CsharpScriptManager::Update() {
        GameObject *g = f_GameObject::FindGameObjectByID(ID);
        if (Scene::stop_scripts)
            return;
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

    void CsharpScriptManager::DeleteComp() {
        selectedScripts.clear();
        HYPER_LOG("CLEARED");
    }
} // namespace HyperAPI::Experimental