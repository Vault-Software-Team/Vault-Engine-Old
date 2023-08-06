#pragma once
#include <dllapi.hpp>
#include "../vendor/glad/include/glad/glad.h"
#include "../vendor/GLFW/glfw3.h"
#include "ImGuizmo/ImGuizmo.h"
#include "../vendor/ImGuizmo/ImSequencer.h"
#include "../vendor/discord-rpc/discord_rpc.h"
#include "../vendor/stb_image/stb_image.h"
#include "../vendor/glm/glm.hpp"
#include "../vendor/tinyxml/tinyxml2.h"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/glm/gtc/type_ptr.hpp"
#include "../vendor/glm/ext.hpp"
#include "../vendor/glm/gtx/quaternion.hpp"
#include "LinearMath/btTransform.h"
#include "mono/metadata/class.h"
#include "mono/metadata/exception.h"
#include "mono/metadata/object.h"
#include "nativeScripts.hpp"
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include "../vendor/glm/gtx/matrix_decompose.hpp"
#include "../vendor/glm/gtx/rotate_vector.hpp"
#include "../vendor/glm/gtx/vector_angle.hpp"
#include "../vendor/json/json.hpp"
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_internal.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../vendor/icons/icons.h"
#include "../vendor/entt/entt.hpp"
#include "../vendor/ImGuiFileDialog/ImGuiFileDialog.h"
#include "../vendor/assimp/Importer.hpp"
#include "../vendor/assimp/scene.h"
#include "../vendor/box2d/box2d.h"
#include "../vendor/assimp/postprocess.h"
#include "../vendor/ImGuiColorTextEdit/TextEditor.h"
#include "../vendor/SDL2/SDL_mixer.h"
#include "../vendor/bullet/bullet/btBulletDynamicsCommon.h"
#include "../vendor/bullet/bullet/BulletCollision/CollisionDispatch/btGhostObject.h"
#include "csharp.hpp"

// C++ Libraries
#include <vector>
#include <functional>
#include <experimental/filesystem>
#include <any>
#include <fstream>
#include <iostream>
#include <sstream>
#include <random>
#include <memory>

// Header Files
#include "scene.hpp"
#include "ScriptEngine.hpp"
#include "InputEvents.hpp"
#include "scripts.hpp"
#include "networking.h"

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
#define HYPER_LOG(x) \
    std::cout << YELLOW "[" << HyperAPI::getTime() << "] - " << RED << "[VAULT] " << RESET << x << std::endl;
#endif

using json = nlohmann::json;

namespace uuid {
    DLL_API extern std::random_device rd;
    DLL_API extern std::mt19937 gen;
    DLL_API extern std::uniform_int_distribution<> dis;
    DLL_API extern std::uniform_int_distribution<> dis2;

    std::string generate_uuid_v4();
} // namespace uuid