#pragma once
#include "vendor/luajit-2.1/lua.hpp"
#include <iostream>
#include <string>

namespace HyperAPI {
    class ComponentSystem;
    namespace Experimental {
        class ComponentEntity;
        class GameObject;

    }
}


struct GLFWwindow;
namespace ScriptEngine {
    extern lua_State *L;
    extern GLFWwindow *window;
    extern HyperAPI::ComponentSystem *m_EntityComp;
    extern HyperAPI::Experimental::GameObject *m_Object;
    extern std::string objID;
    
    namespace Functions {
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
    }

    class m_LuaScript {
    public:
        int r;
        HyperAPI::Experimental::GameObject *m_GameObject;
        std::string ID;
        std::string pathToScript;
        m_LuaScript(const std::string &pathToScript);
        void Init();
        void Update();   
    };

    class LuaScript {
    public:
        int r;
        HyperAPI::ComponentSystem *bindedComp;
        std::string pathToScript;
        LuaScript(const std::string &pathToScript);
        void Init();
        void Update();
    };

    lua_State *Init();
}