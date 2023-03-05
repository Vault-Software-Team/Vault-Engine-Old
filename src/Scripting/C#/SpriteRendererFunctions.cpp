#include "SpriteRendererFunctions.hpp"
#include "csharp.hpp"
#include <sys/types.h>

namespace HyperAPI::CsharpScriptEngine::Functions {
    void SpriteRenderer_GetKey(MonoString *key, MonoString *id, MonoString **result) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }
        auto &component = gameObject->GetComponent<SpriteRenderer>();

        if (m_key == "texture") {
            *result = mono_string_new(appDomain, component.mesh->material.diffuse->texPath.c_str());
        } else if (m_key == "color") {
            *result = mono_string_new(appDomain,
                                      (
                                          std::to_string(component.mesh->material.baseColor.x) + " " +
                                          std::to_string(component.mesh->material.baseColor.y) + " " +
                                          std::to_string(component.mesh->material.baseColor.z))
                                          .c_str());
        }
    }

    void SpriteRenderer_SetTexture(MonoString *texture, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_texture = mono_string_to_utf8(texture);
        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }
        auto &component = gameObject->GetComponent<SpriteRenderer>();

        if (component.mesh->material.diffuse != nullptr)
            delete component.mesh->material.diffuse;

        component.mesh->material.diffuse = new Texture(m_texture.c_str(), 0, "texture_diffuse");
    }

    void SpriteRenderer_SetColor(float x, float y, float z, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }
        auto &component = gameObject->GetComponent<SpriteRenderer>();

        component.mesh->material.baseColor.x = x;
        component.mesh->material.baseColor.y = y;
        component.mesh->material.baseColor.z = z;
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions