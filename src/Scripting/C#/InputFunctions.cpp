#include "InputFunctions.hpp"

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
} // namespace HyperAPI::CsharpScriptEngine::Functions