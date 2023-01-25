#include "EntityFunctions.hpp"
#include "mono/metadata/object.h"

namespace HyperAPI::CsharpScriptEngine::Functions {
    void Entity_GetID(MonoString **result) {
        *result = mono_string_new(CsharpVariables::appDomain, nextId.c_str());
    }

    bool Entity_GetEnabled(MonoString *id) {
        using namespace Experimental;
        auto *gameObject = f_GameObject::FindGameObjectByID(mono_string_to_utf8(id));
        return gameObject->enabled;
    }

    void Entity_SetEnabled(MonoString *id, bool value) {
        using namespace Experimental;
        auto *gameObject = f_GameObject::FindGameObjectByID(mono_string_to_utf8(id));
        gameObject->SetActive(value);
    }

    void Entity_AddComponent(MonoString *id, MonoString *type) {
        using namespace Experimental;
        auto *gameObject = f_GameObject::FindGameObjectByID(mono_string_to_utf8(id));
        const std::string m_type = mono_string_to_utf8(type);

        // omfg this code is so fucking garbage, i fucking hate how theres no switch case for strings AAAAAAAAAAA
        if (m_type == "Text3D") {
            if (!gameObject->HasComponent<Text3D>())
                gameObject->AddComponent<Text3D>();
        } else if (m_type == "Bloom") {
            if (!gameObject->HasComponent<Bloom>())
                gameObject->AddComponent<Bloom>();
        } else if (m_type == "BoxCollider2D") {
            if (!gameObject->HasComponent<BoxCollider2D>())
                gameObject->AddComponent<BoxCollider2D>();
        } else if (m_type == "CameraComponent") {
            if (!gameObject->HasComponent<CameraComponent>())
                gameObject->AddComponent<CameraComponent>();
        } else if (m_type == "BoxCollider3D") {
            if (!gameObject->HasComponent<BoxCollider3D>())
                gameObject->AddComponent<BoxCollider3D>();
        } else if (m_type == "MeshCollider3D") {
            if (!gameObject->HasComponent<MeshCollider3D>())
                gameObject->AddComponent<MeshCollider3D>();
        } else if (m_type == "PointLight") {
            if (!gameObject->HasComponent<c_PointLight>())
                gameObject->AddComponent<c_PointLight>();
        } else if (m_type == "SpotLight") {
            if (!gameObject->HasComponent<c_SpotLight>())
                gameObject->AddComponent<c_SpotLight>();
        } else if (m_type == "DirectionalLight") {
            if (!gameObject->HasComponent<c_DirectionalLight>())
                gameObject->AddComponent<c_DirectionalLight>();
        } else if (m_type == "MeshRenderer") {
            if (!gameObject->HasComponent<MeshRenderer>())
                gameObject->AddComponent<MeshRenderer>();
        } else if (m_type == "Rigidbody2D") {
            if (!gameObject->HasComponent<Rigidbody2D>())
                gameObject->AddComponent<Rigidbody2D>();
        } else if (m_type == "Rigidbody3D") {
            if (!gameObject->HasComponent<Rigidbody3D>())
                gameObject->AddComponent<Rigidbody3D>();
        } else if (m_type == "SpriteAnimation") {
            if (!gameObject->HasComponent<SpriteAnimation>())
                gameObject->AddComponent<SpriteAnimation>();
        } else if (m_type == "SpriteRenderer") {
            if (!gameObject->HasComponent<SpriteRenderer>())
                gameObject->AddComponent<SpriteRenderer>();
        } else if (m_type == "SpritesheetAnimation") {
            if (!gameObject->HasComponent<c_SpritesheetAnimation>())
                gameObject->AddComponent<c_SpritesheetAnimation>();
        } else if (m_type == "SpritesheetRenderer") {
            if (!gameObject->HasComponent<SpritesheetRenderer>())
                gameObject->AddComponent<SpritesheetRenderer>();
        } else if (m_type == "Transform") {
            if (!gameObject->HasComponent<Transform>())
                gameObject->AddComponent<Transform>();
        }
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions