#include "InputFunctions.hpp"
#include "InputEvents.hpp"
#include "csharp.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "mono/metadata/object.h"

namespace HyperAPI::CsharpScriptEngine::Functions {
    // Inputs
    bool Input_IsKeyPressed(int key) {
        return Input::IsKeyPressed(key);
    }

    bool Input_IsKeyReleased(int key) {
        return Input::IsKeyReleased(key);
    }

    bool Input_IsMouseButtonPressed(int button) {
        return Input::IsMouseButtonPressed(button);
    }

    bool Input_IsMouseButtonReleased(int button) {
        return Input::IsMouseButtonReleased(button);
    }

    int Input_GetHorizontalAxis() {
        return Input::GetHorizontalAxis();
    }

    int Input_GetVerticalAxis() {
        return Input::GetVerticalAxis();
    }

    int Input_GetMouseXAxis() {
        return Input::GetMouseXAxis();
    }

    int Input_GetMouseYAxis() {
        return Input::GetMouseYAxis();
    }

    void Input_SetMouseHidden(bool hidden) {
        Input::SetMouseHidden(hidden);
    }

    void Input_SetMousePosition(float x, float y) {
        Input::SetMousePosition(x, y);
    }

    void Input_GetMouseWorldPosition(MonoString **out) {
        *out = mono_string_new(CsharpVariables::appDomain, (std::to_string(Input::mouseRay.x) + " " + std::to_string(-Input::mouseRay.y) + " " + std::to_string(Input::mouseRay.z)).c_str());
    }

    float Input_atan2(float x, float y) {
        return atan2(x, y);
    }

    void Input_normalize(float x, float y, float z, MonoString **out) {
        glm::vec3 normalized = glm::normalize(glm::vec3(x, y, z));
        *out = mono_string_new(CsharpVariables::appDomain, (std::to_string(normalized.x) + " " + std::to_string(-normalized.y) + " " + std::to_string(normalized.z)).c_str());
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions