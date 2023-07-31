#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"

#include "GameObject.hpp"

namespace HyperAPI::Experimental {
    struct m_LuaScriptComponent : public BaseComponent {
        GameObject *m_GameObject;
        std::vector<ScriptEngine::m_LuaScript> scripts;

        m_LuaScriptComponent() {
            for (auto &gameObject : (*Scene::m_GameObjects)) {
                if (gameObject->ID == ID) {
                    m_GameObject = gameObject;
                    std::cout << "FOUND" << std::endl;
                    break;
                }
            }
        };

        void Start() {
            for (auto &script : scripts) {
                script.Init();
            }
        }

        void Update() {
            if (isRunning) {
                for (auto &script : scripts) {
                    script.Update();
                }
            }
        }

        bool isTyping = false;

        void GUI() {
            if (ImGui::TreeNode("Lua Scripts")) {
                for (int i = 0; i < scripts.size(); i++) {
                    if (ImGui::TreeNode(scripts[i].pathToScript.c_str())) {
                        if (!isRunning && !isTyping) {
                            scripts[i].GetFields();
                        }
                        for (auto &field : scripts[i].m_Fields) {
                            // inputtext but the label is on the left
                            // remove first three characters from
                            // field.first
                            std::string label = field.first;
                            label.erase(label.begin(), label.begin() + 3);
                            ImGui::Text("%s", label.c_str());
                            ImGui::SameLine();
                            ImGui::InputText(
                                std::string("##" + field.first).c_str(),
                                field.second.value, 1024);
                            // check if inputtext is typing
                            if (ImGui::IsItemActive()) {
                                isTyping = true;
                            } else {
                                isTyping = false;
                            }
                        }
                        ImGui::NewLine();
                        if (ImGui::Button(ICON_FA_TRASH " Delete")) {
                            scripts.erase(scripts.begin() + i);
                        }
                        ImGui::TreePop();
                    }
                }

                if (ImGui::Button(ICON_FA_PLUS " Add Script")) {
                    ImGuiFileDialog::Instance()->OpenDialog(
                        "ChooseScript", "Choose Script", ".lua", ".");
                }

                if (ImGuiFileDialog::Instance()->Display("ChooseScript")) {
                    // action if OK
                    if (ImGuiFileDialog::Instance()->IsOk()) {
                        std::string filePathName =
                            ImGuiFileDialog::Instance()->GetFilePathName();
                        // remove cwd from filePathName
                        filePathName.erase(0, cwd.length() + 1);
                        std::string filePath =
                            ImGuiFileDialog::Instance()->GetCurrentPath();

                        ScriptEngine::m_LuaScript script(filePathName);
                        script.m_GameObject = m_GameObject;
                        script.ID = ID;
                        // script.Init();
                        script.GetFields();
                        scripts.push_back(script);

                        HYPER_LOG(std::string("Added lua script: ") +
                                  filePathName);
                    }

                    ImGuiFileDialog::Instance()->Close();
                }

                ImGui::NewLine();
                if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                    Scene::m_Registry.remove<m_LuaScriptComponent>(entity);
                }

                ImGui::TreePop();
            }
        }
    };
} // namespace HyperAPI::Experimental