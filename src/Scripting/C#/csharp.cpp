#include <api.hpp>
#include "mono/metadata/appdomain.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/class.h"
#include <csharp.hpp>
#include "mono/metadata/image.h"
#include "mono/metadata/loader.h"
#include "mono/metadata/object-forward.h"
#include "mono/metadata/object.h"
#include <cstddef>
#include <cstring>
#include <experimental/bits/fs_ops.h>
#include <functional>
#include <sstream>
#include <string>

// Functions
#include "InputFunctions.hpp"
#include "LogFunctions.hpp"
#include "AudioFunctions.hpp"
#include "TransformFunctions.hpp"
#include "EntityFunctions.hpp"
#include "3DTextFunctions.hpp"
#include "SpriteRendererFunctions.hpp"
#include "SpriteAnimationFunctions.hpp"
#include "BloomFunctions.hpp"
#include "Rigidbody2DFunctions.hpp"
#include "BoxCollider2DFunctions.hpp"
#include "GameObjectFunctions.hpp"

namespace CsharpVariables {
    MonoDomain *rootDomain;
    MonoDomain *appDomain;
    MonoAssembly *coreAssembly;

    bool compiledAssembly = false;
} // namespace CsharpVariables

namespace HyperAPI::CsharpScriptEngine::Functions {
    void GetComponent(MonoString *type, void *out) {
        using namespace Experimental;
        std::string str = mono_string_to_utf8(type);
        std::cout << str << std::endl;
        if (str == "Transform") {
            out = &Scene::m_GameObjects[0]->GetComponent<Transform>();
        }
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

    void RegisterFunctions() {
        mono_add_internal_call("Vault.Terminal::Log", reinterpret_cast<void *(*)>(NativeLog));
        mono_add_internal_call("Vault.Debug::TestVector", reinterpret_cast<void *(*)>(TestVector));

        // Editor Logs
        mono_add_internal_call("Vault.Debug::Log", reinterpret_cast<void *(*)>(EditorLog));
        mono_add_internal_call("Vault.Debug::Error", reinterpret_cast<void *(*)>(EditorError));
        mono_add_internal_call("Vault.Debug::Warning", reinterpret_cast<void *(*)>(EditorWarning));

        // Components
        mono_add_internal_call("Vault.GameObject::CPP_GetComponent", reinterpret_cast<void *(*)>(GetComponent));

        // Audio
        mono_add_internal_call("Vault.Audio::Play", reinterpret_cast<void *(*)>(AudioPlay));
        mono_add_internal_call("Vault.Audio::Music", reinterpret_cast<void *(*)>(AudioMusic));

        // Transform Component
        mono_add_internal_call("Vault.Transform::GetKey", reinterpret_cast<void *(*)>(Transform_GetKey));
        mono_add_internal_call("Vault.Transform::SetKey", reinterpret_cast<void *(*)>(Transform_SetKey));

        // ID Shit
        mono_add_internal_call("Vault.Entity::GetID", reinterpret_cast<void *(*)>(Entity_GetID));

        // Input Keyboard
        mono_add_internal_call("Vault.Input::IsKeyPressed", reinterpret_cast<void *(*)>(Input_IsKeyPressed));
        mono_add_internal_call("Vault.Input::IsKeyReleased", reinterpret_cast<void *(*)>(Input_IsKeyReleased));
        mono_add_internal_call("Vault.Input::IsMouseButtonPressed", reinterpret_cast<void *(*)>(Input_IsMouseButtonPressed));
        mono_add_internal_call("Vault.Input::IsMouseButtonReleased", reinterpret_cast<void *(*)>(Input_IsMouseButtonReleased));
        mono_add_internal_call("Vault.Input::GetHorizontalAxis", reinterpret_cast<void *(*)>(Input_GetHorizontalAxis));
        mono_add_internal_call("Vault.Input::GetVerticalAxis", reinterpret_cast<void *(*)>(Input_GetVerticalAxis));
        mono_add_internal_call("Vault.Input::GetMouseXAxis", reinterpret_cast<void *(*)>(Input_GetMouseXAxis));
        mono_add_internal_call("Vault.Input::GetMouseYAxis", reinterpret_cast<void *(*)>(Input_GetMouseYAxis));
        mono_add_internal_call("Vault.Input::SetMouseHidden", reinterpret_cast<void *(*)>(Input_SetMouseHidden));
        mono_add_internal_call("Vault.Input::SetMousePosition", reinterpret_cast<void *(*)>(Input_SetMousePosition));

        // GameObject Functions
        mono_add_internal_call("Vault.GameObject::GetIDByName", reinterpret_cast<void *(*)>(GameObject_GetIDByName));
        mono_add_internal_call("Vault.GameObject::GetIDByTag", reinterpret_cast<void *(*)>(GameObject_GetIDByTag));

        // Text3D Component
        mono_add_internal_call("Vault.Text3D::GetKey", reinterpret_cast<void *(*)>(Text3D_GetKey));
        mono_add_internal_call("Vault.Text3D::SetText", reinterpret_cast<void *(*)>(Text3D_SetText));
        mono_add_internal_call("Vault.Text3D::SetOthers", reinterpret_cast<void *(*)>(Text3D_SetOthers));

        // SpriteRenderer Component
        mono_add_internal_call("Vault.SpriteRenderer::GetKey", reinterpret_cast<void *(*)>(SpriteRenderer_GetKey));
        mono_add_internal_call("Vault.SpriteRenderer::SetTexture", reinterpret_cast<void *(*)>(SpriteRenderer_SetTexture));
        mono_add_internal_call("Vault.SpriteRenderer::SetColor", reinterpret_cast<void *(*)>(SpriteRenderer_SetColor));

        // SpriteAnimation Component
        mono_add_internal_call("Vault.SpriteAnimation::GetKey", reinterpret_cast<void *(*)>(SpriteAnimation_GetKey));
        mono_add_internal_call("Vault.SpriteAnimation::SetCurrAnimation", reinterpret_cast<void *(*)>(SpriteAnimation_SetCurrAnimation));

        // Bloom Component
        mono_add_internal_call("Vault.Bloom::GetColor", reinterpret_cast<void *(*)>(Bloom_GetColor));
        mono_add_internal_call("Vault.Bloom::SetColor", reinterpret_cast<void *(*)>(Bloom_SetColor));

        // Rigidbody2D Component
        mono_add_internal_call("Vault.Rigidbody2D::cpp_GetKey", reinterpret_cast<void *(*)>(Rigidbody2D_GetKey));
        mono_add_internal_call("Vault.Rigidbody2D::cpp_SetVelocity", reinterpret_cast<void *(*)>(Rigidbody2D_SetVelocity));
        mono_add_internal_call("Vault.Rigidbody2D::cpp_SetAngularVelocity", reinterpret_cast<void *(*)>(Rigidbody2D_SetAngularVelocity));
        mono_add_internal_call("Vault.Rigidbody2D::cpp_SetPosition", reinterpret_cast<void *(*)>(Rigidbody2D_SetPosition));
        mono_add_internal_call("Vault.Rigidbody2D::cpp_Force", reinterpret_cast<void *(*)>(Rigidbody2D_Force));
        mono_add_internal_call("Vault.Rigidbody2D::cpp_Torque", reinterpret_cast<void *(*)>(Rigidbody2D_Torque));

        mono_add_internal_call("Vault.BoxCollider2D::cpp_GetKey", reinterpret_cast<void *(*)>(BoxCollider2D_GetKey));
        mono_add_internal_call("Vault.BoxCollider2D::cpp_SetKey", reinterpret_cast<void *(*)>(BoxCollider2D_SetKey));
        mono_add_internal_call("Vault.BoxCollider2D::cpp_SetOffset", reinterpret_cast<void *(*)>(BoxCollider2D_SetOffset));
        mono_add_internal_call("Vault.BoxCollider2D::cpp_SetSize", reinterpret_cast<void *(*)>(BoxCollider2D_SetSize));
        mono_add_internal_call("Vault.BoxCollider2D::cpp_GetTrigger", reinterpret_cast<void *(*)>(BoxCollider2D_GetTrigger));
        mono_add_internal_call("Vault.BoxCollider2D::cpp_SetTrigger", reinterpret_cast<void *(*)>(BoxCollider2D_SetTrigger));
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions

namespace HyperAPI::CsharpScriptEngine {
    std::string nextId;

    std::unordered_map<std::string, MonoScriptClass *> entityClasses;
    std::unordered_map<std::string, MonoScriptClass *> instances;

    char *ReadBytes(const std::string &filepath, uint32_t *outSize) {
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

        char *buffer = new char[size];
        stream.read((char *)buffer, size);
        stream.close();

        *outSize = size;
        return buffer;
    }

    MonoAssembly *LoadCSharpAssembly(const std::string &assemblyPath) {
        uint32_t fileSize = 0;
        char *fileData = ReadBytes(assemblyPath, &fileSize);

        // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
        MonoImageOpenStatus status;
        MonoImage *image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

        if (status != MONO_IMAGE_OK) {
            const char *errorMessage = mono_image_strerror(status);
            // Log some error message using the errorMessage data
            return nullptr;
        }

        MonoAssembly *assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
        mono_image_close(image);

        // Don't forget to free the file data
        delete[] fileData;

        return assembly;
    }

    void PrintAssemblyTypes(MonoAssembly *assembly) {
        MonoImage *image = mono_assembly_get_image(assembly);
        const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

        for (int32_t i = 0; i < numTypes; i++) {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            const char *nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const char *name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

            printf("%s.%s\n", nameSpace, name);
        }
    }

    void LoadAssemblyClasses(MonoAssembly *assembly) {
        for (auto klass : entityClasses) {
            delete klass.second;
        }
        entityClasses.clear();

        MonoImage *image = mono_assembly_get_image(assembly);
        const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
        MonoClass *monoClass = mono_class_from_name(image, "Vault", "Entity");

        for (int32_t i = 0; i < numTypes; i++) {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            const char *nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const char *name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

            MonoClass *entityClass = mono_class_from_name(image, nameSpace, name);

            bool isSubclass = mono_class_is_subclass_of(entityClass, monoClass, false);

            if (entityClass == monoClass)
                continue;

            if (isSubclass) {
                entityClasses[std::string(std::string(nameSpace) + "." + name)] = new MonoScriptClass(nameSpace, name);
            }
        }
    }

    void RuntimeInit() {
        using namespace CsharpVariables;
        if (rootDomain == nullptr) {
            rootDomain = mono_jit_init("VaultJITRuntime");

            if (rootDomain == nullptr)
                exit(1);
        }
    }

    void InitMono() {
        using namespace Functions;
        using namespace CsharpVariables;

        if (fs::exists("cs-assembly/bin/Debug/net6.0/cs-assembly.dll")) {
            mono_set_assemblies_path(std::string(cwd + "/mono/lib").c_str());
            RuntimeInit();

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
        mono_domain_unload(appDomain);

        InitMono();
    }

    void CompileAssemblies() {
        Scene::logs.clear();
        std::thread *compilerThread = new std::thread([&] {
            std::array<char, 1000> buffer;
            std::string result;
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("cd cs-assembly && dotnet build", "r"), pclose);

            if (!pipe) {
                throw std::runtime_error("popen() failed!");
            }

            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                using namespace HyperAPI;
                const std::string output = buffer.data();

                if (output.find("error") != std::string::npos) {
                    Log errorLog(output, LOG_ERROR);
                } else if (output.find("warning") != std::string::npos) {
                    Log warningLog(output, LOG_WARNING);
                } else if (output.find("no warning") != std::string::npos) {
                    Log log(output, LOG_INFO);
                } else {
                    Log log(output, LOG_INFO);
                }

                if (output.find("Build succeeded.") != std::string::npos) {
                    CsharpVariables::compiledAssembly = true;
                }
            }

            using namespace CsharpVariables;
            // TODO: hot reload scripts when it's done
        });
    }

    void CreateCsharpProject() {
        // TODO: Create a C# project using dotnet

        Scene::logs.clear();
        system("mkdir cs-assembly");

        if (fs::exists("cs-assembly")) {
            fs::copy("/home/koki1019/Desktop/MainProjects/Vault_Engine/cs-assembly/Main.cs", "cs-assembly/API.cs");
        }

        std::thread *compilerThread = new std::thread([&] {
            std::array<char, 1000> buffer;
            std::string result;
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("cd cs-assembly && dotnet new classlib", "r"), pclose);

            if (!pipe) {
                throw std::runtime_error("popen() failed!");
            }

            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                using namespace HyperAPI;
                const std::string output = buffer.data();

                if (output.find("error") != std::string::npos) {
                    Log errorLog(output, LOG_ERROR);
                } else if (output.find("warning") != std::string::npos) {
                    Log warningLog(output, LOG_WARNING);
                } else if (output.find("no warning") != std::string::npos) {
                    Log log(output, LOG_INFO);
                } else {
                    Log log(output, LOG_INFO);
                }

                if (output.find("Build succeeded.") != std::string::npos) {
                    CsharpVariables::compiledAssembly = true;
                }
            }

            using namespace CsharpVariables;
            // TODO: hot reload scripts when it's done
        });
    }
} // namespace HyperAPI::CsharpScriptEngine

MonoScriptClass::MonoScriptClass(const std::string &nameSpace, const std::string &name) : name_space(nameSpace), name(name) {
    using namespace CsharpVariables;

    MonoImage *image = mono_assembly_get_image(coreAssembly);
    klass = mono_class_from_name(image, nameSpace.c_str(), name.c_str());
    instance = mono_object_new(appDomain, klass);
}

MonoObject *MonoScriptClass::f_GetObject() const {
    return instance;
}

MonoMethod *MonoScriptClass::GetMethod(const std::string method, int parameterCount) const {
    return mono_class_get_method_from_name(klass, method.c_str(), parameterCount);
};

MonoClass *MonoScriptClass::GetClass() const {
    return klass;
};

void MonoScriptClass::CallConstructor() {
    mono_runtime_object_init(instance);
}