#pragma once
// OUTDATED (still usable), just use C++ Scripting (that is still in works)
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"

#include "GameObject.hpp"

namespace HyperAPI::Experimental {
    struct NativeScriptManager : public BaseComponent {
        std::vector<StaticScript *> m_StaticScripts;
        GameObject *gameObject;

        NativeScriptManager() = default;
        ~NativeScriptManager() {
            for (auto &script : m_StaticScripts) {
                delete script;
            }
        }

        void Init() {
            for (auto &gameObject : Scene::m_GameObjects) {
                if (gameObject->ID == ID) {
                    this->gameObject = gameObject;
                    break;
                }
            }
        }

        template <typename T> void AddScript() {
            m_StaticScripts.push_back(std::make_shared<T>());
            m_StaticScripts.back()->gameObject = gameObject;
            HYPER_LOG(std::string("Added script: ") + typeid(T).name());
        }

        void Start() {
            for (auto script : m_StaticScripts) {
                script->OnStart();
            }
        }

        void Update() {
            for (auto script : m_StaticScripts) {
                script->OnUpdate();
            }
        }
    };
} // namespace HyperAPI::Experimental