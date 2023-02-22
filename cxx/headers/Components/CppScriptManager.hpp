#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Timestep.hpp"
#include "../Scripting/CXX/CppScripting.hpp"

namespace HyperAPI::Experimental {
    struct CppScriptManager : public BaseComponent {
        std::vector<CppScripting::Script *> addedScripts;
        bool showScripts;
        std::string id = uuid::generate_uuid_v4();

        CppScriptManager() = default;

        void GUI() {
            if (ImGui::TreeNode("C++ Script Manager")) {
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

                ImGui::TreePop();
            }
        }

        void DeleteComp() {
            for (auto *script : addedScripts) {
                delete script;
            }
            addedScripts.clear();
        }

        void Update() {
            using namespace CppScripting;

            for (auto *script : addedScripts) {
                script->Update();
            }
        }

        void Start() {
            using namespace CppScripting;

            for (auto *script : addedScripts) {
                script->objId = ID;
                script->Start();
            }
        }
    };
} // namespace HyperAPI::Experimental