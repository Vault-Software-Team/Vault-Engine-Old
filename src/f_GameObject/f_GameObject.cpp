#include "f_GameObject.hpp"
#include "../Components/Transform.hpp"
#include "../Components/GameObject.hpp"

namespace HyperAPI::f_GameObject {
    DLL_API Experimental::GameObject *FindGameObjectByName(const std::string &name) {
        for (auto &gameObject : *Scene::m_GameObjects) {
            if (gameObject->name == name) {
                return gameObject;
            }
        }

        return nullptr;
    }

    DLL_API Experimental::GameObject *FindGameObjectByTag(const std::string &tag) {
        for (auto &gameObject : *Scene::m_GameObjects) {
            if (gameObject->tag == tag) {
                return gameObject;
            }
        }

        return nullptr;
    }

    DLL_API Experimental::GameObject *FindGameObjectByID(const std::string &id) {
        for (auto &gameObject : *Scene::m_GameObjects) {
            if (gameObject->ID == id) {
                return gameObject;
            }
        }

        return nullptr;
    }

    DLL_API Experimental::GameObject *FindGameObjectByEntt(const entt::entity &entity) {
        for (auto &gameObject : *Scene::m_GameObjects) {
            if (gameObject->entity == entity) {
                return gameObject;
            }
        }

        return nullptr;
    }

    DLL_API Experimental::GameObject *InstantiatePrefab(const std::string &path) {
        return Scene::LoadPrefab(path);
    }

    DLL_API Experimental::GameObject *
    InstantiatePrefab(const std::string &path,
                      Vector3 position = Vector3(0, 0, 0),
                      Vector3 rotation = Vector3(0, 0, 0)) {
        auto gameObject = Scene::LoadPrefab(path);
        gameObject->GetComponent<Experimental::Transform>().position = position;
        gameObject->GetComponent<Experimental::Transform>().rotation = rotation;

        return gameObject;
    }
} // namespace HyperAPI::f_GameObject