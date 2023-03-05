#include "CameraFunctions.hpp"
#include "csharp.hpp"
#include "mono/metadata/object.h"
#include <string>

namespace HyperAPI::CsharpScriptEngine::Functions {
    void Camera_GetKey(MonoString *key, MonoString *id, MonoString **result) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }
        auto &component = gameObject->GetComponent<CameraComponent>();

        if (m_key == "fov") {
            *result = mono_string_new(appDomain, std::to_string(component.camera->cam_fov).c_str());
        } else if (m_key == "near") {
            *result = mono_string_new(appDomain, std::to_string(component.camera->cam_near).c_str());
        } else if (m_key == "far") {
            *result = mono_string_new(appDomain, std::to_string(component.camera->cam_far).c_str());
        }
    }

    void Camera_SetKey(MonoString *key, float value, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }
        auto &component = gameObject->GetComponent<CameraComponent>();

        if (m_key == "fov") {
            component.camera->cam_fov = value;
        } else if (m_key == "near") {
            component.camera->cam_near = value;
        } else if (m_key == "far") {
            component.camera->cam_far = value;
        }
    }

    void Camera_Set2D(bool value, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }
        auto &component = gameObject->GetComponent<CameraComponent>();

        component.camera->mode2D = value;
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions