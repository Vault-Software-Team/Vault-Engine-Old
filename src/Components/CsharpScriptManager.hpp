#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Timestep.hpp"
#include "csharp.hpp"
#include "imgui/imgui.h"
#include "mono/metadata/exception.h"
#include "mono/metadata/object-forward.h"
#include "mono/metadata/object.h"
#include "../Renderer/Log.hpp"

namespace HyperAPI::Experimental {
    struct DLL_API CsharpScriptManager : public BaseComponent {
        std::unordered_map<std::string, std::string> selectedScripts;
        std::unordered_map<std::string, MonoScriptClass *> behaviours;

        CsharpScriptManager() = default;

        void GUI();

        void Start();
        void Update();

        void DeleteComp() override;
    };
} // namespace HyperAPI::Experimental