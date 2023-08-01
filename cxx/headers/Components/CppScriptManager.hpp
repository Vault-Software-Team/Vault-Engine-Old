#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Timestep.hpp"
#include "../Scripting/CXX/CppScripting.hpp"
#include "scene.hpp"

namespace HyperAPI::Experimental {
    struct DLL_API CppScriptManager : public BaseComponent {
        std::vector<CppScripting::Script *> addedScripts;
        bool showScripts;
        std::string id = uuid::generate_uuid_v4();

        void GUI();
        void DeleteComp() {
            for (auto *script : addedScripts) {
                delete script;
            }
            addedScripts.clear();
        }

        void Update() {
            using namespace CppScripting;
            if (Scene::stop_scripts)
                return;

            for (auto *script : addedScripts) {
                // HYPER_LOG(script);

                script->Update();
            }
        }

        void Start();
    };
} // namespace HyperAPI::Experimental