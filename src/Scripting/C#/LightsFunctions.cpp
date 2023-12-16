#pragma once
#include "LightsFunctions.hpp"
namespace HyperAPI::CsharpScriptEngine::Functions {
    void PointLight_GetKey(MonoString *key, MonoString *id, MonoString **result) {
        using namespace CsharpVariables;
        using namespace CsharpScriptEngine;

        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            *result = mono_string_new(CsharpVariables::appDomain, "0 0 0");
            return;
        }
        auto &component = gameObject->GetComponent<Experimental::c_PointLight>();
        if (m_key == "color") {
            *result = mono_string_new(CsharpVariables::appDomain, "0 0 0");
        }

        if (m_key == "intensity") {
            *result = mono_string_new(CsharpVariables::appDomain, (std::to_string(component.intensity) + " 0 0").c_str());
        }

        if (m_key == "shadows") {
            *result = mono_string_new(CsharpVariables::appDomain, component.light->renderShadows ? "true" : "false");
        }
    }
    void PointLight_SetKey(MonoString *key, MonoString *id, float x, float y, float z) {
        using namespace CsharpVariables;
        using namespace CsharpScriptEngine;

        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }
        auto &component = gameObject->GetComponent<Experimental::c_PointLight>();
        if (m_key == "color") {
            component.color.x = x;
            component.color.y = y;
            component.color.z = z;
        }

        if (m_key == "intensity") {
            component.intensity = x;
        }

        if (m_key == "shadows") {
            component.light->renderShadows = x == 1.0f ? true : false; // can just do x == 1.0f but just in case or some shit idk
        }
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions