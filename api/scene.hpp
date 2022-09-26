#pragma once
#pragma once
#include "vendor/glad/include/glad/glad.h"
#include "vendor/GLFW/glfw3.h"
#include <fstream>
#include <iostream>
#include "vendor/stb_image/stb_image.h"
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"
#include "vendor/glm/ext.hpp"
#include "vendor/glm/gtx/rotate_vector.hpp"
#include "vendor/glm/gtx/vector_angle.hpp"
#include "vendor/json/json.hpp"
// #include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "vendor/icons/icons.h"
#include "vendor/entt/entt.hpp"
#include "vendor/ImGuiFileDialog/ImGuiFileDialog.h"
#include <vector>
#include <functional>
#include <experimental/filesystem>
#include "vendor/assimp/Importer.hpp"
#include "vendor/assimp/scene.h"
//import glm helpers
#include "vendor/box2d/box2d.h"
#include "vendor/box2d/b2_world.h"
#include "vendor/box2d/b2_body.h"
#include "vendor/box2d/b2_fixture.h"
#include "vendor/box2d/b2_polygon_shape.h"

#include "vendor/assimp/postprocess.h"
#include "vendor/SDL2/SDL_mixer.h"
#include <any>
#include "scene.hpp"

#ifdef _WIN32
#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif
#else
#define DLL_EXPORT
#endif

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
            DRAG_SPRITE
        };
        extern DLL_EXPORT std::string currentScenePath;
        void DLL_EXPORT LoadScene(const std::string &scenePath);
        void DLL_EXPORT SaveScene(const std::string &path);

        void DLL_EXPORT DropTargetMat(DragType type, Mesh *currEntity);

        extern DLL_EXPORT Experimental::GameObject *m_Object;
        extern DLL_EXPORT char name[499];
        extern DLL_EXPORT char tag[499];
        extern DLL_EXPORT entt::registry m_Registry;
        extern DLL_EXPORT std::vector<Experimental::GameObject*> m_GameObjects;

        extern DLL_EXPORT std::vector<Mesh*> entities;
        extern DLL_EXPORT std::vector<Model> models;
        extern DLL_EXPORT Camera *mainCamera;
        extern DLL_EXPORT std::vector<Camera*> cameras;
        extern DLL_EXPORT std::vector<Log> logs;
        extern DLL_EXPORT glm::mat4 projection;

        extern DLL_EXPORT std::vector<entt::entity> backup_entities;

        extern DLL_EXPORT std::vector<HyperAPI::PointLight*> PointLights;
        extern DLL_EXPORT std::vector<HyperAPI::Light2D*> Lights2D;
        extern DLL_EXPORT std::vector<HyperAPI::SpotLight*> SpotLights;
        extern DLL_EXPORT std::vector<HyperAPI::DirectionalLight*> DirLights;
        extern DLL_EXPORT std::vector<HyperAPI::Mesh*> hyperEntities;

        extern DLL_EXPORT b2World *world;
    }
}
