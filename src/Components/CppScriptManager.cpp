#pragma once
#include "CppScriptManager.hpp"
#include "GameObject.hpp"
#include "libs.hpp"
#include "scene.hpp"
#include "../f_GameObject/f_GameObject.hpp"

namespace HyperAPI::Experimental {
    void CppScriptManager::Start() {
        using namespace CppScripting;
        if (Scene::stop_scripts)
            return;

        for (auto *script : addedScripts) {
            script->objId = ID;
            script->Start();
        }
    }

    void CppScriptManager::GUI() {
        if (ImGui::CollapsingHeader("C++ Script Manager")) {
            ImGui::ListBoxHeader("Scripts");
            for (int i = 0; i < CppScripting::cpp_scripts.size(); i++) {
                auto item = CppScripting::cpp_scripts[i];
                bool addedToScripts = false;
                int index = -1;
                for (int j = 0; j < addedScripts.size(); j++) {
                    if (addedScripts[j]->name == item.name) {
                        addedToScripts = true;
                        index = j;
                        break;
                    }
                }

                if (addedToScripts) {
                    if (ImGui::Selectable(std::string(ICON_FA_CHECK +
                                                      std::string(" ") +
                                                      item.name)
                                              .c_str())) {
                        addedScripts.erase(addedScripts.begin() + index);
                    }
                } else {
                    if (ImGui::Selectable(item.name.c_str())) {
                        CppScripting::Script *script = item.create();
                        addedScripts.push_back(script);
                        addedScripts[addedScripts.size() - 1]->name =
                            item.name;
                    }
                }

                /*
                if(ImGui::Selectable(
                    addedToScripts ? std::string(ICON_FA_CHECK +
                std::string(" ") + item.name).c_str() : item.name.c_str()
                )) {
                    if(addedToScripts) {
                        addedScripts.erase(addedScripts.begin() + index);
                    } else {
                        CppScripting::Script *script = item.create();
                        script->name = item.name;
                        addedScripts.push_back(script);
                    }
                } */
            }
            ImGui::ListBoxFooter();

            ImGui::NewLine();
            if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                Scene::m_Registry.remove<CppScriptManager>(entity);
            }
        }
    }
} // namespace HyperAPI::Experimental