#include "lib/api.hpp"
#include "mono/metadata/appdomain.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/class.h"
#include "lib/csharp.hpp"
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

namespace CsharpVariables {
    MonoDomain *rootDomain;
    MonoDomain *appDomain;
    MonoAssembly *coreAssembly;

    bool compiledAssembly = false;
} // namespace CsharpVariables

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
        if (str == "Transform") {
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

        if (keyStr == "position") {
            *result = mono_string_new(CsharpVariables::appDomain, (
                                                                      std::to_string(transform.position.x) + " " + std::to_string(transform.position.y) + " " + std::to_string(transform.position.z))
                                                                      .c_str());
        }

        if (keyStr == "rotation") {
            *result = mono_string_new(CsharpVariables::appDomain, (
                                                                      std::to_string(transform.rotation.x) + " " + std::to_string(transform.rotation.y) + " " + std::to_string(transform.rotation.z))
                                                                      .c_str());
        }

        if (keyStr == "scale") {
            *result = mono_string_new(CsharpVariables::appDomain, (
                                                                      std::to_string(transform.scale.x) + " " + std::to_string(transform.scale.y) + " " + std::to_string(transform.scale.z))
                                                                      .c_str());
        }
    }

    void Transform_SetKey(MonoString *key, MonoString *id, float x, float y, float z) {
        using namespace Experimental;
        const std::string keyStr = mono_string_to_utf8(key);
        const std::string idStr = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(idStr);
        auto &transform = gameObject->GetComponent<Transform>();

        if (keyStr == "position") {
            transform.position.x = x;
            transform.position.y = y;
            transform.position.z = z;
        }

        if (keyStr == "rotation") {
            transform.rotation.x = x;
            transform.rotation.y = y;
            transform.rotation.z = z;
        }

        if (keyStr == "scale") {
            transform.scale.x = x;
            transform.scale.y = y;
            transform.scale.z = z;
        }
    }

    // Entity Shit
    void Entity_GetID(MonoString **result) {
        *result = mono_string_new(CsharpVariables::appDomain, nextId.c_str());
    }

    // Inputs
    bool Input_IsKeyPressed(int key) {
        return Input::IsKeyPressed(key);
    }

    bool Input_IsKeyReleased(int key) {
        return Input::IsKeyReleased(key);
    }

    bool Input_IsMouseButtonPressed(int button) {
        return Input::IsMouseButtonPressed(button);
    }

    bool Input_IsMouseButtonReleased(int button) {
        return Input::IsMouseButtonReleased(button);
    }

    int Input_GetHorizontalAxis() {
        return Input::GetHorizontalAxis();
    }

    int Input_GetVerticalAxis() {
        return Input::GetVerticalAxis();
    }

    int Input_GetMouseXAxis() {
        return Input::GetMouseXAxis();
    }

    int Input_GetMouseYAxis() {
        return Input::GetMouseYAxis();
    }

    void Input_SetMouseHidden(bool hidden) {
        Input::SetMouseHidden(hidden);
    }

    void Input_SetMousePosition(float x, float y) {
        Input::SetMousePosition(x, y);
    }

    // Material Stuff
    void Material_GetTexture(MonoString *ID, MonoString *Component, MonoString *type, MonoString **texture) {
        using namespace Experimental;

        const std::string id = mono_string_to_utf8(ID);
        const std::string component = mono_string_to_utf8(Component);
        const std::string Type = mono_string_to_utf8(type);

        auto *gameObject = f_GameObject::FindGameObjectByID(id);

        if (component == "MeshRenderer") {
            auto &renderer = gameObject->GetComponent<MeshRenderer>();

            if (Type == "diffuse") {
                *texture = mono_string_new(CsharpVariables::appDomain, renderer.m_Mesh->material.diffuse != nullptr ? renderer.m_Mesh->material.diffuse->texPath.c_str() : "null");
            } else if (Type == "specular") {
                *texture = mono_string_new(CsharpVariables::appDomain, renderer.m_Mesh->material.specular != nullptr ? renderer.m_Mesh->material.specular->texPath.c_str() : "null");
            } else if (Type == "normal") {
                *texture = mono_string_new(CsharpVariables::appDomain, renderer.m_Mesh->material.normal != nullptr ? renderer.m_Mesh->material.normal->texPath.c_str() : "null");
            } else if (Type == "emission") {
                *texture = mono_string_new(CsharpVariables::appDomain, renderer.m_Mesh->material.emission != nullptr ? renderer.m_Mesh->material.emission->texPath.c_str() : "null");
            }
        } else if (component == "SpriteRenderer") {
            auto &renderer = gameObject->GetComponent<SpriteRenderer>();

            if (Type == "diffuse") {
                *texture = mono_string_new(CsharpVariables::appDomain, renderer.mesh->material.diffuse != nullptr ? renderer.mesh->material.diffuse->texPath.c_str() : "null");
            } else if (Type == "specular") {
                *texture = mono_string_new(CsharpVariables::appDomain, renderer.mesh->material.specular != nullptr ? renderer.mesh->material.specular->texPath.c_str() : "null");
            } else if (Type == "normal") {
                *texture = mono_string_new(CsharpVariables::appDomain, renderer.mesh->material.normal != nullptr ? renderer.mesh->material.normal->texPath.c_str() : "null");
            } else if (Type == "emission") {
                *texture = mono_string_new(CsharpVariables::appDomain, renderer.mesh->material.emission != nullptr ? renderer.mesh->material.emission->texPath.c_str() : "null");
            }
        } else if (component == "SpritesheetAnimation") {
            auto &renderer = gameObject->GetComponent<c_SpritesheetAnimation>();

            if (Type == "diffuse") {
                *texture = mono_string_new(CsharpVariables::appDomain, renderer.mesh->material.diffuse != nullptr ? renderer.mesh->material.diffuse->texPath.c_str() : "null");
            } else if (Type == "specular") {
                *texture = mono_string_new(CsharpVariables::appDomain, renderer.mesh->material.specular != nullptr ? renderer.mesh->material.specular->texPath.c_str() : "null");
            } else if (Type == "normal") {
                *texture = mono_string_new(CsharpVariables::appDomain, renderer.mesh->material.normal != nullptr ? renderer.mesh->material.normal->texPath.c_str() : "null");
            } else if (Type == "emission") {
                *texture = mono_string_new(CsharpVariables::appDomain, renderer.mesh->material.emission != nullptr ? renderer.mesh->material.emission->texPath.c_str() : "null");
            }
        }
    }

    void Material_SetTexture(MonoString *ID, MonoString *Component, MonoString *type, MonoString *texture) {
        using namespace Experimental;

        const std::string id = mono_string_to_utf8(ID);
        const std::string component = mono_string_to_utf8(Component);
        const std::string Type = mono_string_to_utf8(type);
        const std::string tex = mono_string_to_utf8(texture);

        auto *gameObject = f_GameObject::FindGameObjectByID(id);

        if (component == "MeshRenderer") {
            auto &renderer = gameObject->GetComponent<MeshRenderer>();

            if (Type == "diffuse") {
                if (renderer.m_Mesh->material.diffuse == nullptr) {
                    renderer.m_Mesh->material.diffuse = new Texture(tex.c_str(), 0, "texture_diffuse");
                } else {
                    delete renderer.m_Mesh->material.diffuse;
                    renderer.m_Mesh->material.diffuse = new Texture(tex.c_str(), 0, "texture_diffuse");
                }
            } else if (Type == "specular") {
                if (renderer.m_Mesh->material.specular == nullptr) {
                    renderer.m_Mesh->material.specular = new Texture(tex.c_str(), 0, "texture_specular");
                } else {
                    delete renderer.m_Mesh->material.specular;
                    renderer.m_Mesh->material.specular = new Texture(tex.c_str(), 0, "texture_specular");
                }
            } else if (Type == "normal") {
                if (renderer.m_Mesh->material.normal == nullptr) {
                    renderer.m_Mesh->material.normal = new Texture(tex.c_str(), 0, "texture_normal");
                } else {
                    delete renderer.m_Mesh->material.normal;
                    renderer.m_Mesh->material.normal = new Texture(tex.c_str(), 0, "texture_normal");
                }
            } else if (Type == "emission") {
                if (renderer.m_Mesh->material.emission == nullptr) {
                    renderer.m_Mesh->material.emission = new Texture(tex.c_str(), 0, "texture_emission");
                } else {
                    delete renderer.m_Mesh->material.emission;
                    renderer.m_Mesh->material.emission = new Texture(tex.c_str(), 0, "texture_emission");
                }
            }
        } else if (component == "SpriteRenderer") {
            auto &renderer = gameObject->GetComponent<SpriteRenderer>();

            if (Type == "diffuse") {
                if (renderer.mesh->material.diffuse == nullptr) {
                    renderer.mesh->material.diffuse = new Texture(tex.c_str(), 0, "texture_diffuse");
                } else {
                    delete renderer.mesh->material.diffuse;
                    renderer.mesh->material.diffuse = new Texture(tex.c_str(), 0, "texture_diffuse");
                }
            } else if (Type == "specular") {
                if (renderer.mesh->material.specular == nullptr) {
                    renderer.mesh->material.specular = new Texture(tex.c_str(), 0, "texture_specular");
                } else {
                    delete renderer.mesh->material.specular;
                    renderer.mesh->material.specular = new Texture(tex.c_str(), 0, "texture_specular");
                }
            } else if (Type == "normal") {
                if (renderer.mesh->material.normal == nullptr) {
                    renderer.mesh->material.normal = new Texture(tex.c_str(), 0, "texture_normal");
                } else {
                    delete renderer.mesh->material.normal;
                    renderer.mesh->material.normal = new Texture(tex.c_str(), 0, "texture_normal");
                }
            } else if (Type == "emission") {
                if (renderer.mesh->material.emission == nullptr) {
                    renderer.mesh->material.emission = new Texture(tex.c_str(), 0, "texture_emission");
                } else {
                    delete renderer.mesh->material.emission;
                    renderer.mesh->material.emission = new Texture(tex.c_str(), 0, "texture_emission");
                }
            }
        } else if (component == "SpritesheetAnimation") {
            auto &renderer = gameObject->GetComponent<c_SpritesheetAnimation>();

            if (Type == "diffuse") {
                if (renderer.mesh->material.diffuse == nullptr) {
                    renderer.mesh->material.diffuse = new Texture(tex.c_str(), 0, "texture_diffuse");
                } else {
                    delete renderer.mesh->material.diffuse;
                    renderer.mesh->material.diffuse = new Texture(tex.c_str(), 0, "texture_diffuse");
                }
            } else if (Type == "specular") {
                if (renderer.mesh->material.specular == nullptr) {
                    renderer.mesh->material.specular = new Texture(tex.c_str(), 0, "texture_specular");
                } else {
                    delete renderer.mesh->material.specular;
                    renderer.mesh->material.specular = new Texture(tex.c_str(), 0, "texture_specular");
                }
            } else if (Type == "normal") {
                if (renderer.mesh->material.normal == nullptr) {
                    renderer.mesh->material.normal = new Texture(tex.c_str(), 0, "texture_normal");
                } else {
                    delete renderer.mesh->material.normal;
                    renderer.mesh->material.normal = new Texture(tex.c_str(), 0, "texture_normal");
                }
            } else if (Type == "emission") {
                if (renderer.mesh->material.emission == nullptr) {
                    renderer.mesh->material.emission = new Texture(tex.c_str(), 0, "texture_emission");
                } else {
                    delete renderer.mesh->material.emission;
                    renderer.mesh->material.emission = new Texture(tex.c_str(), 0, "texture_emission");
                }
            }
        }
    }

    void Material_GetColor(MonoString *ID, MonoString *Component, MonoString **out) {
        using namespace Experimental;

        const std::string id = mono_string_to_utf8(ID);
        const std::string component = mono_string_to_utf8(Component);

        auto *gameObject = f_GameObject::FindGameObjectByID(id);
        Vector4 color;

        if (component == "MeshRenderer") {
            auto &renderer = gameObject->GetComponent<MeshRenderer>();

            color = renderer.m_Mesh->material.baseColor;
        } else if (component == "SpriteRenderer") {
            auto &renderer = gameObject->GetComponent<SpriteRenderer>();

            color = renderer.mesh->material.baseColor;
        } else if (component == "SpritesheetAnimation") {
            auto &renderer = gameObject->GetComponent<c_SpritesheetAnimation>();

            color = renderer.mesh->material.baseColor;
        }

        *out = mono_string_new(CsharpVariables::appDomain, (
                                                               std::to_string(color.r) + " " + std::to_string(color.g) + " " + std::to_string(color.b) + " " + std::to_string(color.a))
                                                               .c_str());
    }

    void Material_SetColor(MonoString *ID, MonoString *Component, float r, float g, float b, float a) {
        using namespace Experimental;

        const std::string id = mono_string_to_utf8(ID);
        const std::string component = mono_string_to_utf8(Component);

        auto *gameObject = f_GameObject::FindGameObjectByID(id);
        Vector4 *color;

        if (component == "MeshRenderer") {
            auto &renderer = gameObject->GetComponent<MeshRenderer>();

            color = &renderer.m_Mesh->material.baseColor;
        } else if (component == "SpriteRenderer") {
            auto &renderer = gameObject->GetComponent<SpriteRenderer>();

            color = &renderer.mesh->material.baseColor;
        } else if (component == "SpritesheetAnimation") {
            auto &renderer = gameObject->GetComponent<c_SpritesheetAnimation>();

            color = &renderer.mesh->material.baseColor;
        }

        color->r = r;
        color->g = g;
        color->b = b;
        color->a = a;
    }

    void SpritesheetAnimation_GetCurrentAnimation(MonoString *ID, MonoString **out) {
        using namespace CsharpVariables;
        using namespace Experimental;

        const std::string id = mono_string_to_utf8(ID);

        auto *gameObject = f_GameObject::FindGameObjectByID(id);
        auto &renderer = gameObject->GetComponent<c_SpritesheetAnimation>();

        *out = mono_string_new(appDomain, renderer.currAnim);
    }

    void SpritesheetAnimation_SetCurrentAnimation(MonoString *ID, MonoString *value) {
        using namespace CsharpVariables;
        using namespace Experimental;

        const std::string id = mono_string_to_utf8(ID);
        const std::string Value = mono_string_to_utf8(value);

        auto *gameObject = f_GameObject::FindGameObjectByID(id);
        auto &renderer = gameObject->GetComponent<c_SpritesheetAnimation>();

        strcpy(renderer.currAnim, Value.c_str());
    }

    void Material_GetTextureScale(MonoString *ID, MonoString **result) {
        using namespace CsharpVariables;
        using namespace Experimental;

        const std::string id = mono_string_to_utf8(ID);
        auto *gameObject = f_GameObject::FindGameObjectByID(id);

        auto &renderer = gameObject->GetComponent<MeshRenderer>();

        if (renderer.m_Mesh) {
            *result = mono_string_new(appDomain, (
                                                     std::to_string(renderer.m_Mesh->material.texUVs.x) + " " +
                                                     std::to_string(renderer.m_Mesh->material.texUVs.y))
                                                     .c_str());
        }
    }

    void Material_SetTextureScale(MonoString *ID, float x, float y) {
        using namespace CsharpVariables;
        using namespace Experimental;

        const std::string id = mono_string_to_utf8(ID);
        auto *gameObject = f_GameObject::FindGameObjectByID(id);

        auto &renderer = gameObject->GetComponent<MeshRenderer>();

        if (renderer.m_Mesh) {
            renderer.m_Mesh->material.texUVs.x = x;
            renderer.m_Mesh->material.texUVs.y = y;
        }
    }

    void Material_GetMetallic(MonoString *ID, MonoString **result) {
        using namespace CsharpVariables;
        using namespace Experimental;

        const std::string id = mono_string_to_utf8(ID);
        auto *gameObject = f_GameObject::FindGameObjectByID(id);

        auto &renderer = gameObject->GetComponent<MeshRenderer>();

        if (renderer.m_Mesh) {
            *result = mono_string_new(appDomain, (
                                                     std::to_string(renderer.m_Mesh->material.metallic))
                                                     .c_str());
        }
    }

    void Material_SetMetallic(MonoString *ID, float value) {
        using namespace CsharpVariables;
        using namespace Experimental;

        std::cout << value << std::endl;

        const std::string id = mono_string_to_utf8(ID);
        auto *gameObject = f_GameObject::FindGameObjectByID(id);

        auto &renderer = gameObject->GetComponent<MeshRenderer>();

        if (renderer.m_Mesh) {
            renderer.m_Mesh->material.metallic = value;
        }
    }

    void Material_GetRoughness(MonoString *ID, MonoString **result) {
        using namespace CsharpVariables;
        using namespace Experimental;

        const std::string id = mono_string_to_utf8(ID);
        auto *gameObject = f_GameObject::FindGameObjectByID(id);

        auto &renderer = gameObject->GetComponent<MeshRenderer>();

        if (renderer.m_Mesh) {
            *result = mono_string_new(appDomain, (
                                                     std::to_string(renderer.m_Mesh->material.roughness))
                                                     .c_str());
        }
    }

    void Material_SetRoughness(MonoString *ID, float value) {
        using namespace CsharpVariables;
        using namespace Experimental;

        const std::string id = mono_string_to_utf8(ID);
        auto *gameObject = f_GameObject::FindGameObjectByID(id);

        auto &renderer = gameObject->GetComponent<MeshRenderer>();

        if (renderer.m_Mesh) {
            renderer.m_Mesh->material.roughness = value;
        }
    }

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

        // Material Component
        mono_add_internal_call("Vault.Material::GetTexture", reinterpret_cast<void *(*)>(Material_GetTexture));
        mono_add_internal_call("Vault.Material::SetTexture", reinterpret_cast<void *(*)>(Material_SetTexture));
        mono_add_internal_call("Vault.Material::GetTextureScale", reinterpret_cast<void *(*)>(Material_GetTextureScale));
        mono_add_internal_call("Vault.Material::SetTextureScale", reinterpret_cast<void *(*)>(Material_SetTextureScale));
        mono_add_internal_call("Vault.Material::GetMetallic", reinterpret_cast<void *(*)>(Material_GetMetallic));
        mono_add_internal_call("Vault.Material::SetMetallic", reinterpret_cast<void *(*)>(Material_SetMetallic));
        mono_add_internal_call("Vault.Material::GetRoughness", reinterpret_cast<void *(*)>(Material_GetRoughness));
        mono_add_internal_call("Vault.Material::SetRoughness", reinterpret_cast<void *(*)>(Material_SetRoughness));

        mono_add_internal_call("Vault.Material::GetColor", reinterpret_cast<void *(*)>(Material_GetColor));
        mono_add_internal_call("Vault.Material::SetColor", reinterpret_cast<void *(*)>(Material_SetColor));

        // SpritesheetAnimation Component
        mono_add_internal_call("Vault.SpritesheetAnimation::GetCurrentAnimation", reinterpret_cast<void *(*)>(SpritesheetAnimation_GetCurrentAnimation));
        mono_add_internal_call("Vault.SpritesheetAnimation::SetCurrentAnimation", reinterpret_cast<void *(*)>(SpritesheetAnimation_SetCurrentAnimation));
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

    void InitMono() {
        using namespace Functions;
        using namespace CsharpVariables;

        if (fs::exists("cs-assembly/bin/Debug/net6.0/cs-assembly.dll")) {
            mono_set_assemblies_path(std::string(cwd + "/mono/lib").c_str());
            rootDomain = mono_jit_init("VaultJITRuntime");

            if (rootDomain == nullptr)
                exit(1);

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

        // TODO: mono_domain_unload crashes. fix this
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
        void exec(const char *cmd) {
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
    } // namespace Utils

    void CompileAssemblies() {
        Scene::logs.clear();
        Utils::exec("cd cs-assembly && dotnet build");
    }

    void CreateCsharpProject() {
        // TODO: Create a C# project using dotnet

        Scene::logs.clear();
        system("mkdir cs-assembly");

        if (fs::exists("cs-assembly")) {
            fs::copy("/home/koki1019/Desktop/MainProjects/Vault_Engine/cs-assembly/Main.cs", "cs-assembly/API.cs");
        }

        Utils::exec("cd cs-assembly && dotnet new classlib");
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