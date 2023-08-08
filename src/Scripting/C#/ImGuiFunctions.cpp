#include "ImGuiFunctions.hpp"
#include "csharp.hpp"
#include "mono/metadata/object.h"
#include "scene.hpp"
#include <api.hpp>

namespace HyperAPI::CsharpScriptEngine::Functions {
    void ImGui_WindowBegin(MonoString *label) {
        using namespace Experimental;
        using namespace CsharpVariables;

        ImGui::Begin(mono_string_to_utf8(label));
    }

    void ImGui_WindowEnd() {
        ImGui::End();
    }

    MonoString *ImGui_WindowGetSize() {
        ImVec2 win_size = ImGui::GetWindowSize();
        return mono_string_new(CsharpVariables::appDomain, std::string(std::to_string(win_size.x) + std::to_string(win_size.y)).c_str());
    }

    MonoString *ImGui_WindowGetPos() {
        ImVec2 win_pos = ImGui::GetWindowPos();
        return mono_string_new(CsharpVariables::appDomain, std::string(std::to_string(win_pos.x) + std::to_string(win_pos.y)).c_str());
    }

    bool ImGui_IsWindowHovered() {
        return ImGui::IsWindowHovered();
    }

    void ImGui_Text(MonoString *content) {
        ImGui::Text("%s", mono_string_to_utf8(content));
    }

    void ImGui_WindowChildBegin(MonoString *label) {
        ImGui::BeginChild(mono_string_to_utf8(label));
    }
    void ImGui_WindowChildEnd() {
        ImGui::EndChild();
    }

    void ImGui_TreeNodeBegin(MonoString *label) {
        ImGui::TreeNode(mono_string_to_utf8(label));
    }
    void ImGui_TreeNodeEnd() {
        ImGui::TreePop();
    }

    void ImGui_DragFloat(MonoString *label, float &val, float step, float min, float max, MonoString *format) {
        ImGui::DragFloat(mono_string_to_utf8(label), &val, step, min, max, mono_string_to_utf8(format));
    }
    void ImGui_DragInt(MonoString *label, int &val, int step, int min, int max, MonoString *format) {
        ImGui::DragInt(mono_string_to_utf8(label), &val, step, min, max, mono_string_to_utf8(format));
    }
    void ImGui_NewLine() {
        ImGui::NewLine();
    }
    void ImGui_Separator() {
        ImGui::Separator();
    }
    void ImGui_EnableWindowDocking() {
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    }
    bool ImGui_Button(MonoString *label, float x, float y) {
        ImGui::Button(mono_string_to_utf8(label), ImVec2(x, y));
    }
    void ImGui_Image(MonoString *texture_path, float size_x, float size_y) {
        m_Texture *texture = nullptr;
        for (auto *tex : HyperAPI::textures) {
            if (tex->texPath == std::string(mono_string_to_utf8(texture_path))) {
                texture = tex;
                break;
            }
        }

        if (texture == nullptr) {
            texture = (new Texture(mono_string_to_utf8(texture_path), 0, "texture_diffuse"))->tex;
        }

        ImGui::Image((void *)texture->ID, ImVec2(size_x, size_y));
    }
    void ImGui_SameLine(float offset_from_start_x, float spacing) {
        ImGui::SameLine(offset_from_start_x, spacing);
    }

    void ImGui_StylesPush(ImGuiStyleVar_ var, float x, float y) {
        ImGui::PushStyleVar(var, ImVec2(x, y));
    }
    void ImGui_StylesPop(int count) {
        ImGui::PopStyleVar(count);
    }

    // Styles Class
    void ImGui_ColorsPush(ImGuiCol_ var, float r, float g, float b, float alpha) {
        ImGui::PushStyleColor(var, ImVec4(r / 255, g / 255, b / 255, alpha / 255));
    }
    void ImGui_ColorsPop(int count) {
        ImGui::PopStyleColor(count);
    }

    void ImGui_SetCursorPos(float x, float y) {
        ImGui::SetCursorPos(ImVec2(x, y));
    }

    void ImGui_SetWindowPos(float x, float y) {
        ImGui::SetWindowPos(ImVec2(x, y));
    }

    void ImGui_SetWindowSize(float x, float y) {
        ImGui::SetWindowSize(ImVec2(x, y));
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions