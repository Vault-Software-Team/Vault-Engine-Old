#pragma once
#pragma once
#include "../vendor/glad/include/glad/glad.h"
#include "../vendor/GLFW/glfw3.h"
#include <fstream>
#include <iostream>
#include "../vendor/stb_image/stb_image.h"
#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/glm/gtc/type_ptr.hpp"
#include "../vendor/glm/ext.hpp"
#include "../vendor/glm/gtx/rotate_vector.hpp"
#include "../vendor/glm/gtx/vector_angle.hpp"
#include "../vendor/json/json.hpp"
// #include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"
#include "../vendor/icons/icons.h"
#include "../vendor/entt/entt.hpp"
#include "../vendor/ImGuiFileDialog/ImGuiFileDialog.h"
#include <vector>
#include <functional>
#include <experimental/filesystem>
#include "../vendor/assimp/Importer.hpp"
#include "../vendor/assimp/scene.h"
// import glm helpers
#include "../vendor/box2d/box2d.h"
#include "../vendor/box2d/b2_world.h"
#include "../vendor/box2d/b2_body.h"
#include "../vendor/box2d/b2_fixture.h"
#include "../vendor/box2d/b2_polygon_shape.h"

#include "../vendor/assimp/postprocess.h"
#include <any>
#include <map>
#include "scene.hpp"
#include <memory>

bool G_END_WITH(std::string const &value, std::string const &ending);

namespace HyperAPI {
    class Mesh;
    class Model;
    class Camera;
    class Texture;
    class Shader;
    class Texture;
    struct Log;

    struct PointLight;
    struct SpotLight;
    struct DirectionalLight;
    struct Light2D;
    namespace Experimental {
        class GameObject;
        struct m_AnimationData;

    } // namespace Experimental

    namespace Scene {
        extern std::map<std::string, Texture *> Textures;

        enum DragType {
            DRAG_DIFFUSE,
            DRAG_SPECULAR,
            DRAG_NORMAL,
            DRAG_MODEL,
            DRAG_SCENE,
            DRAG_SPRITE,
            DRAG_SPRITE_NO_MESH,
            DRAG_PREFAB,
            DRAG_GAMEOBJECT,
            DRAG_SHADER
        };

        enum SceneType {
            MAIN_SCENE,
            UI_SCENE
        };

        extern bool LoadingScene;
        extern std::vector<Experimental::GameObject *> m_GameObjects;
        extern std::vector<Experimental::GameObject *> m_UIObjects;
        extern std::map<std::string, bool> layers;
        extern std::map<std::string, std::map<std::string, int>> currFrames;
        extern std::map<std::string, std::map<std::string, float>> currDelays;
        extern std::string currentScenePath;
        void SaveComponents(nlohmann::json &JSON, Experimental::GameObject *gameObject, const int i, int &componentOffset);

        void LoadScene(const std::string &scenePath, nlohmann::json &StateScene);
        Experimental::GameObject *LoadPrefab(const std::string &scenePath);
        Experimental::GameObject *LoadJSONPrefab(const nlohmann::json &JSON);
        void SavePrefab(const std::string &path, Experimental::GameObject *gameObject);
        void SaveJSONPrefab(nlohmann::json &JSON, Experimental::GameObject *gameObject);

        void SaveScene(const std::string &path, nlohmann::json &StateScene);

        bool DropTargetMat(DragType type, Mesh *currEntity, Texture *otherData = nullptr);

        extern Experimental::GameObject *m_Object;
        extern char name[499];
        extern char tag[499];
        extern char layer[32];
        extern entt::registry m_Registry;

        extern std::vector<Mesh *> entities;
        extern std::vector<Model> models;
        extern Camera *mainCamera;
        extern std::vector<Camera *> cameras;
        extern std::vector<Log> logs;
#ifdef _WIN32
#ifdef BUILD_DLL
        extern "C" __declspec(dllimport) std::vector<Log> *GetLogs();
#else
        extern "C" __declspec(dllexport) std::vector<Log> *GetLogs();
#endif
#endif
        extern glm::mat4 projection;

        extern std::vector<entt::entity> backup_entities;

        extern std::vector<HyperAPI::PointLight *> PointLights;
        extern std::vector<HyperAPI::Light2D *> Lights2D;
        extern std::vector<HyperAPI::SpotLight *> SpotLights;
        extern std::vector<HyperAPI::DirectionalLight *> DirLights;
        extern std::vector<HyperAPI::Mesh *> hyperEntities;

        extern b2World *world;
    } // namespace Scene
} // namespace HyperAPI
