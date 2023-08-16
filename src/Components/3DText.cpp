#pragma once
#include "3DText.hpp"

namespace HyperAPI::Experimental {
    void Text3D::GUI() {
        if (ImGui::CollapsingHeader("3D Text")) {
            ImGui::InputTextMultiline("Text", text, 999);
            ImGui::ColorEdit3("Color", &color.r);
            ImGui::DragFloat("Scale", &scale, 0.001f, 0);
            ImGui::DragFloat("Y Offset", &y_offset, 0.01f, 0);

            ImGui::Selectable(font != nullptr ? ("Loaded Font: " + font->font_path).c_str() : "Drag Font Here", false);
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload =
                        ImGui::AcceptDragDropPayload("file")) {
                    if (
                        G_END_WITH(dirPayloadData, ".ttf") ||
                        G_END_WITH(dirPayloadData, ".ttc") ||
                        G_END_WITH(dirPayloadData, ".cff") ||
                        G_END_WITH(dirPayloadData, ".woff") ||
                        G_END_WITH(dirPayloadData, ".otf")) {
                        if (font != nullptr)
                            delete font;

                        font = new Font(dirPayloadData.c_str(), 48);
                    }
                }
            }

            ImGui::NewLine();
            if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                Scene::m_Registry.remove<Text3D>(entity);
            }
        }
    }
} // namespace HyperAPI::Experimental