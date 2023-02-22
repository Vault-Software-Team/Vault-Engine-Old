#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Timestep.hpp"
#include "csharp.hpp"
#include "imgui/imgui.h"
#include "mono/metadata/exception.h"
#include "mono/metadata/object-forward.h"
#include "mono/metadata/object.h"

namespace HyperAPI::Experimental {
    struct CsharpScriptManager : public BaseComponent {
        std::unordered_map<std::string, std::string> selectedScripts;
        std::unordered_map<std::string, MonoScriptClass *> behaviours;

        CsharpScriptManager() = default;

        void GUI() {
            if (ImGui::TreeNode("C# Script Manager")) {
                ImGui::ListBoxHeader("C# Scripts");

                for (auto klass : CsharpScriptEngine::entityClasses) {
                    if (ImGui::Selectable(selectedScripts[klass.first] ==
                                                  klass.first
                                              ? (std::string(ICON_FA_CHECK) +
                                                 " " + klass.first)
                                                    .c_str()
                                              : klass.first.c_str(),
                                          false)) {
                        if (selectedScripts[klass.first] == klass.first) {
                            selectedScripts.erase(klass.first);
                        } else {
                            selectedScripts[klass.first] = klass.first;
                        }
                    }
                }
                ImGui::ListBoxFooter();

                ImGui::NewLine();
                if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                    Scene::m_Registry.remove<CsharpScriptManager>(entity);
                }

                ImGui::TreePop();
            }
        }

        void Start() {
            for (auto klass : selectedScripts) {
                MonoObject *exception = nullptr;
                std::istringstream iss(klass.first);
                std::vector<std::string> tokens;
                std::string token;
                while (std::getline(iss, token, '.')) {
                    if (!token.empty())
                        tokens.push_back(token);
                }

                MonoScriptClass *behaviour =
                    new MonoScriptClass(tokens[0], tokens[1]);
                behaviours[klass.first] = behaviour;
                behaviour->CallConstructor();
                CsharpScriptEngine::nextId = ID;
                void *params[0] = {};
                MonoMethod *onStart = behaviour->GetMethod("OnStart", 0);

                // void *iterator = nullptr;
                // while(MonoClassField *field =
                // mono_class_get_fields(behaviour->GetClass(), &iterator))
                // {
                //     const char *name = mono_field_get_name(field);
                //     const char *type =
                //     mono_type_get_name(mono_field_get_type(field));

                //     std::cout << "Name: " << name << " Type: " << type <<
                //     std::endl;
                // }

                mono_runtime_invoke(onStart, behaviour->f_GetObject(), params, &exception);
                CsharpScriptEngine::instances[klass.first] = behaviour;
            }
        }

        void Update() {
            for (auto klass : selectedScripts) {
                MonoObject *exception = nullptr;
                MonoScriptClass *behaviour =
                    CsharpScriptEngine::instances[klass.first];
                MonoMethod *onUpdate = behaviour->GetMethod("OnUpdate", 1);

                void *params = &Timestep::deltaTime;
                // TODO: Get the exception if theres one and print it in the editor console, mono_runtime_invoke(onUpdate, behaviour->f_GetObject(), &params, &exception);
                mono_runtime_invoke(onUpdate, behaviour->f_GetObject(), &params, nullptr);
            }
        }
    };
} // namespace HyperAPI::Experimental