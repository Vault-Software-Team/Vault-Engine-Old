#pragma once
#include <libs.hpp>

namespace HyperAPI::f_GameObject {
    Experimental::GameObject *FindGameObjectByName(const std::string &name);
    Experimental::GameObject *FindGameObjectByTag(const std::string &tag);
    Experimental::GameObject *FindGameObjectByID(const std::string &id);
    Experimental::GameObject *FindGameObjectByEntt(const entt::entity &entity);
    Experimental::GameObject *InstantiatePrefab(const std::string &path);

    Experimental::GameObject *
    InstantiateTransformPrefab(const std::string &path,
                               Vector3 position = Vector3(0, 0, 0),
                               Vector3 rotation = Vector3(0, 0, 0));
} // namespace HyperAPI::f_GameObject