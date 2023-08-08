#pragma once
#include <dllapi.hpp>
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
    // Window Class
    void ImGui_WindowBegin(MonoString *label);
    void ImGui_WindowEnd();
    MonoString *ImGui_WindowGetSize();
    MonoString *ImGui_WindowGetPos();
    bool ImGui_IsWindowHovered();
    void ImGui_SetWindowPos(float x, float y);
    void ImGui_SetWindowSize(float x, float y);

    // WindowChild Class
    void ImGui_WindowChildBegin(MonoString *label);
    void ImGui_WindowChildEnd();

    // TreeNode Class
    void ImGui_TreeNodeBegin(MonoString *label);
    void ImGui_TreeNodeEnd();

    // GUI Class
    void ImGui_Text(MonoString *content);
    void ImGui_DragFloat(MonoString *label, float &val, float step, float min, float max, MonoString *format);
    void ImGui_DragInt(MonoString *label, int &val, int step, int min, int max, MonoString *format);
    void ImGui_NewLine();
    void ImGui_Separator();
    void ImGui_EnableWindowDocking();
    bool ImGui_Button(MonoString *label, float x, float y);
    void ImGui_Image(MonoString *texture_path, float size_x, float size_y);
    void ImGui_SameLine(float offset_from_start_x, float spacing);
    void ImGui_SetCursorPos(float x, float y);

    // Styles Class
    void ImGui_StylesPush(ImGuiStyleVar_ var, float x, float y);
    void ImGui_StylesPop(int count);

    // Styles Class
    void ImGui_ColorsPush(ImGuiCol_ var, float r, float g, float b, float alpha);
    void ImGui_ColorsPop(int count);
} // namespace HyperAPI::CsharpScriptEngine::Functions