#pragma once
#include "vendor/luajit-2.1/lua.hpp"
#include <iostream>
#include <string>

namespace HyperAPI {
    class ComponentSystem;
}


struct GLFWwindow;
namespace ScriptEngine {
    extern lua_State *L;
    extern GLFWwindow *window;
    extern HyperAPI::ComponentSystem *m_EntityComp;
    
    namespace Functions {
        int Log(lua_State *L);
        int LogWarning(lua_State *L);
        int LogError(lua_State *L);
        int GetComponent(lua_State *L);
        int UpdateComponent(lua_State *L);
        int GetKeyPress(lua_State *L);
        int GetKeyRelease(lua_State *L);
        int GetEntityComponent(lua_State *L);
        int UpdateEntityComponent(lua_State *L);
        int Transform_LookAt(lua_State *L);
    }

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