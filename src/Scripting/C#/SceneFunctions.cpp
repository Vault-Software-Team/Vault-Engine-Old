#include "3DTextFunctions.hpp"
#include "csharp.hpp"
#include "mono/metadata/object.h"
#include "scene.hpp"

namespace HyperAPI::CsharpScriptEngine::Functions {
    void Scene_LoadScene(MonoString *scene_path) {
        // sleep(1);
        const std::string scene = mono_string_to_utf8(scene_path);
        CsharpVariables::scene_schedule.scene_path = scene;
        CsharpVariables::scene_schedule.scheduled = true;
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions