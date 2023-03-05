#include "SpriteAnimationFunctions.hpp"
#include "csharp.hpp"
#include <sys/types.h>

namespace HyperAPI::CsharpScriptEngine::Functions {
    void SpriteAnimation_GetKey(MonoString *key, MonoString *id, MonoString **result) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }
        auto &component = gameObject->GetComponent<SpriteAnimation>();

        if (m_key == "currentAnimation") {
            *result = mono_string_new(appDomain, component.currAnim);
        }
    }

    void SpriteAnimation_SetCurrAnimation(MonoString *anim, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_anim = mono_string_to_utf8(anim);
        const std::string m_id = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }
        auto &component = gameObject->GetComponent<SpriteAnimation>();

        strcpy(component.currAnim, m_anim.c_str());
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions