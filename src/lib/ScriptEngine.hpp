#pragma once
#include <dllapi.hpp>
#include "../vendor/luajit-2.1/lua.hpp"
#include <iostream>
#include <string>
#include <map>
#include <memory>

// #ifdef _WIN32
// #ifdef BUILD_DLL
// #define   __declspec(dllexport)
// #else
// #define   __declspec(dllimport)
// #endif
// #else
// #define
// #endif

namespace HyperAPI {
    class DLL_API ComponentSystem;
    namespace Experimental {
        class DLL_API ComponentEntity;
        class DLL_API GameObject;

    } // namespace Experimental
} // namespace HyperAPI

struct DLL_API GLFWwindow;
namespace ScriptEngine {
    DLL_API extern GLFWwindow *window;
    DLL_API extern HyperAPI::ComponentSystem *m_EntityComp;
    DLL_API extern HyperAPI::Experimental::GameObject *m_Object;
    DLL_API extern std::string objID;

    namespace Functions {
        //  Rigidbody functions
        int SetVelocity(lua_State *L);
        int SetAngularVelocity(lua_State *L);
        int SetPosition(lua_State *L);
        int Force(lua_State *L);
        int Torque(lua_State *L);
        int GetVelocity(lua_State *L);

        // Transform functions
        int LookAt(lua_State *L);
        int Translate(lua_State *L);
        int Rotate(lua_State *L);
        int Scale(lua_State *L);

        // MeshRenderer Functions
        int ChangeMeshDiffuse(lua_State *L);
        int ChangeMeshSpecular(lua_State *L);
        int ChangeMeshNormal(lua_State *L);

        // SpriteRenderer Functions
        int ChangeSpriteDiffuse(lua_State *L);

        // SpritesheetRenderer Functions
        int ChangeSpritesheetDiffuse(lua_State *L);

        // Rigidbody3D Functions
        int SetVelocity3D(lua_State *L);
        int SetAngularVelocity3D(lua_State *L);
        int AddForce3D(lua_State *L);
        int AddTorque3D(lua_State *L);
        int GetVelocity3D(lua_State *L);
        int GetAngularVelocity3D(lua_State *L);
        int AddForceAtPosition3D(lua_State *L);
        int MoveByRotation3D(lua_State *L);

        // Camera Functions
        int MouseInput(lua_State *L);
        int ControllerInput(lua_State *L);

        // Controller Functions
        int SetCurrentController(lua_State *L);
        int GetLeftAnalogX(lua_State *L);
        int GetLeftAnalogY(lua_State *L);
        int GetRightAnalogX(lua_State *L);
        int GetRightAnalogY(lua_State *L);
        int GetL2(lua_State *L);
        int GetR2(lua_State *L);
        int IsButtonPressed(lua_State *L);
        int IsButtonReleased(lua_State *L);

        // Scene Functions
        int LoadScene(lua_State *L);

        // GameObject functions
        int SetActive(lua_State *L);

        int Log(lua_State *L);
        int LogWarning(lua_State *L);
        int LogError(lua_State *L);
        int GetComponent(lua_State *L);
        int HasComponent(lua_State *L);
        int UpdateComponent(lua_State *L);
        int IsKeyPressed(lua_State *L);
        int IsKeyReleased(lua_State *L);
        int IsMouseButtonPressed(lua_State *L);
        int IsMouseButtonReleased(lua_State *L);
        int GetHorizontalAxis(lua_State *L);
        int GetVerticalAxis(lua_State *L);
        int GetMouseXAxis(lua_State *L);
        int GetMouseYAxis(lua_State *L);
        int SetMouseHidden(lua_State *L);
        int SetMousePosition(lua_State *L);
        int GetEntComponent(lua_State *L);
        int UpdateEntComponent(lua_State *L);
        int FindGameObjectByName(lua_State *L);
        int FindGameObjectByTag(lua_State *L);
        int FindGameObjectsByName(lua_State *L);
        int FindGameObjectsByTag(lua_State *L);
        int InstantiatePrefab(lua_State *L);
        int InstantiatePrefabWithProperties(lua_State *L);
        int PlayAudio(lua_State *L);
        int PlayMusic(lua_State *L);
        int StopAudio(lua_State *L);
        int StopMusic(lua_State *L);
        int DisableResizing(lua_State *L);
        int ToDegrees(lua_State *L);
        int ToRadians(lua_State *L);
        int LerpFloat(lua_State *L);
    } // namespace Functions

    lua_State *m_Init();

    struct DLL_API m_FieldValue {
        char value[1024];
        bool updated = false;
    };

    class DLL_API m_LuaScript {
    public:
        std::map<std::string, m_FieldValue> m_Fields;
        lua_State *L;
        int r;
        HyperAPI::Experimental::GameObject *m_GameObject;
        std::string ID;
        std::string scriptName;

        // 1 / 60
        double fpsLimit = 1.0 / 60.0;
        double lastUpdateTime = 0;
        double lastFrameTime = 0;
        float timerStart = 0.00606060606; // 0.0166666667;
        float timer = 0.00606060606;      // 0.0166666667;

        std::string pathToScript;
        m_LuaScript(const std::string &pathToScript);
        void Delete() {
            lua_close(L);
        }
        void Init();
        void Update();
        void GetFields();
        void Collision2D(HyperAPI::Experimental::GameObject *other);
        void CollisionExit2D(HyperAPI::Experimental::GameObject *other);

        void Collision3D(HyperAPI::Experimental::GameObject *other);
        void CollisionExit3D(HyperAPI::Experimental::GameObject *other);
        void OnMouseEnter();
        void OnMouseExit();
    };

    class DLL_API LuaScript {
    public:
        lua_State *L;
        int r;
        HyperAPI::ComponentSystem *bindedComp;
        std::string pathToScript;
        LuaScript(const std::string &pathToScript);
        void Init();
        void Update();
    };

} // namespace ScriptEngine
