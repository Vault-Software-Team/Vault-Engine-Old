#pragma once
#include "../vendor/luajit-2.1/lua.hpp"
#include <iostream>
#include <string>

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
    class ComponentSystem;
    namespace Experimental {
        class ComponentEntity;
        class GameObject;

    }
}

struct GLFWwindow;
namespace ScriptEngine {
    extern GLFWwindow *window;
    extern HyperAPI::ComponentSystem *m_EntityComp;
    extern HyperAPI::Experimental::GameObject *m_Object;
    extern std::string objID;
    
    lua_State* m_Init();

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
        int PlayAudio(lua_State *L);
        int PlayMusic(lua_State *L);
        int StopAudio(lua_State *L);
        int StopMusic(lua_State *L);
        int DisableResizing(lua_State *L);
    }

    class m_LuaScript {
    public:
        lua_State *L;
        int r;
        HyperAPI::Experimental::GameObject *m_GameObject;
        std::string ID;
        std::string pathToScript;
        m_LuaScript(const std::string &pathToScript);
        void Delete() {
            lua_close(L);
        }
        void Init();
        void Update(); 
        void Collision2D(HyperAPI::Experimental::GameObject *other);  
        void CollisionExit2D(HyperAPI::Experimental::GameObject *other);  
    };

    class LuaScript {
    public:
        lua_State *L;
        int r;
        HyperAPI::ComponentSystem *bindedComp;
        std::string pathToScript;
        LuaScript(const std::string &pathToScript);
        void Init();
        void Update();
    };

}