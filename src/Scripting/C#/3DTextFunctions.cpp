#include "3DTextFunctions.hpp"
#include "csharp.hpp"

namespace HyperAPI::CsharpScriptEngine::Functions {
    void Text3D_GetKey(MonoString *key, MonoString *id, MonoString **result) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        auto &component = gameObject->GetComponent<Text3D>();

        if (m_key == "text") {
            *result = mono_string_new(appDomain, component.text);
        } else if (m_key == "scale") {
            *result = mono_string_new(appDomain, std::to_string(component.scale).c_str());
        } else if (m_key == "y_offset") {
            *result = mono_string_new(appDomain, std::to_string(component.y_offset).c_str());
        }
    }
    void Text3D_SetText(MonoString *text, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_text = mono_string_to_utf8(text);
        const std::string m_id = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        auto &component = gameObject->GetComponent<Text3D>();
        strcpy(component.text, m_text.c_str());
    }
    void Text3D_SetOthers(float value, MonoString *key, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        auto &component = gameObject->GetComponent<Text3D>();

        if (m_key == "scale") {
            component.scale = value;
        } else if (m_key == "y_offset") {
            component.y_offset = value;
        }
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions