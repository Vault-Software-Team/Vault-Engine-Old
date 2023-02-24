#include "GameObjectFunctions.hpp"
#include "csharp.hpp"
#include "mono/metadata/object.h"
#include "scene.hpp"

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

    void GameObject_AddGameObject(MonoString *name, MonoString *tag, MonoString **m_id) {
        using namespace Experimental;
        using namespace CsharpVariables;
        const std::string m_name = mono_string_to_utf8(name);
        const std::string m_tag = mono_string_to_utf8(tag);

        GameObject *go = new GameObject();
        go->AddComponent<Transform>();
        go->name = m_name;
        go->tag = m_tag;
        Scene::m_GameObjects.push_back(go);
        *m_id = mono_string_new(appDomain, go->ID.c_str());
    }

    void GameObject_RemoveGameObject(MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;
        const std::string m_id = mono_string_to_utf8(id);

        auto *go = f_GameObject::FindGameObjectByID(m_id);
        if (go) {
            go->DeleteGameObject();
        }
    }

    void GameObject_InstantiatePrefab(MonoString *prefab_path) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string path = mono_string_to_utf8(prefab_path);
        Scene::LoadPrefab(path);
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions