#pragma once
#include <dllapi.hpp>
#include <libs.hpp>

namespace HyperAPI::f_GameObject {
    DLL_API Experimental::GameObject *FindGameObjectByName(const std::string &name);
    DLL_API Experimental::GameObject *FindGameObjectByTag(const std::string &tag);
    DLL_API Experimental::GameObject *FindGameObjectByID(const std::string &id);
    DLL_API Experimental::GameObject *FindGameObjectByEntt(const entt::entity &entity);
    DLL_API Experimental::GameObject *InstantiatePrefab(const std::string &path);

    DLL_API Experimental::GameObject *
    InstantiateTransformPrefab(const std::string &path,
                               Vector3 position = Vector3(0, 0, 0),
                               Vector3 rotation = Vector3(0, 0, 0));
} // namespace HyperAPI::f_GameObject