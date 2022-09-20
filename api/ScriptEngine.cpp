#include "ScriptEngine.hpp"
#include "api.hpp"

// THIS ISNT WORKING YET
// BEACUSE OF ENTT SUPPORT THIS WONT WORK AT ALL DONT EVEN TRY

void PushTableKey(lua_State *L, const char *key, const char *value) {
    lua_pushstring(L, key);
    lua_pushstring(L, value);
    lua_settable(L, -3);
}

void PushTableKey(lua_State *L, const char *key, float value) {
    lua_pushstring(L, key);
    lua_pushnumber(L, value);
    lua_settable(L, -3);
}

void PushTableFunction(lua_State *L, const char *name, lua_CFunction func) {
    lua_pushstring(L, name);
    lua_pushcfunction(L, func);
    lua_settable(L, -3);
}

void GetTableArg(lua_State *L, const char *key, std::string &value) {
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    value = lua_tostring(L, -1);
    lua_pop(L, 1);
}

void GetTableArg(lua_State *L, const char *key, float &value) {
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    value = lua_tonumber(L, -1);
    lua_pop(L, 1);
}

void LuaSetComponentValues(lua_State *L, const std::string &type, HyperAPI::ComponentSystem *ent) {
    ScriptEngine::m_EntityComp = ent;
    if(type == "Transform") {
        HyperAPI::TransformComponent transform = ent->GetComponent<HyperAPI::TransformComponent>();

        lua_newtable(L);
        PushTableKey(L, "type", "Transform");
        PushTableKey(L, "positionX", transform.position.x);
        PushTableKey(L, "positionY", transform.position.y);
        PushTableKey(L, "positionZ", transform.position.z);

        PushTableKey(L, "rotationX", transform.rotation.x);
        PushTableKey(L, "rotationY", transform.rotation.y);
        PushTableKey(L, "rotationZ", transform.rotation.z);

        PushTableKey(L, "scaleX", transform.scale.x);
        PushTableKey(L, "scaleY", transform.scale.y);
        PushTableKey(L, "scaleZ", transform.scale.z);
        PushTableFunction(L, "LookAt", ScriptEngine::Functions::Transform_LookAt);
    }
}

void LuaUpdateComponentValues(lua_State *L, const std::string &type, HyperAPI::ComponentSystem *ent) {
    HyperAPI::TransformComponent transform = ent->GetComponent<HyperAPI::TransformComponent>();
    
    GetTableArg(L, "positionX", transform.position.x);
    GetTableArg(L, "positionY", transform.position.y);
    GetTableArg(L, "positionZ", transform.position.z);

    GetTableArg(L, "rotationX", transform.rotation.x);
    GetTableArg(L, "rotationY", transform.rotation.y);
    GetTableArg(L, "rotationZ", transform.rotation.z);

    GetTableArg(L, "scaleX", transform.scale.x);
    GetTableArg(L, "scaleY", transform.scale.y);
    GetTableArg(L, "scaleZ", transform.scale.z);
    ent->UpdateComponent(transform);
    if(type == "Transform") {
    }
}

namespace ScriptEngine {
    lua_State *L = nullptr;
    GLFWwindow *window = nullptr;
    HyperAPI::ComponentSystem *m_ComponentSystem = nullptr;
    HyperAPI::ComponentSystem *m_EntityComp = nullptr;

    LuaScript::LuaScript(const std::string &pathToScript) {
        this->pathToScript = pathToScript;
    }

    void LuaScript::Init() {
        r = luaL_dofile(L, pathToScript.c_str());

        if(r == LUA_OK) {
            lua_getglobal(L, "OnStart");
            if(lua_isfunction(L, -1)) {
                lua_pcall(L, 0, 0, 0);
            }
        } else {
            std::string error = lua_tostring(L, -1);
            std::cout << error << std::endl;
        }
    }

    void LuaScript::Update() {
        m_ComponentSystem = bindedComp;

        if(r == LUA_OK) {
            lua_getglobal(L, "OnUpdate");
            if(lua_isfunction(L, -1)) {
                lua_pcall(L, 0, 0, 0);
            }
        } else {
            std::string error = lua_tostring(L, -1);
            std::cout << error << std::endl;
        }
    }

    lua_State *Init() {
        L = luaL_newstate();
        luaL_openlibs(L);

        lua_register(L, "Log", Functions::Log);
        lua_register(L, "Warning", Functions::LogWarning);
        lua_register(L, "Error", Functions::LogError);
        lua_register(L, "GetComponent", Functions::GetComponent);
        lua_register(L, "UpdateComponent", Functions::UpdateComponent);
        lua_register(L, "GetKeyPress", Functions::GetKeyPress);
        lua_register(L, "GetKeyRelease", Functions::GetKeyRelease);

        lua_register(L, "GetEntityComponent", Functions::GetEntityComponent);
        lua_register(L, "UpdateEntityComponent", Functions::UpdateEntityComponent);

        return L;
    }

    namespace Functions
    {
        int DebugLog(lua_State *L) {
            std::string a = (std::string)lua_tostring(L, 1);

            std::cout << "[STATIC] - " << a << std::endl;
            return 1;
        }

        int GetComponent(lua_State *L) {
            std::string type = (std::string)lua_tostring(L, 1);

            if(type == "Transform") {
                HyperAPI::TransformComponent transform = m_ComponentSystem->GetComponent<HyperAPI::TransformComponent>();

                lua_newtable(L);
                PushTableKey(L, "type", "Transform");
                PushTableKey(L, "positionX", transform.position.x);
                PushTableKey(L, "positionY", transform.position.y);
                PushTableKey(L, "positionZ", transform.position.z);

                PushTableKey(L, "rotationX", transform.rotation.x);
                PushTableKey(L, "rotationY", transform.rotation.y);
                PushTableKey(L, "rotationZ", transform.rotation.z);

                PushTableKey(L, "scaleX", transform.scale.x);
                PushTableKey(L, "scaleY", transform.scale.y);
                PushTableKey(L, "scaleZ", transform.scale.z);
                PushTableFunction(L, "LookAt", Functions::Transform_LookAt);
            }

            return 1;
        }

        int GetEntityComponent(lua_State *L) {
            std::string type = (std::string)lua_tostring(L, 1);
            std::string name = (std::string)lua_tostring(L, 2);

            for(auto &ent : HyperAPI::Scene::entities) {
                if(ent->name != name) continue;
                LuaSetComponentValues(L, type, ent);
            }

            for(auto &ent : HyperAPI::Scene::models) {
                if(ent.name != name) continue;
                LuaSetComponentValues(L, type, &ent);
            }

            for(auto &ent : DirLights) {
                if(ent->name != name) continue;
                LuaSetComponentValues(L, type, ent);
            }

            for(auto &ent : SpotLights) {
                if(ent->name != name) continue;
                LuaSetComponentValues(L, type, ent);
            }

            for(auto &ent : PointLights) {
                if(ent->name != name) continue;
                LuaSetComponentValues(L, type, ent);
            }

            return 1;
        }

        int UpdateEntityComponent(lua_State *L) {
            std::string type = "Transform";
            LuaUpdateComponentValues(L, type, m_EntityComp);

            return 1;
        }

        int UpdateComponent(lua_State *L) {
            if(m_ComponentSystem != nullptr) {
                // get table from the argument
                std::string type = "";
                GetTableArg(L, "type", type);
                
                LuaUpdateComponentValues(L, type, m_ComponentSystem);
            }
            return 1;
        }

        int Log(lua_State *L) {
            std::string a = (std::string)lua_tostring(L, 1);
            HyperAPI::Log log(a, HyperAPI::LOG_INFO);

            return 1;
        }

        int LogWarning(lua_State *L) {
            std::string a = (std::string)lua_tostring(L, 1);
            HyperAPI::Log log(a, HyperAPI::LOG_WARNING);

            return 1;
        }

        int LogError(lua_State *L) {
            std::string a = (std::string)lua_tostring(L, 1);
            HyperAPI::Log log(a, HyperAPI::LOG_ERROR);

            return 1;
        }

        int GetKeyPress(lua_State *L) {
            int a = (int)lua_tonumber(L, 1);

            if(glfwGetKey(window, a) == GLFW_PRESS) {
                lua_pushboolean(L, true);
            } else {
                lua_pushboolean(L, false);
            }

            return 1;
        }

        int GetKeyRelease(lua_State *L) {
            int a = (int)lua_tonumber(L, 1);

            if(glfwGetKey(window, a) == GLFW_RELEASE) {
                lua_pushboolean(L, true);
            } else {
                lua_pushboolean(L, false);
            }

            return 1;
        }

        int Transform_LookAt(lua_State *L) {
            HyperAPI::TransformComponent transform = m_ComponentSystem->GetComponent<HyperAPI::TransformComponent>();
            Vector3 target = Vector3((float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2), (float)lua_tonumber(L, 3));

            transform.LookAt(target);
            
            return 1;
        }
    }
}