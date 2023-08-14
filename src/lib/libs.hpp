#pragma once

// Vendor Includes
#include "../Time/Time.hpp"
#include "../vendor/glad/include/glad/glad.h"
#include "../vendor/GLFW/glfw3.h"
#include "../vendor/ImGuizmo/ImSequencer.h"
#include "../vendor/discord-rpc/discord_rpc.h"
#include "../vendor/glm/ext.hpp"
#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/glm/gtc/type_ptr.hpp"
#include "../vendor/glm/gtx/quaternion.hpp"
#include "../vendor/stb_image/stb_image.h"
#include "../vendor/tinyxml/tinyxml2.h"
#include <FileWatch/FileWatch.hpp>
#include "ImGuizmo/ImGuizmo.h"
#include "LinearMath/btTransform.h"
#include "box2d/b2_world_callbacks.h"
#include "mono/metadata/class.h"
#include "mono/metadata/exception.h"
#include "mono/metadata/object.h"
#include "nativeScripts.hpp"
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include "../vendor/glm/gtx/matrix_decompose.hpp"
#include "../vendor/glm/gtx/rotate_vector.hpp"
#include "../vendor/glm/gtx/vector_angle.hpp"
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"
#include "../vendor/imgui/imgui_internal.h"
#include "../vendor/json/json.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../vendor/ImGuiColorTextEdit/TextEditor.h"
#include "../vendor/ImGuiFileDialog/ImGuiFileDialog.h"
#include "../vendor/SDL2/SDL_mixer.h"
#include "../vendor/assimp/Importer.hpp"
#include "../vendor/assimp/postprocess.h"
#include "../vendor/assimp/scene.h"
#include "../vendor/box2d/box2d.h"
#include "../vendor/bullet/bullet/BulletCollision/CollisionDispatch/btGhostObject.h"
#include "../vendor/bullet/bullet/btBulletDynamicsCommon.h"
#include "../vendor/entt/entt.hpp"
#include "../vendor/icons/icons.h"
#include "csharp.hpp"

// C++ Libraries
#include <any>
#include <experimental/filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <vector>

// Header Files
#include "InputEvents.hpp"
#include "ScriptEngine.hpp"
#include "networking.h"
#include "scene.hpp"
#include "scripts.hpp"

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#ifndef _WIN32

#include <unistd.h>

#else
#include <windows.h>
#endif

#define Vector2 glm::vec2
#define Vector3 glm::vec3
#define Vector4 glm::vec4

#define MAX_BONE_INFLUENCE 4

// command prompt colors
#ifdef _WIN32
#define RED SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12)
#define GREEN SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10)
#define BLUE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9)
#define YELLOW SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14)
#define WHITE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15)
#define PURPLE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13)
#define CYAN SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11)
#define GREY SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8)
#define RESET SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7)
#define HYPER_LOG(x)           \
    RED;                       \
    std::cout << "[VAULT] - "; \
    RESET;                     \
    std::cout << x << std::endl;
#else
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"
#define RESET "\033[0m"
#define HYPER_LOG(x)                                                                                          \
    std::cout << YELLOW "[" << HyperAPI::getTime() << "] - " << RED << "[VAULT] " << RESET << x << std::endl; \
    HyperAPI::currentHyperLog = x;
#endif

using json = nlohmann::json;

namespace uuid {
    DLL_API extern std::random_device rd;
    DLL_API extern std::mt19937 gen;
    DLL_API extern std::uniform_int_distribution<> dis;
    DLL_API extern std::uniform_int_distribution<> dis2;

    std::string generate_uuid_v4();
} // namespace uuid

namespace HyperAPI {
    DLL_API extern std::string cwd;
    DLL_API extern std::string dirPayloadData;
    DLL_API extern bool isRunning;
    DLL_API extern bool isStopped;
    DLL_API extern glm::vec3 mousePosWorld, mousePosCamWorld;
    DLL_API extern float sceneMouseX, sceneMouseY;
    DLL_API extern b2ContactListener *b2_listener;
    DLL_API extern std::string currentHyperLog;

#ifdef _WIN32
    __declspec(dllexport) void test_dll_function();
#endif

    struct DLL_API Config {
        char name[50];
        std::string mainScene;
        float ambientLight;
        float exposure;
        bool resizable;
        bool fullscreenOnLaunch;
        int width, height;
        bool deferredShading = false;

        struct DLL_API PostProcessing {
            bool enabled;
            struct DLL_API Vignette {
                float intensity;
                float smoothness;
            } vignette;

            struct DLL_API Bloom {
                bool enabled;
                float intensity;
                float threshold;
            } bloom;

            struct DLL_API ChromaticAberration {
                float intensity;
            } chromaticAberration;
        } postProcessing;

        struct DLL_API EditorCamera {
            float shiftSpeed = 0.4f;
        } editorCamera;

        char linuxCompiler[500];
        char windowsCompiler[500];
    };

    DLL_API extern Config config;

    struct DLL_API Component {
        bool IsPubliclyAddable = false;
    };

    class DLL_API ComponentSystem {
    public:
        std::string ID = uuid::generate_uuid_v4();
        std::string name = "GameObject";

        std::string Tag;
        // HyperAPI::LuaScriptComponent scriptComponent;
        std::vector<std::any> Components;

        ComponentSystem() = default;
        ~ComponentSystem() = default;

        void SetTag(const std::string &str) { Tag = str; }

        template <typename T>
        void AddComponent(T component) {
            Components.push_back(std::any_cast<T>(component));
        }

        template <typename T>
        T GetComponent() {
            for (auto component : Components) {
                try {
                    if (typeid(T) == typeid(std::any_cast<T>(component))) {
                        return std::any_cast<T>(component);
                    }
                } catch (const std::bad_any_cast &e) {
                }
            }
        }

        template <typename T>
        bool HasComponent() {
            for (auto component : Components) {
                if (typeid(T) == typeid(std::any_cast<T>(component))) {
                    return true;
                }
            }
            return false;
        }

        template <typename T>
        void UpdateComponent(T component) {
            for (auto &comp : Components) {
                if (typeid(T) == typeid(std::any_cast<T>(comp))) {
                    comp = component;
                }
            }
        }
    };

    struct DLL_API KeyPosition {
        glm::vec3 position;
        float timeStamp;
    };

    struct DLL_API KeyRotation {
        glm::quat orientation;
        float timeStamp;
    };

    struct DLL_API KeyScale {
        glm::vec3 scale;
        float timeStamp;
    };

    struct DLL_API TransformComponent : public Component {
        glm::mat4 transform = glm::mat4(1.0f);
        glm::vec3 position = glm::vec3(0, 0, 0);
        glm::vec3 rotation = glm::vec3(0, 0, 0);
        glm::vec3 scale = glm::vec3(1, 1, 1);

        void GUI() {
            // OLD ui
            //           ImGui::DragFloat3("Position", &position.x, 0.1f);
            //           ImGui::DragFloat3("Rotation", &rotation.x, 0.1f);
            //           ImGui::DragFloat3("Scale", &scale.x, 0.1f);
            // ---------------------------------------------------------------------------------------------------------
            //            NEW UI HERE
            // ---------------------------------------------------------------------------------------------------------
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, 100);
            ImGui::Text("Position");
            ImGui::NextColumn();
            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        }
    };
} // namespace HyperAPI