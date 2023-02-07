#include "Audio3DFunctions.hpp"
#include "csharp.hpp"

namespace HyperAPI::CsharpScriptEngine::Functions {
    void Audio3D_GetKey(MonoString *key, MonoString *id, MonoString **result) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        auto &component = gameObject->GetComponent<Audio3D>();

        if (m_key == "volume") {
            *result = mono_string_new(appDomain, std::to_string(component.src.volume).c_str());
        } else if (m_key == "pitch") {
            *result = mono_string_new(appDomain, std::to_string(component.src.pitch).c_str());
        } else if (m_key == "max_distance") {
            *result = mono_string_new(appDomain, std::to_string(component.src.max_distance).c_str());
        } else if (m_key == "velocity") {
            *result = mono_string_new(CsharpVariables::appDomain, (
                                                                      std::to_string(component.src.velocity.x) + " " +
                                                                      std::to_string(component.src.velocity.y) + " " +
                                                                      std::to_string(component.src.velocity.z))
                                                                      .c_str());
        }
    }
    void Audio3D_SetFloatKey(MonoString *key, MonoString *id, float val) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        auto &component = gameObject->GetComponent<Audio3D>();

        if (m_key == "volume") {
            component.src.volume = val;
        } else if (m_key == "pitch") {
            component.src.pitch = val;
        } else if (m_key == "max_distance") {
            component.src.max_distance = val;
        }
    }
    void Audio3D_SetVelocity(MonoString *id, float x, float y, float z) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        auto &component = gameObject->GetComponent<Audio3D>();

        component.src.velocity.x = x;
        component.src.velocity.y = y;
        component.src.velocity.z = z;
    }
    bool Audio3D_GetLoop(MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        auto &component = gameObject->GetComponent<Audio3D>();

        return component.src.loop;
    }
    void Audio3D_SetLoop(MonoString *id, bool val) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        auto &component = gameObject->GetComponent<Audio3D>();

        component.src.loop = val;
    }
    void Audio3D_Play(MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        auto &component = gameObject->GetComponent<Audio3D>();

        component.Play();
    }
    void Audio3D_Stop(MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        auto &component = gameObject->GetComponent<Audio3D>();

        component.Stop();
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions