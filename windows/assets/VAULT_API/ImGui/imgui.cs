using System;
using System.Runtime.CompilerServices;
using Vault;

namespace ImGui
{
    public enum ImGuiCol_
    {
        ImGuiCol_Text,
        ImGuiCol_TextDisabled,
        ImGuiCol_WindowBg,              // Background of normal windows
        ImGuiCol_ChildBg,               // Background of child windows
        ImGuiCol_PopupBg,               // Background of popups, menus, tooltips windows
        ImGuiCol_Border,
        ImGuiCol_BorderShadow,
        ImGuiCol_FrameBg,               // Background of checkbox, radio button, plot, slider, text input
        ImGuiCol_FrameBgHovered,
        ImGuiCol_FrameBgActive,
        ImGuiCol_TitleBg,
        ImGuiCol_TitleBgActive,
        ImGuiCol_TitleBgCollapsed,
        ImGuiCol_MenuBarBg,
        ImGuiCol_ScrollbarBg,
        ImGuiCol_ScrollbarGrab,
        ImGuiCol_ScrollbarGrabHovered,
        ImGuiCol_ScrollbarGrabActive,
        ImGuiCol_CheckMark,
        ImGuiCol_SliderGrab,
        ImGuiCol_SliderGrabActive,
        ImGuiCol_Button,
        ImGuiCol_ButtonHovered,
        ImGuiCol_ButtonActive,
        ImGuiCol_Header,                // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
        ImGuiCol_HeaderHovered,
        ImGuiCol_HeaderActive,
        ImGuiCol_Separator,
        ImGuiCol_SeparatorHovered,
        ImGuiCol_SeparatorActive,
        ImGuiCol_ResizeGrip,            // Resize grip in lower-right and lower-left corners of windows.
        ImGuiCol_ResizeGripHovered,
        ImGuiCol_ResizeGripActive,
        ImGuiCol_Tab,                   // TabItem in a TabBar
        ImGuiCol_TabHovered,
        ImGuiCol_TabActive,
        ImGuiCol_TabUnfocused,
        ImGuiCol_TabUnfocusedActive,
        ImGuiCol_DockingPreview,        // Preview overlay color when about to docking something
        ImGuiCol_DockingEmptyBg,        // Background color for empty node (e.g. CentralNode with no window docked into it)
        ImGuiCol_PlotLines,
        ImGuiCol_PlotLinesHovered,
        ImGuiCol_PlotHistogram,
        ImGuiCol_PlotHistogramHovered,
        ImGuiCol_TableHeaderBg,         // Table header background
        ImGuiCol_TableBorderStrong,     // Table outer and header borders (prefer using Alpha=1.0 here)
        ImGuiCol_TableBorderLight,      // Table inner borders (prefer using Alpha=1.0 here)
        ImGuiCol_TableRowBg,            // Table row background (even rows)
        ImGuiCol_TableRowBgAlt,         // Table row background (odd rows)
        ImGuiCol_TextSelectedBg,
        ImGuiCol_DragDropTarget,        // Rectangle highlighting a drop target
        ImGuiCol_NavHighlight,          // Gamepad/keyboard: current highlighted item
        ImGuiCol_NavWindowingHighlight, // Highlight window when using CTRL+TAB
        ImGuiCol_NavWindowingDimBg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
        ImGuiCol_ModalWindowDimBg,      // Darken/colorize entire screen behind a modal window, when one is active
        ImGuiCol_COUNT
    };

    public enum ImGuiStyleVar_
    {
        // Enum name --------------------- // Member in ImGuiStyle structure (see ImGuiStyle for descriptions)
        ImGuiStyleVar_Alpha,               // float     Alpha
        ImGuiStyleVar_DisabledAlpha,       // float     DisabledAlpha
        ImGuiStyleVar_WindowPadding,       // ImVec2    WindowPadding
        ImGuiStyleVar_WindowRounding,      // float     WindowRounding
        ImGuiStyleVar_WindowBorderSize,    // float     WindowBorderSize
        ImGuiStyleVar_WindowMinSize,       // ImVec2    WindowMinSize
        ImGuiStyleVar_WindowTitleAlign,    // ImVec2    WindowTitleAlign
        ImGuiStyleVar_ChildRounding,       // float     ChildRounding
        ImGuiStyleVar_ChildBorderSize,     // float     ChildBorderSize
        ImGuiStyleVar_PopupRounding,       // float     PopupRounding
        ImGuiStyleVar_PopupBorderSize,     // float     PopupBorderSize
        ImGuiStyleVar_FramePadding,        // ImVec2    FramePadding
        ImGuiStyleVar_FrameRounding,       // float     FrameRounding
        ImGuiStyleVar_FrameBorderSize,     // float     FrameBorderSize
        ImGuiStyleVar_ItemSpacing,         // ImVec2    ItemSpacing
        ImGuiStyleVar_ItemInnerSpacing,    // ImVec2    ItemInnerSpacing
        ImGuiStyleVar_IndentSpacing,       // float     IndentSpacing
        ImGuiStyleVar_CellPadding,         // ImVec2    CellPadding
        ImGuiStyleVar_ScrollbarSize,       // float     ScrollbarSize
        ImGuiStyleVar_ScrollbarRounding,   // float     ScrollbarRounding
        ImGuiStyleVar_GrabMinSize,         // float     GrabMinSize
        ImGuiStyleVar_GrabRounding,        // float     GrabRounding
        ImGuiStyleVar_TabRounding,         // float     TabRounding
        ImGuiStyleVar_ButtonTextAlign,     // ImVec2    ButtonTextAlign
        ImGuiStyleVar_SelectableTextAlign, // ImVec2    SelectableTextAlign
        ImGuiStyleVar_COUNT
    };

    public class Styles
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Push(ImGuiStyleVar_ var, float x, float y);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Pop(int count = 1);
    }

    public class Colors
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Push(ImGuiCol_ var, float r, float g, float b, float alpha);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Pop(int count = 1);
    }

    public class Window
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Begin(string label);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void End();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static bool IsWindowHovered();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static bool SetWindowPosition(float x, float y);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static bool SetWindowSize(float x, float y);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static string cpp_get_win_size();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static string cpp_get_win_pos();

        Vector2 GetWindowSize()
        {
            string str = cpp_get_win_size();
            string[] split = str.Split(" ");

            return new Vector2(float.Parse(split[0]), float.Parse(split[0]));
        }

        Vector2 GetWindowPosition()
        {
            string str = cpp_get_win_pos();
            string[] split = str.Split(" ");

            return new Vector2(float.Parse(split[0]), float.Parse(split[0]));
        }
    }

    public class WindowChild
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Begin(string label);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void End();
    }

    public class TreeNode
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Begin(string label);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void End();
    }

    public class GUI
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Text(string content);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void DragFloat(string label, ref float val, float step = 1, float min = 0, float max = 100, string format = "%.3f");

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void DragInt(string label, ref int val, int step = 1, int min = 0, int max = 100, string format = "%.3f");

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void NewLine();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Separator();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void EnableWindowDocking();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static bool Button(string label, float x, float y);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Image(string texture_path, float size_x, float size_y);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SameLine(float offset_from_start_x = 0.0f, float spacing = -1.0f);
        // implementations


        /*
        ImGui::DockSpaceOverViewport(
                ImGui::GetMainViewport(),
                ImGuiDockNodeFlags_PassthruCentralNode);
                */
    }
}