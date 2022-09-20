#pragma once
#include "vendor/glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
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
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "vendor/icons/icons.h"
#include "vendor/entt/entt.hpp"
#include <vector>
#include <functional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <box2d/box2d.h>
#include <assimp/postprocess.h>
#include <SDL2/SDL_mixer.h>
#include <tinyxml2.h>
#include <any>

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
            DRAG_SCENE
        };

        void DropTargetMat(DragType type, Mesh *currEntity);

        extern Experimental::GameObject *m_Object;
        extern char name[499];
        extern entt::registry m_Registry;
        extern std::vector<Experimental::GameObject*> m_GameObjects;

        extern std::vector<Mesh*> entities;
        extern std::vector<Model> models;
        extern Camera *mainCamera;
        extern std::vector<Camera*> cameras;
        extern std::vector<Log> logs;
        extern glm::mat4 projection;

        extern std::vector<Mesh*> backup_entities;
        extern std::vector<Model> backup_models;
        extern std::vector<Camera*> backup_cameras;

        extern std::vector<HyperAPI::PointLight*> PointLights;
        extern std::vector<HyperAPI::Light2D*> Lights2D;
        extern std::vector<HyperAPI::SpotLight*> SpotLights;
        extern std::vector<HyperAPI::DirectionalLight*> DirLights;
        extern std::vector<HyperAPI::Mesh*> hyperEntities;
    }
}
