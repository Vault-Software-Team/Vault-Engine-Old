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
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with name: " + m_name), LOG_ERROR);
            return mono_string_new(appDomain, "null");
        }

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
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with tag: " + m_tag), LOG_ERROR);
            return mono_string_new(appDomain, "null");
        }

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
        Scene::m_GameObjects->push_back(go);
        *m_id = mono_string_new(appDomain, go->ID.c_str());
    }

    void GameObject_RemoveGameObject(MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;
        const std::string m_id = mono_string_to_utf8(id);

        auto *go = f_GameObject::FindGameObjectByID(m_id);
        if (go) {
            go->schedule_deletion = true;
        }
    }

    MonoString *GameObject_InstantiatePrefab(MonoString *prefab_path) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string path = mono_string_to_utf8(prefab_path);
        PrefabSchedule prefab;
        prefab.id = uuid::generate_uuid_v4();
        prefab.name = "Prefab";
        prefab.path = path;

        schedule_prefab_spawn.push_back(prefab);
        return mono_string_new(appDomain, prefab.id.c_str());
        // Scene::LoadPrefab(path);
    }

    MonoString *GameObject_InstantiatePrefabWithProperties(MonoString *prefab_path, float px, float py, float pz, float rx, float ry, float rz, MonoString *parent_id, MonoString *object_name, MonoString *object_tag, MonoString *custom_uuid) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string path = mono_string_to_utf8(prefab_path);
        const std::string m_parent_id = mono_string_to_utf8(parent_id);
        const std::string m_object_name = mono_string_to_utf8(object_name);
        const std::string m_object_tag = mono_string_to_utf8(object_tag);
        const std::string m_custom_uuid = mono_string_to_utf8(custom_uuid);
        // auto *go = Scene::LoadPrefab(path);
        PrefabSchedule prefab;
        prefab.id = m_custom_uuid == "GENERATE_RANDOM" ? uuid::generate_uuid_v4() : m_custom_uuid;
        prefab.name = m_object_name;
        prefab.path = path;
        prefab.parent_id = m_parent_id;
        prefab.tag = m_object_tag;

        schedule_prefab_spawn.push_back(prefab);

        // go->parentID = m_parent_id;

        // auto &transform = go->GetComponent<Transform>();
        prefab.pos.x = px;
        prefab.pos.y = py;
        prefab.pos.z = pz;

        prefab.rot.x = rx;
        prefab.rot.y = ry;
        prefab.rot.z = rz;

        return mono_string_new(appDomain, prefab.id.c_str());
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions