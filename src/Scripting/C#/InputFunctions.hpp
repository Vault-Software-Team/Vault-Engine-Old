#pragma once
#include <api.hpp>
#include "mono/metadata/appdomain.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/class.h"
#include "mono/metadata/image.h"
#include "mono/metadata/loader.h"
#include "mono/metadata/object-forward.h"
#include "mono/metadata/object.h"
#include <cstddef>
#include <cstring>
#include <experimental/bits/fs_ops.h>
#include <functional>
#include <sstream>
#include <string>

namespace HyperAPI::CsharpScriptEngine::Functions {
    bool Input_IsKeyPressed(int key);
    bool Input_IsKeyReleased(int key);
    bool Input_IsMouseButtonPressed(int button);
    bool Input_IsMouseButtonReleased(int button);
    int Input_GetHorizontalAxis();
    int Input_GetVerticalAxis();
    int Input_GetMouseXAxis();
    int Input_GetMouseYAxis();
    void Input_SetMouseHidden(bool hidden);
    void Input_SetMousePosition(float x, float y);
} // namespace HyperAPI::CsharpScriptEngine::Functions