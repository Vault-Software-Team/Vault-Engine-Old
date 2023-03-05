#include "BloomFunctions.hpp"
#include "csharp.hpp"
#include "mono/metadata/object.h"
#include <string>
#include <sys/types.h>

namespace HyperAPI::CsharpScriptEngine::Functions {
    void Bloom_GetColor(MonoString *id, MonoString **result) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }

        auto &component = gameObject->GetComponent<Bloom>();

        *result = mono_string_new(appDomain, (std::to_string(component.bloomColor.x) + std::to_string(component.bloomColor.y) + std::to_string(component.bloomColor.z)).c_str());
    }

    void Bloom_SetColor(float x, float y, float z, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }

        auto &component = gameObject->GetComponent<Bloom>();

        component.bloomColor.x = x;
        component.bloomColor.y = y;
        component.bloomColor.z = z;
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions