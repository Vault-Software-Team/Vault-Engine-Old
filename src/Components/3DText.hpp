#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"
#include "imgui/imgui.h"
#include "../Renderer/Font.hpp"
#include "Bloom.hpp"

namespace HyperAPI::Experimental {
    struct DLL_API Text3D : public BaseComponent {
        char text[999] = "Hello, World!";
        Font *font = new Font("assets/fonts/OpenSans-Bold.ttf", 48);
        Vector3 color = Vector3(1, 1, 1);
        Vector3 bloomColor;
        float scale = 0.01;
        float y_offset = 0.02;

        Text3D() = default;

        void DeleteComp() override {
            for (auto &c : font->Characters) {
                glDeleteTextures(1, &c.second.TextureID);
            }

            delete font;
        }

        void GUI() override {
            if (ImGui::TreeNode("3D Text")) {
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
                ImGui::TreePop();
            }
        }

        void Update() {
            if (Scene::m_Registry.has<Bloom>(entity)) {
                auto &bloom = Scene::m_Registry.get<Bloom>(entity);
                bloomColor = bloom.bloomColor;
            } else {
                bloomColor = Vector3(0, 0, 0);
            }
        }
    };
} // namespace HyperAPI::Experimental