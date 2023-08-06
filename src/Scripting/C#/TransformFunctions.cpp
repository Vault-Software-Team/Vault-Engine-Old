#include "TransformFunctions.hpp"
#include "csharp.hpp"
#include <exception>

namespace HyperAPI::CsharpScriptEngine::Functions {
    void Transform_GetKey(MonoString *key, MonoString *id, MonoString **result) {
        using namespace Experimental;
        if (!id)
            return;

        try {
            const std::string keyStr = mono_string_to_utf8(key);
            const std::string idStr = mono_string_to_utf8(id);

            auto *gameObject = f_GameObject::FindGameObjectByID(idStr);
            if (!gameObject) {
                Log log(("C#: Couldn't find game object with ID: " + idStr), LOG_ERROR);
                *result = mono_string_new(CsharpVariables::appDomain, "0, 0, 0");
                return;
            }
            auto &transform = gameObject->GetComponent<Transform>();

            if (keyStr == "position") {
                *result = mono_string_new(CsharpVariables::appDomain, (
                                                                          std::to_string(transform.position.x) + " " + std::to_string(transform.position.y) + " " + std::to_string(transform.position.z))
                                                                          .c_str());
            }

            if (keyStr == "rotation") {
                *result = mono_string_new(CsharpVariables::appDomain, (
                                                                          std::to_string(transform.rotation.x) + " " + std::to_string(transform.rotation.y) + " " + std::to_string(transform.rotation.z))
                                                                          .c_str());
            }

            if (keyStr == "scale") {
                *result = mono_string_new(CsharpVariables::appDomain, (
                                                                          std::to_string(transform.scale.x) + " " + std::to_string(transform.scale.y) + " " + std::to_string(transform.scale.z))
                                                                          .c_str());
            }

            if (keyStr == "forward") {
                *result = mono_string_new(CsharpVariables::appDomain, (
                                                                          std::to_string(transform.forward.x) + " " + std::to_string(transform.forward.y) + " " + std::to_string(transform.forward.z))
                                                                          .c_str());
            }

            if (keyStr == "up") {
                *result = mono_string_new(CsharpVariables::appDomain, (
                                                                          std::to_string(transform.up.x) + " " + std::to_string(transform.up.y) + " " + std::to_string(transform.up.z))
                                                                          .c_str());
            }

            if (keyStr == "right") {
                *result = mono_string_new(CsharpVariables::appDomain, (
                                                                          std::to_string(transform.right.x) + " " + std::to_string(transform.right.y) + " " + std::to_string(transform.right.z))
                                                                          .c_str());
            }
        } catch (std::exception &e) {
        }
    }

    void Transform_SetKey(MonoString *key, MonoString *id, float x, float y, float z) {
        using namespace Experimental;
        const std::string keyStr = mono_string_to_utf8(key);
        const std::string idStr = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(idStr);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + idStr), LOG_ERROR);
            return;
        }
        auto &transform = gameObject->GetComponent<Transform>();

        if (keyStr == "position") {
            transform.position.x = x;
            transform.position.y = y;
            transform.position.z = z;
        }

        if (keyStr == "rotation") {
            transform.rotation.x = x;
            transform.rotation.y = y;
            transform.rotation.z = z;
        }

        if (keyStr == "scale") {
            transform.scale.x = x;
            transform.scale.y = y;
            transform.scale.z = z;
        }
    }

    void Transform_LookAt(MonoString *id, float x, float y, float z) {
        using namespace Experimental;
        if (!id)
            return;

        try {
            const std::string idStr = mono_string_to_utf8(id);

            auto *gameObject = f_GameObject::FindGameObjectByID(idStr);
            if (!gameObject) {
                Log log(("C#: Couldn't find game object with ID: " + idStr), LOG_ERROR);
                return;
            }
            auto &transform = gameObject->GetComponent<Transform>();

            transform.LookAt(glm::vec3(x, y, z));
        } catch (std::exception &e) {
        };
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions