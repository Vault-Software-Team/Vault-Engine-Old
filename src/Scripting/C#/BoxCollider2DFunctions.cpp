#include "BoxCollider2DFunctions.hpp"
#include "csharp.hpp"
#include "mono/metadata/object.h"

namespace HyperAPI::CsharpScriptEngine::Functions {
    void BoxCollider2D_GetKey(MonoString *key, MonoString *id, MonoString **result) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        auto &component = gameObject->GetComponent<BoxCollider2D>();

        if (m_key == "density") {
            *result = mono_string_new(appDomain, std::to_string(component.density).c_str());
        } else if (m_key == "friction") {
            *result = mono_string_new(appDomain, std::to_string(component.friction).c_str());
        } else if (m_key == "restitution") {
            *result = mono_string_new(appDomain, std::to_string(component.restitution).c_str());
        } else if (m_key == "restitutionThreshold") {
            *result = mono_string_new(appDomain, std::to_string(component.restitutionThreshold).c_str());
        }
    }

    void BoxCollider2D_SetKey(MonoString *key, float value, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        auto &component = gameObject->GetComponent<BoxCollider2D>();

        if (m_key == "density") {
            component.density = value;
        } else if (m_key == "friction") {
            component.friction = value;
        } else if (m_key == "restitution") {
            component.restitution = value;
        } else if (m_key == "restitutionThreshold") {
            component.restitutionThreshold = value;
        }
    }

    void BoxCollider2D_SetOffset(float x, float y, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        auto &component = gameObject->GetComponent<BoxCollider2D>();

        component.offset.x = x;
        component.offset.y = y;
    }

    void BoxCollider2D_SetSize(float x, float y, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        auto &component = gameObject->GetComponent<BoxCollider2D>();

        component.size.x = x;
        component.size.y = y;
    }

    bool BoxCollider2D_GetTrigger(MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        auto &component = gameObject->GetComponent<BoxCollider2D>();

        return component.trigger;
    }

    void BoxCollider2D_SetTrigger(MonoString *id, bool val) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        auto &component = gameObject->GetComponent<BoxCollider2D>();
        component.trigger = val;
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions