#include "GameObjectFunctions.hpp"
#include "csharp.hpp"

namespace HyperAPI::CsharpScriptEngine::Functions {
    MonoString *GameObject_GetIDByName(MonoString *name) {
        using namespace Experimental;
        using namespace CsharpVariables;
        const std::string m_name = mono_string_to_utf8(name);

        auto *gameObject = f_GameObject::FindGameObjectByName(m_name);

        if (gameObject != nullptr) {
            return mono_string_new(appDomain, gameObject->ID.c_str());
        } else {
            return mono_string_new(appDomain, "null");
        }
    }

    MonoString *GameObject_GetIDByTag(MonoString *tag) {
        using namespace Experimental;
        using namespace CsharpVariables;
        const std::string m_tag = mono_string_to_utf8(tag);

        auto *gameObject = f_GameObject::FindGameObjectByTag(m_tag);

        if (gameObject != nullptr) {
            return mono_string_new(appDomain, gameObject->ID.c_str());
        } else {
            return mono_string_new(appDomain, "null");
        }
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions