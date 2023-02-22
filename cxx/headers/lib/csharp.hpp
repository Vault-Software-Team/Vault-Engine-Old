#pragma once
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

namespace CsharpVariables {
    extern MonoDomain *rootDomain;
    extern MonoDomain *appDomain;
    extern MonoAssembly *coreAssembly;
    extern bool compiledAssembly;
    extern std::string oldCwd;
} // namespace CsharpVariables

class MonoScriptClass {
private:
    MonoClass *klass;
    MonoObject *instance;

public:
    std::string name_space, name;

    MonoScriptClass(const std::string &nameSpace, const std::string &name);
    void CallConstructor();

    MonoClass *GetClass() const;
    MonoMethod *GetMethod(const std::string method, int parameterCount) const;
    MonoObject *f_GetObject() const;
};

namespace HyperAPI {
    namespace CsharpScriptEngine {
        extern std::unordered_map<std::string, MonoScriptClass *> entityClasses;
        extern std::unordered_map<std::string, MonoScriptClass *> instances;
        extern std::string nextId;

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