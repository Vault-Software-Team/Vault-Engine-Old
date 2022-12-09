#include "lib/api.hpp"
#include "mono/metadata/appdomain.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/class.h"
#include "lib/csharp.hpp"
#include "mono/metadata/image.h"
#include "mono/metadata/loader.h"
#include "mono/metadata/object-forward.h"
#include "mono/metadata/object.h"
#include <cstring>
#include <experimental/bits/fs_ops.h>
#include <functional>
#include <sstream>
#include <string>

namespace CsharpVariables {
    MonoDomain *rootDomain;
    MonoDomain *appDomain;
    MonoAssembly *coreAssembly;

    bool compiledAssembly = false;
}

namespace HyperAPI::CsharpScriptEngine::Functions {
    void NativeLog(MonoString *text) {
        std::string str(mono_string_to_utf8(text));
        std::cout << str << std::endl;
    }

    void EditorLog(MonoString *text) {
        std::string str(mono_string_to_utf8(text));
        Log log(str, LOG_INFO);
    }

    void EditorWarning(MonoString *text) {
        std::string str(mono_string_to_utf8(text));
        Log log(str, LOG_WARNING);
    }

    void EditorError(MonoString *text) {
        std::string str(mono_string_to_utf8(text));
        Log log(str, LOG_ERROR);
    }

    void TestVector(glm::vec3 *parameter) {
        std::cout << parameter->x << " " << parameter->y << " " << parameter->z << std::endl;
    }

    void GetComponent(MonoString *type, void *out) {
        using namespace Experimental;
        std::string str = mono_string_to_utf8(type);
        std::cout << str << std::endl;
        if(str == "Transform") {
            out = &Scene::m_GameObjects[0]->GetComponent<Transform>();
        }
    }

    void AudioPlay(MonoString *file, float volume, bool loop, int channel = -1) {
        using namespace Experimental;
        std::string str = mono_string_to_utf8(file);

        AudioEngine::PlaySound(str, volume, loop, channel);
    }

    void AudioMusic(MonoString *file, float volume, bool loop) {
        using namespace Experimental;
        std::string str = mono_string_to_utf8(file);

        AudioEngine::PlayMusic(str, volume, loop);
    }

    struct m_Vec3 {
        float x;
        float y;
        float z;

        m_Vec3(float x, float y, float z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }
    };

    // Transform Component Function
    void Transform_GetKey(MonoString *key, MonoString *id, MonoString **result) {
        using namespace Experimental;

        const std::string keyStr = mono_string_to_utf8(key);
        const std::string idStr = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(idStr);
        auto &transform = gameObject->GetComponent<Transform>();

        if(keyStr == "position") {
            *result = mono_string_new(CsharpVariables::appDomain, (
                std::to_string(transform.position.x) + " " + std::to_string(transform.position.y) + " " + std::to_string(transform.position.z)
            ).c_str());
        } 

        if(keyStr == "rotation") {
            *result = mono_string_new(CsharpVariables::appDomain, (
                std::to_string(transform.rotation.x) + " " + std::to_string(transform.rotation.y) + " " + std::to_string(transform.rotation.z)
            ).c_str());
        }

        if(keyStr == "scale") {
            *result = mono_string_new(CsharpVariables::appDomain, (
                std::to_string(transform.scale.x) + " " + std::to_string(transform.scale.y) + " " + std::to_string(transform.scale.z)
            ).c_str());
        }
    }

    void Transform_SetKey(MonoString *key, MonoString *id, MonoString *value) {
        using namespace Experimental;
        const std::string valStr = mono_string_to_utf8(value);

        const std::string keyStr = mono_string_to_utf8(key);
        const std::string idStr = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(idStr);
        auto &transform = gameObject->GetComponent<Transform>();

        std::cout << "Test" << std::endl;

        std::vector<float> vals;
        std::stringstream ss(valStr);
        std::string line;

        while(getline(ss, line, ' ')) {
            vals.push_back(std::stof(line));
        }

        if(keyStr == "position") {
            transform.position.x = vals[0];
            transform.position.y = vals[1];
            transform.position.z = vals[2];
        } 

        if(keyStr == "rotation") {
            transform.rotation.x = vals[0];
            transform.rotation.y = vals[1];
            transform.rotation.z = vals[2];
        }

        if(keyStr == "scale") {
            transform.scale.x = vals[0];
            transform.scale.y = vals[1];
            transform.scale.z = vals[2];
        }
    }

    // Entity Shit
    void Entity_GetID(MonoString **result) {
        *result = mono_string_new(CsharpVariables::appDomain, nextId.c_str());
    }

    void RegisterFunctions() {
        mono_add_internal_call("Vault.Terminal::Log", reinterpret_cast<void*(*)>(NativeLog));
        mono_add_internal_call("Vault.Debug::TestVector", reinterpret_cast<void*(*)>(TestVector));

        // Editor Logs
        mono_add_internal_call("Vault.Debug::Log", reinterpret_cast<void*(*)>(EditorLog));
        mono_add_internal_call("Vault.Debug::Error", reinterpret_cast<void*(*)>(EditorError));
        mono_add_internal_call("Vault.Debug::Warning", reinterpret_cast<void*(*)>(EditorWarning));

        // Components
        mono_add_internal_call("Vault.GameObject::CPP_GetComponent", reinterpret_cast<void*(*)>(GetComponent));
    
        // Audio
        mono_add_internal_call("Vault.Audio::Play", reinterpret_cast<void*(*)>(AudioPlay));
        mono_add_internal_call("Vault.Audio::Music", reinterpret_cast<void*(*)>(AudioMusic));

        // Transform Component
        mono_add_internal_call("Vault.Transform::GetKey", reinterpret_cast<void*(*)>(Transform_GetKey));
        mono_add_internal_call("Vault.Transform::SetKey", reinterpret_cast<void*(*)>(Transform_SetKey));

        // ID Shit
        mono_add_internal_call("Vault.Entity::GetID", reinterpret_cast<void*(*)>(Entity_GetID));
    }
}

namespace HyperAPI::CsharpScriptEngine {
    std::string nextId;

    std::unordered_map<std::string, MonoScriptClass*> entityClasses;
    std::unordered_map<std::string, MonoScriptClass*> instances;

    char* ReadBytes(const std::string& filepath, uint32_t* outSize) {
        std::ifstream stream(filepath, std::ios::binary | std::ios::ate);
        
        if (!stream) {
            return nullptr;
        }

        std::streampos end = stream.tellg();
        stream.seekg(0, std::ios::beg);
        uint32_t size = end - stream.tellg();
        
        if (size == 0) {
            return nullptr;
        }

        char* buffer = new char[size];
        stream.read((char*)buffer, size);
        stream.close();

        *outSize = size;
        return buffer;
    }

    MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath) {
        uint32_t fileSize = 0;
        char* fileData = ReadBytes(assemblyPath, &fileSize);

        // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
        MonoImageOpenStatus status;
        MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

        if (status != MONO_IMAGE_OK) {
            const char* errorMessage = mono_image_strerror(status);
            // Log some error message using the errorMessage data
            return nullptr;
        }

        MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
        mono_image_close(image);
                    
        // Don't forget to free the file data
        delete[] fileData;

        return assembly;
    }

    void PrintAssemblyTypes(MonoAssembly* assembly) {
        MonoImage* image = mono_assembly_get_image(assembly);
        const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

        for (int32_t i = 0; i < numTypes; i++)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

            printf("%s.%s\n", nameSpace, name);
        }
    }

    void LoadAssemblyClasses(MonoAssembly* assembly) {
        for(auto klass : entityClasses) {
            delete klass.second;
        }
        entityClasses.clear();

        MonoImage* image = mono_assembly_get_image(assembly);
        const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
        MonoClass *monoClass = mono_class_from_name(image, "Vault", "Entity");
        
        for (int32_t i = 0; i < numTypes; i++)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

            MonoClass *entityClass = mono_class_from_name(image, nameSpace, name);
           
            bool isSubclass = mono_class_is_subclass_of(entityClass, monoClass, false);

            if(entityClass == monoClass) continue;

            if(isSubclass) {
                entityClasses[std::string(std::string(nameSpace) + "." + name)] = new MonoScriptClass(nameSpace, name);
            }
        }
    }

    void InitMono() {
        using namespace Functions;
        using namespace CsharpVariables;

        if(fs::exists("cs-assembly/bin/Debug/net6.0/cs-assembly.dll")) {
            mono_set_assemblies_path(std::string(cwd + "/mono/lib").c_str());
            rootDomain = mono_jit_init("VaultJITRuntime");

            if(rootDomain == nullptr) exit(1);

            appDomain = mono_domain_create_appdomain("VaultScriptRuntime", nullptr);
            mono_domain_set(appDomain, true);
            
            RegisterFunctions();

            coreAssembly = LoadCSharpAssembly("cs-assembly/bin/Debug/net6.0/cs-assembly.dll");
            PrintAssemblyTypes(coreAssembly);
            LoadAssemblyClasses(coreAssembly);

            MonoScriptClass mainClass("Vault", "Terminal");
            mainClass.CallConstructor();

            MonoScriptClass debugClass("Vault", "Debug");
            debugClass.CallConstructor();

            MonoScriptClass audioClass("Vault", "Audio");
            mainClass.CallConstructor();
        }
    }

    void ReloadAssembly() {
        using namespace Functions;
        using namespace CsharpVariables;

        mono_domain_set(mono_get_root_domain(), false);

        //TODO: mono_domain_unload crashes. fix this
        mono_domain_unload(appDomain);

        appDomain = mono_domain_create_appdomain("VaultScriptRuntime", nullptr);
        mono_domain_set(appDomain, true);
            
        coreAssembly = LoadCSharpAssembly("cs-assembly/bin/Debug/net6.0/cs-assembly.dll");
        PrintAssemblyTypes(coreAssembly);
        LoadAssemblyClasses(coreAssembly);

        MonoScriptClass mainClass("Vault", "Terminal");
        mainClass.CallConstructor();

        MonoScriptClass debugClass("Vault", "Debug");
        debugClass.CallConstructor();

        MonoScriptClass audioClass("Vault", "Audio");
        mainClass.CallConstructor();
    }

    namespace Utils {
        void exec(const char* cmd) {
            std::thread *compilerThread = new std::thread([&] {
                std::array<char, 1000> buffer;
                std::string result;
                std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
                
                if (!pipe) {
                    throw std::runtime_error("popen() failed!");
                }

                while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                    using namespace HyperAPI;
                    const std::string output = buffer.data();    

                    if(output.find("error") != std::string::npos) {
                        Log errorLog(output, LOG_ERROR);
                    }
                    else if(output.find("warning") != std::string::npos) {
                        Log warningLog(output, LOG_WARNING);
                    }
                    else if(output.find("no warning") != std::string::npos) {
                        Log log(output, LOG_INFO);
                    }
                    else {
                        Log log(output, LOG_INFO);
                    }

                    if(output.find("Build succeeded.") != std::string::npos) {
                        CsharpVariables::compiledAssembly = true;
                    }
                }

                using namespace CsharpVariables;
                //TODO: hot reload scripts when it's done
            });
        }
    }

    void CompileAssemblies() {        
        Scene::logs.clear();
        Utils::exec("cd cs-assembly && dotnet build");
    }

    void CreateCsharpProject() {
        //TODO: Create a C# project using dotnet

        Scene::logs.clear();
        system("mkdir cs-assembly");

        if(fs::exists("cs-assembly")) {
            fs::copy("/home/koki1019/Desktop/MainProjects/Vault_Engine/cs-assembly/Main.cs", "cs-assembly/API.cs");
        }

        Utils::exec("cd cs-assembly && dotnet new classlib");
    }
}

MonoScriptClass::MonoScriptClass(const std::string &nameSpace, const std::string &name) : name_space(nameSpace), name(name) {
    using namespace CsharpVariables;

    MonoImage *image = mono_assembly_get_image(coreAssembly);
    klass = mono_class_from_name(image, nameSpace.c_str(), name.c_str());
    instance = mono_object_new(appDomain, klass);
}

MonoObject* MonoScriptClass::GetObject() const {    
    return instance;
}

MonoMethod* MonoScriptClass::GetMethod(const std::string method, int parameterCount) const {
    return mono_class_get_method_from_name(klass, method.c_str(), parameterCount);
};

MonoClass* MonoScriptClass::GetClass() const {
    return klass;
};

void MonoScriptClass::CallConstructor() {
    mono_runtime_object_init(instance);
}