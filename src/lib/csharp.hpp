#pragma once
#include <dllapi.hpp>
#include "mono/utils/mono-forward.h"
#include <functional>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/exception.h>
#include <mono/metadata/threads.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <glm/ext.hpp>

namespace CsharpVariables {
    DLL_API extern MonoDomain *rootDomain;
    DLL_API extern MonoDomain *appDomain;
    DLL_API extern MonoAssembly *coreAssembly;
    DLL_API extern bool compiledAssembly;
    DLL_API extern std::string oldCwd;

    struct SceneSchedule {
        std::string scene_path = "";
        bool scheduled = false;
    };

    DLL_API extern SceneSchedule scene_schedule;

    struct PrefabSchedule {
        std::string path = "";
        std::string name = "Prefab";
        std::string tag = "Untagged";
        std::string parent_id = "NO_PARENT";
        std::string id;
        glm::vec3 pos = glm::vec3(0, 0, 0);
        glm::vec3 rot = glm::vec3(0, 0, 0);
    };

    DLL_API extern std::vector<PrefabSchedule> schedule_prefab_spawn;
} // namespace CsharpVariables

class DLL_API MonoScriptClass {
private:
    MonoClass *klass;
    MonoObject *instance;
    uint32_t gc_handle;
    MonoObject *gc_instance;

public:
    std::string name_space, name;

    MonoScriptClass(const std::string &nameSpace, const std::string &name);
    void CallConstructor();

    MonoClass *GetClass() const;
    MonoMethod *GetMethod(const std::string method, int parameterCount) const;
    MonoObject *f_GetObject() const;
    MonoObject *f_GetObjectGC() const;
};

namespace HyperAPI {
    namespace CsharpScriptEngine {
        DLL_API extern std::unordered_map<std::string, MonoScriptClass *> entityClasses;
        DLL_API extern std::unordered_map<std::string, MonoScriptClass *> instances;
        DLL_API extern std::string nextId;

        namespace Functions {
            void NativeLog(MonoString *text);

            void EditorLog(MonoString *text);
            void EditorWarning(MonoString *text);
            void EditorError(MonoString *text);

            void RegisterFunctions();
        } // namespace Functions

        char *ReadBytes(const std::string &filepath, uint32_t *outSize);
        MonoAssembly *LoadCSharpAssembly(const std::string &assemblyPath);
        void PrintAssemblyTypes(MonoAssembly *assembly);
        void InitMono();
        void ReloadAssembly();
        void CompileAssemblies();
        void CreateCsharpProject();
    } // namespace CsharpScriptEngine
} // namespace HyperAPI