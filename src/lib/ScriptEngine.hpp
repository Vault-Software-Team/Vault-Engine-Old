#pragma once
#include "../../api/api.hpp"
#include <luajit-2.1/lua.hpp>

namespace ScriptEngine {
    extern lua_State *L;
    
    namespace Functions {
        int DebugLog(lua_State *L);
    }

    class LuaScript {
    public:
        std::string pathToScript;
        LuaScript(const std::string &pathToScript);
        void Init();
    };

    lua_State *Init();
}