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
//import glm helpers
#include "../vendor/box2d/box2d.h"
#include "../vendor/box2d/b2_world.h"
#include "../vendor/box2d/b2_body.h"
#include "../vendor/box2d/b2_fixture.h"
#include "../vendor/box2d/b2_polygon_shape.h"

#include "../vendor/assimp/postprocess.h"
#include "../vendor/SDL2/SDL_mixer.h"
#include <any>
#include <map>
#include "scene.hpp"

namespace HyperAPI {
    class Mesh;
    class Model;
    class Camera;
    struct Log;

    struct PointLight;
    struct SpotLight;
    struct DirectionalLight;
    struct Light2D;
    namespace Experimental {
        class GameObject;
    }

    namespace Scene {
        enum DragType {
            DRAG_DIFFUSE,
            DRAG_SPECULAR,
            DRAG_NORMAL,
            DRAG_MODEL,
            DRAG_SCENE,
            DRAG_SPRITE,
            DRAG_PREFAB,
            DRAG_GAMEOBJECT
        };
        extern std::vector<Experimental::GameObject*> m_GameObjects;
        extern std::map<std::string, bool> layers;
        extern std::string currentScenePath;
        void LoadScene(const std::string &scenePath);
        void LoadPrefab(const std::string &scenePath);
        void SavePrefab(const std::string &path, Experimental::GameObject *gameObject);
        void SaveScene(const std::string &path);

        void DropTargetMat(DragType type, Mesh *currEntity);

        extern Experimental::GameObject *m_Object;
        extern char name[499];
        extern char tag[499];
        extern char layer[32];
        extern entt::registry m_Registry;

        extern std::vector<Mesh*> entities;
        extern std::vector<Model> models;
        extern Camera *mainCamera;
        extern std::vector<Camera*> cameras;
        extern std::vector<Log> logs;
        extern glm::mat4 projection;

        extern std::vector<entt::entity> backup_entities;

        extern std::vector<HyperAPI::PointLight*> PointLights;
        extern std::vector<HyperAPI::Light2D*> Lights2D;
        extern std::vector<HyperAPI::SpotLight*> SpotLights;
        extern std::vector<HyperAPI::DirectionalLight*> DirLights;
        extern std::vector<HyperAPI::Mesh*> hyperEntities;

        extern b2World *world;
    }
}
