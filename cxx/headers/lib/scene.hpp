#pragma once
#include <dllapi.hpp>
#pragma once
#include <dllapi.hpp>
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
    class DLL_API Mesh;
    class DLL_API Model;
    class DLL_API Camera;
    class DLL_API Texture;
    class DLL_API Shader;
    class DLL_API Texture;
    struct DLL_API Log;

    struct DLL_API PointLight;
    struct DLL_API SpotLight;
    struct DLL_API DirectionalLight;
    struct DLL_API Light2D;
    namespace Experimental {
        class DLL_API GameObject;
        struct DLL_API m_AnimationData;

    } // namespace Experimental

    namespace Scene {
        DLL_API extern std::map<std::string, Texture *> Textures;

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

        DLL_API extern bool LoadingScene;
        DLL_API extern std::vector<Experimental::GameObject *> *m_GameObjects;
        DLL_API extern std::vector<Experimental::GameObject *> m_UIObjects;
        DLL_API extern std::map<std::string, bool> layers;
        DLL_API extern std::map<std::string, std::map<std::string, int>> currFrames;
        DLL_API extern std::map<std::string, std::map<std::string, float>> currDelays;
        DLL_API extern std::string currentScenePath;
        void SaveComponents(nlohmann::json &JSON, Experimental::GameObject *gameObject, const int i, int &componentOffset);

        void LoadScene(const std::string &scenePath, nlohmann::json &StateScene);
        Experimental::GameObject *LoadPrefab(const std::string &scenePath);
        Experimental::GameObject *LoadJSONPrefab(const nlohmann::json &JSON);
        void SavePrefab(const std::string &path, Experimental::GameObject *gameObject);
        void SaveJSONPrefab(nlohmann::json &JSON, Experimental::GameObject *gameObject);

        void SaveScene(const std::string &path, nlohmann::json &StateScene);

        bool DropTargetMat(DragType type, Mesh *currEntity, Texture *otherData = nullptr);

        DLL_API extern Experimental::GameObject *m_Object;
        DLL_API extern char name[499];
        DLL_API extern char tag[499];
        DLL_API extern char layer[32];
        DLL_API extern entt::registry m_Registry;

        DLL_API extern std::vector<Mesh *> entities;
        DLL_API extern std::vector<Model> models;
        DLL_API extern Camera *mainCamera;
        DLL_API extern Camera *scene_camera;
        DLL_API extern bool stop_scripts;
        DLL_API extern std::vector<Camera *> cameras;
        DLL_API extern std::vector<Log> logs;
#ifdef _WIN32
#ifdef BUILD_DLL
        DLL_API extern "C" __declspec(dllimport) std::vector<Log> *GetLogs();
#else
        DLL_API extern "C" __declspec(dllexport) std::vector<Log> *GetLogs();
#endif
#endif
        DLL_API extern glm::mat4 projection;

        DLL_API extern std::vector<entt::entity> backup_entities;

        DLL_API extern std::vector<HyperAPI::PointLight *> PointLights;
        DLL_API extern std::vector<HyperAPI::Light2D *> Lights2D;
        DLL_API extern std::vector<HyperAPI::SpotLight *> SpotLights;
        DLL_API extern std::vector<HyperAPI::DirectionalLight *> DirLights;
        DLL_API extern std::vector<HyperAPI::Mesh *> hyperEntities;

        DLL_API extern b2World *world;
    } // namespace Scene
} // namespace HyperAPI
