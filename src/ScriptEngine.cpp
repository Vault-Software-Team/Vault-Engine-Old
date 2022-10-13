#include "lib/ScriptEngine.hpp"
#include "lib/api.hpp"
#include "lib/InputEvents.hpp"

// THIS ISNT WORKING YET
// BEACUSE OF ENTT SUPPORT THIS WONT WORK AT ALL DONT EVEN TRY

#define lua_pushglobaltable(L) lua_pushvalue(L,LUA_GLOBALSINDEX)

using namespace HyperAPI::Experimental;
using namespace HyperAPI;

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

void PushTableKey(lua_State *L, const char *key, bool value) {
    lua_pushstring(L, key);
    lua_pushboolean(L, value);
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

void GetTableArg(lua_State *L, const char *key, const char *value) {
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    strcpy((char *)value, lua_tostring(L, -1));
    lua_pop(L, 1);
}

void GetTableArg(lua_State *L, const char *key, float &value) {
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    value = lua_tonumber(L, -1);
    lua_pop(L, 1);
}

void GetTableArg(lua_State *L, const char *key, bool &value) {
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    value = lua_toboolean(L, -1);
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
        PushTableFunction(L, "LookAt", ScriptEngine::Functions::LookAt);
    }
}

void LuaUpdateComponentValues(lua_State *L, const std::string &type, GameObject *ent) {
    
    if(type == "Transform") {
        Transform &transform = ent->GetComponent<Transform>();

        GetTableArg(L, "positionX", transform.position.x);
        GetTableArg(L, "positionY", transform.position.y);
        GetTableArg(L, "positionZ", transform.position.z);

        GetTableArg(L, "rotationX", transform.rotation.x);
        GetTableArg(L, "rotationY", transform.rotation.y);
        GetTableArg(L, "rotationZ", transform.rotation.z);

        GetTableArg(L, "scaleX", transform.scale.x);
        GetTableArg(L, "scaleY", transform.scale.y);
        GetTableArg(L, "scaleZ", transform.scale.z);
    } else if(type == "MeshRenderer") {
        MeshRenderer &meshRenderer = ent->GetComponent<MeshRenderer>();
        if(meshRenderer.m_Mesh == nullptr) return;
        HyperAPI::Material &material = meshRenderer.m_Mesh->material;

        GetTableArg(L, "colorX", material.baseColor.x);
        GetTableArg(L, "colorY", material.baseColor.y);
        GetTableArg(L, "colorZ", material.baseColor.z);

        GetTableArg(L, "metallic", material.metallic);
        GetTableArg(L, "roughness", material.roughness);

        GetTableArg(L, "uvX", material.texUVs.x);
        GetTableArg(L, "uvY", material.texUVs.y);
    } else if(type == "DirectionalLight") {
        c_DirectionalLight &light = ent->GetComponent<c_DirectionalLight>();

        GetTableArg(L, "colorX", light.color.x);
        GetTableArg(L, "colorY", light.color.y);
        GetTableArg(L, "colorZ", light.color.z);
    } else if(type == "PointLight") {
        c_PointLight &light = ent->GetComponent<c_PointLight>();

        GetTableArg(L, "colorX", light.color.x);
        GetTableArg(L, "colorY", light.color.y);
        GetTableArg(L, "colorZ", light.color.z);
        GetTableArg(L, "intensity", light.intensity);
    } else if(type == "SpotLight") {
        c_SpotLight &light = ent->GetComponent<c_SpotLight>();

        GetTableArg(L, "colorX", light.color.x);
        GetTableArg(L, "colorY", light.color.y);
        GetTableArg(L, "colorZ", light.color.z);
    } else if(type == "SpriteRenderer") {
        auto &spriteRenderer = ent->GetComponent<SpriteRenderer>();

        GetTableArg(L, "colorX", spriteRenderer.mesh->material.baseColor.x);
        GetTableArg(L, "colorY", spriteRenderer.mesh->material.baseColor.y);
        GetTableArg(L, "colorZ", spriteRenderer.mesh->material.baseColor.z);
    }
    else if(type == "SpriteAnimation") {
        auto &spriteAnimation = ent->GetComponent<SpriteAnimation>();

        GetTableArg(L, "currAnim", spriteAnimation.currAnim);
    }
    else if(type == "SpritesheetAnimation") {
        auto &spritesheetAnimation = ent->GetComponent<c_SpritesheetAnimation>();

        GetTableArg(L, "currAnim", spritesheetAnimation.currAnim);
    }
    else if(type == "Rigidbody2D") {
        auto &rigidbody = ent->GetComponent<Rigidbody2D>();

        GetTableArg(L, "gravityScale", rigidbody.gravityScale);
        GetTableArg(L, "fixedRotation", rigidbody.fixedRotation);
    }
    else if(type == "BoxCollider2D") {
        auto &boxCollider = ent->GetComponent<BoxCollider2D>();

        GetTableArg(L, "sizeX", boxCollider.size.x);
        GetTableArg(L, "sizeY", boxCollider.size.y);
        GetTableArg(L, "offsetX", boxCollider.offset.x);
        GetTableArg(L, "offsetY", boxCollider.offset.y);
        GetTableArg(L, "density", boxCollider.density);
        GetTableArg(L, "friction", boxCollider.friction);
        GetTableArg(L, "restitution", boxCollider.restitution);
        GetTableArg(L, "restitutionThreshold", boxCollider.restitutionThreshold);
    }
    else if(type == "Rigidbody3D") {
        auto &rigidbody = ent->GetComponent<Rigidbody3D>();

        GetTableArg(L, "fixedRotation", rigidbody.fixedRotation);
        GetTableArg(L, "trigger", rigidbody.trigger);
        GetTableArg(L, "mass", rigidbody.mass);
    }
    else if(type == "BoxCollider3D") {
        auto &boxCollider = ent->GetComponent<BoxCollider3D>();

        GetTableArg(L, "sizeX", boxCollider.size.x);
        GetTableArg(L, "sizeY", boxCollider.size.y);
        GetTableArg(L, "sizeZ", boxCollider.size.z);
    }
    else if(type == "MeshCollider3D") {
        auto &meshCollider = ent->GetComponent<MeshCollider3D>();

        GetTableArg(L, "sizeX", meshCollider.size.x);
        GetTableArg(L, "sizeY", meshCollider.size.y);
        GetTableArg(L, "sizeZ", meshCollider.size.z);
    }
    else if(type == "CameraComponent") {
        auto &camera = ent->GetComponent<CameraComponent>();

        GetTableArg(L, "fov", camera.camera->cam_fov);
        GetTableArg(L, "near", camera.camera->cam_near);
        GetTableArg(L, "far", camera.camera->cam_far);
    }
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

namespace ScriptEngine {
    GLFWwindow *window = nullptr;
    HyperAPI::ComponentSystem *m_ComponentSystem = nullptr;
    HyperAPI::Experimental::GameObject *m_Object = nullptr;
    std::string objID = "";
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

    m_LuaScript::m_LuaScript(const std::string &pathToScript) {
        this->pathToScript = pathToScript;
    }

    void m_LuaScript::Init() {
        L = m_Init();
        r = luaL_dofile(L, pathToScript.c_str());

        lua_pushglobaltable(L);
        lua_pushnil(L);
        while (lua_next(L,-2) != 0) {
            std::string key = lua_tostring(L, -2);
            if(key.find("fx_") != std::string::npos) {
                // get the value of the key
                lua_getglobal(L, key.c_str());
                std::string value = lua_tostring(L, -1);
                lua_pop(L, 1);

                if(is_number(value)) {
                    float num = std::stof(std::string(m_Fields[key].value));
                    lua_pushnumber(L, num);
                } else {
                    lua_pushstring(L, m_Fields[key].value);
                }

                lua_setglobal(L, key.c_str());
            }
            lua_pop(L,1);
        }
        lua_pop(L,1);

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

    void m_LuaScript::GetFields() {
        lua_State *fxL = m_Init();
        int r = luaL_dofile(fxL, pathToScript.c_str());

        lua_pushglobaltable(fxL);
        lua_pushnil(fxL);
        while (lua_next(fxL,-2) != 0) {
            std::string key = lua_tostring(fxL, -2);
            if(key.find("fx_") != std::string::npos) {
                // get the value of the key
                lua_getglobal(fxL, key.c_str());
                std::string value = lua_tostring(fxL, -1);
                lua_pop(fxL, 1);

                auto it = m_Fields.find(key);
                if(it != m_Fields.end()) {
                    m_Fields[key].updated = true;
                } else {
                    m_FieldValue fv;
                    fv.updated = true;
                    strcpy(fv.value, value.c_str());

                    m_Fields[key] = fv;
                }
            }

            lua_pop(fxL,1);
        }

        for(auto &field : m_Fields) {
            if(field.second.updated) {
                field.second.updated = false;
            } else {
                m_Fields.erase(field.first);
                break;
            }
        }
        lua_pop(fxL,1);

        lua_close(fxL);
    }

    void m_LuaScript::Update() {
        // set global variable with name "timestep" that is a table.
        lua_newtable(L);
        lua_pushstring(L, "DeltaTime");
        lua_pushnumber(L, HyperAPI::Timestep::deltaTime);
        lua_settable(L, -3);
        lua_setglobal(L, "Timestep");

        objID = ID;
        for(int i = 0; i < HyperAPI::Scene::m_GameObjects.size(); i++) {
            if(HyperAPI::Scene::m_GameObjects[i]->ID == objID) {
                m_Object = HyperAPI::Scene::m_GameObjects[i];
            }
        }

        if(r == LUA_OK) {
            lua_getglobal(L, "OnUpdate");
            if(lua_isfunction(L, -1)) {
                lua_pcall(L, 0, 0, 0);
            }
        } else {
            std::string error = lua_tostring(L, -1);
            std::cout << error << std::endl;
        }

        lua_pushglobaltable(L);
        lua_pushnil(L);
        while (lua_next(L,-2) != 0) {
            std::string key = lua_tostring(L, -2);
            if(key.find("fx_") != std::string::npos) {
                // get the value of the key
                lua_getglobal(L, key.c_str());
                std::string value = lua_tostring(L, -1);
                lua_pop(L, 1);

                if(strcmp(m_Fields[key].value, value.c_str()) != 0) {
                    strcpy(m_Fields[key].value, value.c_str());
                } else {
                    if(is_number(value)) {
                        float num = std::stof(std::string(m_Fields[key].value));
                        lua_pushnumber(L, num);
                    } else {
                        lua_pushstring(L, m_Fields[key].value);
                    }

                    lua_setglobal(L, key.c_str());
                }
            }
            lua_pop(L,1);
        }
        lua_pop(L,1);
    }

    void m_LuaScript::Collision2D(GameObject *other) {
        lua_getglobal(L, "Collision2D");
        if(lua_isfunction(L, -1)) {
            lua_newtable(L);
            PushTableKey(L, "obj_id", other->ID.c_str());
            PushTableKey(L, "name", other->name.c_str());
            PushTableKey(L, "tag", other->tag.c_str());
            PushTableFunction(L, "GetComponent", Functions::GetEntComponent);
            PushTableFunction(L, "UpdateComponent", Functions::UpdateEntComponent);
            
            lua_pcall(L, 1, 0, 0);
        }
    }

    void m_LuaScript::CollisionExit2D(GameObject *other) {
        lua_getglobal(L, "CollisionExit2D");
        if(lua_isfunction(L, -1)) {
            lua_newtable(L);
            PushTableKey(L, "obj_id", other->ID.c_str());
            PushTableKey(L, "name", other->name.c_str());
            PushTableKey(L, "tag", other->tag.c_str());
            PushTableFunction(L, "GetComponent", Functions::GetEntComponent);
            PushTableFunction(L, "UpdateComponent", Functions::UpdateEntComponent);
            
            lua_pcall(L, 1, 0, 0);
        }
    }

    void m_LuaScript::Collision3D(GameObject *other) {
        lua_getglobal(L, "Collision3D");
        if(lua_isfunction(L, -1)) {
            lua_newtable(L);
            PushTableKey(L, "obj_id", other->ID.c_str());
            PushTableKey(L, "name", other->name.c_str());
            PushTableKey(L, "tag", other->tag.c_str());
            PushTableFunction(L, "GetComponent", Functions::GetEntComponent);
            PushTableFunction(L, "UpdateComponent", Functions::UpdateEntComponent);

            lua_pcall(L, 1, 0, 0);
        }
    }

    void m_LuaScript::CollisionExit3D(GameObject *other) {
        lua_getglobal(L, "CollisionExit3D");
        if(lua_isfunction(L, -1)) {
            lua_newtable(L);
            PushTableKey(L, "obj_id", other->ID.c_str());
            PushTableKey(L, "name", other->name.c_str());
            PushTableKey(L, "tag", other->tag.c_str());
            PushTableFunction(L, "GetComponent", Functions::GetEntComponent);
            PushTableFunction(L, "UpdateComponent", Functions::UpdateEntComponent);

            lua_pcall(L, 1, 0, 0);
        }
    }

    lua_State *m_Init() {
        lua_State *L = luaL_newstate();
        luaL_openlibs(L);

        {
            lua_pushnumber(L, -1);
            lua_setglobal(L, "KEY_UNKNOWN");  
            lua_pushnumber(L, 32);
            lua_setglobal(L, "KEY_SPACE");
            lua_pushnumber(L, 39);
            lua_setglobal(L, "KEY_APOSTROPHE");
            lua_pushnumber(L, 44);
            lua_setglobal(L, "KEY_COMMA");
            lua_pushnumber(L, 45);
            lua_setglobal(L, "KEY_MINUS");
            lua_pushnumber(L, 46);
            lua_setglobal(L, "KEY_PERIOD");
            lua_pushnumber(L, 47);
            lua_setglobal(L, "KEY_SLASH");
            lua_pushnumber(L, 48);
            lua_setglobal(L, "KEY_0");
            lua_pushnumber(L, 49);
            lua_setglobal(L, "KEY_1");
            lua_pushnumber(L, 50);
            lua_setglobal(L, "KEY_2");
            lua_pushnumber(L, 51);
            lua_setglobal(L, "KEY_3");
            lua_pushnumber(L, 52);
            lua_setglobal(L, "KEY_4");
            lua_pushnumber(L, 53);
            lua_setglobal(L, "KEY_5");
            lua_pushnumber(L, 54);
            lua_setglobal(L, "KEY_6");
            lua_pushnumber(L, 55);
            lua_setglobal(L, "KEY_7");
            lua_pushnumber(L, 56);
            lua_setglobal(L, "KEY_8");    
            lua_pushnumber(L, 57);
            lua_setglobal(L, "KEY_9");
            lua_pushnumber(L, 59);
            lua_setglobal(L, "KEY_SEMICOLON");
            lua_pushnumber(L, 61);
            lua_setglobal(L, "KEY_EQUAL");
            lua_pushnumber(L, 65);
            lua_setglobal(L, "KEY_A");                  
            lua_pushnumber(L, 66);
            lua_setglobal(L, "KEY_B");                  
            lua_pushnumber(L, 67);
            lua_setglobal(L, "KEY_C");                  
            lua_pushnumber(L, 68);
            lua_setglobal(L, "KEY_D");                  
            lua_pushnumber(L, 69);
            lua_setglobal(L, "KEY_E");                  
            lua_pushnumber(L, 70);
            lua_setglobal(L, "KEY_F");                  
            lua_pushnumber(L, 71);
            lua_setglobal(L, "KEY_G");                  
            lua_pushnumber(L, 72);
            lua_setglobal(L, "KEY_H");                  
            lua_pushnumber(L, 73);
            lua_setglobal(L, "KEY_I");                  
            lua_pushnumber(L, 74);
            lua_setglobal(L, "KEY_J");                  
            lua_pushnumber(L, 75);
            lua_setglobal(L, "KEY_K");                  
            lua_pushnumber(L, 76);
            lua_setglobal(L, "KEY_L");                  
            lua_pushnumber(L, 77);
            lua_setglobal(L, "KEY_M");                  
            lua_pushnumber(L, 78);
            lua_setglobal(L, "KEY_N");                  
            lua_pushnumber(L, 79);
            lua_setglobal(L, "KEY_O");                  
            lua_pushnumber(L, 80);
            lua_setglobal(L, "KEY_P");                  
            lua_pushnumber(L, 81);
            lua_setglobal(L, "KEY_Q");                  
            lua_pushnumber(L, 82);
            lua_setglobal(L, "KEY_R");                  
            lua_pushnumber(L, 83);
            lua_setglobal(L, "KEY_S");                  
            lua_pushnumber(L, 84);
            lua_setglobal(L, "KEY_T");                  
            lua_pushnumber(L, 85);
            lua_setglobal(L, "KEY_U");                  
            lua_pushnumber(L, 86);
            lua_setglobal(L, "KEY_V");                  
            lua_pushnumber(L, 87);
            lua_setglobal(L, "KEY_W");                  
            lua_pushnumber(L, 88);
            lua_setglobal(L, "KEY_X");                  
            lua_pushnumber(L, 89);
            lua_setglobal(L, "KEY_Y");                  
            lua_pushnumber(L, 90);lua_setglobal(L, "KEY_Z");
            lua_pushnumber(L, 91);
            lua_setglobal(L, "KEY_LEFT_BRACKET");
            lua_pushnumber(L, 92);
            lua_setglobal(L, "KEY_BACKSLASH");
            lua_pushnumber(L, 93);
            lua_setglobal(L, "KEY_RIGHT_BRACKET");
            lua_pushnumber(L, 96);
            lua_setglobal(L, "KEY_GRAVE_ACCENT");
            lua_pushnumber(L, 161);
            lua_setglobal(L, "KEY_WORLD_1");
            lua_pushnumber(L, 162);
            lua_setglobal(L, "KEY_WORLD_2");
            lua_pushnumber(L, 256);
            lua_setglobal(L, "KEY_ESCAPE");
            lua_pushnumber(L, 257);
            lua_setglobal(L, "KEY_ENTER");
            lua_pushnumber(L, 258);
            lua_setglobal(L, "KEY_TAB");
            lua_pushnumber(L, 259);
            lua_setglobal(L, "KEY_BACKSPACE");
            lua_pushnumber(L, 260);
            lua_setglobal(L, "KEY_INSERT");
            lua_pushnumber(L, 261);
            lua_setglobal(L, "KEY_DELETE");
            lua_pushnumber(L, 262);
            lua_setglobal(L, "KEY_RIGHT");
            lua_pushnumber(L, 263);
            lua_setglobal(L, "KEY_LEFT");
            lua_pushnumber(L, 264);
            lua_setglobal(L, "KEY_DOWN");
            lua_pushnumber(L, 265);
            lua_setglobal(L, "KEY_UP");
            lua_pushnumber(L, 266);
            lua_setglobal(L, "KEY_PAGE_UP");
            lua_pushnumber(L, 267);
            lua_setglobal(L, "KEY_PAGE_DOWN");
            lua_pushnumber(L, 268);
            lua_setglobal(L, "KEY_HOME");
            lua_pushnumber(L, 269);
            lua_setglobal(L, "KEY_END");
            lua_pushnumber(L, 280);
            lua_setglobal(L, "KEY_CAPS_LOCK");
            lua_pushnumber(L, 281);
            lua_setglobal(L, "KEY_SCROLL_LOCK");
            lua_pushnumber(L, 282);
            lua_setglobal(L, "KEY_NUM_LOCK");
            lua_pushnumber(L, 283);
            lua_setglobal(L, "KEY_PRINT_SCREEN");
            lua_pushnumber(L, 284);
            lua_setglobal(L, "KEY_PAUSE");
            lua_pushnumber(L, 290);
            lua_setglobal(L, "KEY_F1");
            lua_pushnumber(L, 291);
            lua_setglobal(L, "KEY_F2");
            lua_pushnumber(L, 292);
            lua_setglobal(L, "KEY_F3");
            lua_pushnumber(L, 293);
            lua_setglobal(L, "KEY_F4");
            lua_pushnumber(L, 294);
            lua_setglobal(L, "KEY_F5");
            lua_pushnumber(L, 295);
            lua_setglobal(L, "KEY_F6");
            lua_pushnumber(L, 296);
            lua_setglobal(L, "KEY_F7");
            lua_pushnumber(L, 297);
            lua_setglobal(L, "KEY_F8");
            lua_pushnumber(L, 298);
            lua_setglobal(L, "KEY_F9");
            lua_pushnumber(L, 299);
            lua_setglobal(L, "KEY_F10");
            lua_pushnumber(L, 300);
            lua_setglobal(L, "KEY_F11");
            lua_pushnumber(L, 301);
            lua_setglobal(L, "KEY_F12");
            lua_pushnumber(L, 302);
            lua_setglobal(L, "KEY_F13");
            lua_pushnumber(L, 303);
            lua_setglobal(L, "KEY_F14");
            lua_pushnumber(L, 304);
            lua_setglobal(L, "KEY_F15");
            lua_pushnumber(L, 305);
            lua_setglobal(L, "KEY_F16");
            lua_pushnumber(L, 306);
            lua_setglobal(L, "KEY_F17");
            lua_pushnumber(L, 307);
            lua_setglobal(L, "KEY_F18");
            lua_pushnumber(L, 308);
            lua_setglobal(L, "KEY_F19");
            lua_pushnumber(L, 309);
            lua_setglobal(L, "KEY_F20");
            lua_pushnumber(L, 310);
            lua_setglobal(L, "KEY_F21");
            lua_pushnumber(L, 311);
            lua_setglobal(L, "KEY_F22");
            lua_pushnumber(L, 312);
            lua_setglobal(L, "KEY_F23");
            lua_pushnumber(L, 313);
            lua_setglobal(L, "KEY_F24");
            lua_pushnumber(L, 314);
            lua_setglobal(L, "KEY_F25");
            lua_pushnumber(L, 320);
            lua_setglobal(L, "KEY_KP_0");
            lua_pushnumber(L, 321);
            lua_setglobal(L, "KEY_KP_1");
            lua_pushnumber(L, 322);
            lua_setglobal(L, "KEY_KP_2");
            lua_pushnumber(L, 323);
            lua_setglobal(L, "KEY_KP_3");
            lua_pushnumber(L, 324);
            lua_setglobal(L, "KEY_KP_4");
            lua_pushnumber(L, 325);
            lua_setglobal(L, "KEY_KP_5");
            lua_pushnumber(L, 326);
            lua_setglobal(L, "KEY_KP_6");
            lua_pushnumber(L, 327);
            lua_setglobal(L, "KEY_KP_7");
            lua_pushnumber(L, 328);
            lua_setglobal(L, "KEY_KP_8");
            lua_pushnumber(L, 329);
            lua_setglobal(L, "KEY_KP_9");
            lua_pushnumber(L, 330);
            lua_setglobal(L, "KEY_KP_DECIMAL");
            lua_pushnumber(L, 331);
            lua_setglobal(L, "KEY_KP_DIVIDE");
            lua_pushnumber(L, 332);
            lua_setglobal(L, "KEY_KP_MULTIPLY");
            lua_pushnumber(L, 333);
            lua_setglobal(L, "KEY_KP_SUBTRACT");
            lua_pushnumber(L, 334);
            lua_setglobal(L, "KEY_KP_ADD");
            lua_pushnumber(L, 335);
            lua_setglobal(L, "KEY_KP_ENTER");
            lua_pushnumber(L, 336);
            lua_setglobal(L, "KEY_KP_EQUAL");
            lua_pushnumber(L, 340);
            lua_setglobal(L, "KEY_LEFT_SHIFT");
            lua_pushnumber(L, 341);
            lua_setglobal(L, "KEY_LEFT_CONTROL");
            lua_pushnumber(L, 342);
            lua_setglobal(L, "KEY_LEFT_ALT");
            lua_pushnumber(L, 343);
            lua_setglobal(L, "KEY_LEFT_SUPER");
            lua_pushnumber(L, 344);
            lua_setglobal(L, "KEY_RIGHT_SHIFT");
            lua_pushnumber(L, 345);
            lua_setglobal(L, "KEY_RIGHT_CONTROL");
            lua_pushnumber(L, 346);
            lua_setglobal(L, "KEY_RIGHT_ALT");
            lua_pushnumber(L, 347);
            lua_setglobal(L, "KEY_RIGHT_SUPER");
            lua_pushnumber(L, 348);
            lua_setglobal(L, "KEY_MENU");
        }

        lua_register(L, "Log", Functions::Log);
        lua_register(L, "Warning", Functions::LogWarning);
        lua_register(L, "Error", Functions::LogError);
        lua_register(L, "GetComponent", Functions::GetComponent);
        lua_register(L, "UpdateComponent", Functions::UpdateComponent);
        lua_register(L, "HasComponent", Functions::HasComponent);

        lua_register(L, "FindGameObjectByName", Functions::FindGameObjectByName);
        lua_register(L, "FindGameObjectByTag", Functions::FindGameObjectByTag);
        lua_register(L, "FindGameObjectsByName", Functions::FindGameObjectsByName);
        lua_register(L, "FindGameObjectsByTag", Functions::FindGameObjectsByTag);
        lua_register(L, "InstantiatePrefab", Functions::InstantiatePrefab);
        lua_register(L, "InstantiateTransformPrefab", Functions::InstantiateTransformPrefab);
        lua_register(L, "PlayAudio", Functions::PlayAudio);
        lua_register(L, "StopAudio", Functions::StopAudio);
        lua_register(L, "PlayMusic", Functions::PlayMusic);
        lua_register(L, "StopMusic", Functions::StopMusic);
        lua_register(L, "ToDegrees", Functions::ToDegrees);
        lua_register(L, "ToRadians", Functions::ToRadians);

        lua_newtable(L);
        PushTableFunction(L, "IsKeyPressed", Functions::IsKeyPressed);
        PushTableFunction(L, "IsKeyReleased", Functions::IsKeyReleased);
        PushTableFunction(L, "IsMouseButtonPressed", Functions::IsMouseButtonPressed);
        PushTableFunction(L, "IsMouseButtonReleased", Functions::IsMouseButtonReleased);
        PushTableKey(L, "GetHorizontalAxis", &Functions::GetHorizontalAxis);
        PushTableKey(L, "GetVerticalAxis", &Functions::GetVerticalAxis);
        PushTableKey(L, "GetMouseXAxis", &Functions::GetMouseXAxis);
        PushTableKey(L, "GetMouseYAxis", &Functions::GetMouseYAxis);
        PushTableKey(L, "SetMouseHidden", &Functions::SetMouseHidden);
        PushTableKey(L, "SetMousePosition", &Functions::SetMousePosition);
        lua_setglobal(L, "Input");

        lua_newtable(L);
        PushTableFunction(L, "Lerp", &Functions::LerpFloat);
        lua_setglobal(L, "Float");

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
            if(m_Object == nullptr) return 1;
            if(type == "Transform") {
                Transform &transform = m_Object->GetComponent<Transform>();

                lua_newtable(L);
                PushTableKey(L, "obj_id", m_Object->ID.c_str());
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

                PushTableKey(L, "forwardX", transform.forward.x);
                PushTableKey(L, "forwardY", transform.forward.y);
                PushTableKey(L, "forwardZ", transform.forward.z);

                PushTableKey(L, "upX", transform.up.x);
                PushTableKey(L, "upY", transform.up.y);
                PushTableKey(L, "upZ", transform.up.z);

                PushTableKey(L, "rightX", transform.right.x);
                PushTableKey(L, "rightY", transform.right.y);
                PushTableKey(L, "rightZ", transform.right.z);

                PushTableFunction(L, "LookAt", LookAt);
            }
            else if(type == "MeshRenderer") {
                MeshRenderer &meshRenderer = m_Object->GetComponent<MeshRenderer>();
                HyperAPI::Material &material = meshRenderer.m_Mesh->material;

                lua_newtable(L);
                PushTableKey(L, "obj_id", m_Object->ID.c_str());
                PushTableKey(L, "type", "MeshRenderer");
                PushTableKey(L, "colorX", material.baseColor.x);
                PushTableKey(L, "colorY", material.baseColor.y);
                PushTableKey(L, "colorZ", material.baseColor.z);

                PushTableKey(L, "metallic", material.metallic);
                PushTableKey(L, "roughness", material.roughness);

                PushTableKey(L, "uvX", material.texUVs.x);
                PushTableKey(L, "uvY", material.texUVs.y);
            }
            else if(type == "DirectionalLight") {
                c_DirectionalLight &light = m_Object->GetComponent<c_DirectionalLight>();

                lua_newtable(L);
                PushTableKey(L, "obj_id", m_Object->ID.c_str());
                PushTableKey(L, "type", "DirectionalLight");
                PushTableKey(L, "colorX", light.color.x);
                PushTableKey(L, "colorY", light.color.y);
                PushTableKey(L, "colorZ", light.color.z);
            }
            else if(type == "PointLight") {
                c_PointLight &light = m_Object->GetComponent<c_PointLight>();

                lua_newtable(L);
                PushTableKey(L, "obj_id", m_Object->ID.c_str());
                PushTableKey(L, "type", "PointLight");
                PushTableKey(L, "colorX", light.color.x);
                PushTableKey(L, "colorY", light.color.y);
                PushTableKey(L, "colorZ", light.color.z);
                PushTableKey(L, "intensity", light.intensity);
            }
            else if(type == "SpotLight") {
                c_SpotLight &light = m_Object->GetComponent<c_SpotLight>();

                lua_newtable(L);
                PushTableKey(L, "obj_id", m_Object->ID.c_str());
                PushTableKey(L, "type", "SpotLight");
                PushTableKey(L, "colorX", light.color.x);
                PushTableKey(L, "colorY", light.color.y);
                PushTableKey(L, "colorZ", light.color.z);
            }
            else if(type == "SpriteRenderer") {
                auto &spriteRenderer = m_Object->GetComponent<SpriteRenderer>();

                lua_newtable(L);
                PushTableKey(L, "obj_id", m_Object->ID.c_str());
                PushTableKey(L, "type", "SpriteRenderer");
                PushTableKey(L, "colorX", spriteRenderer.mesh->material.baseColor.x);
                PushTableKey(L, "colorY", spriteRenderer.mesh->material.baseColor.y);
                PushTableKey(L, "colorZ", spriteRenderer.mesh->material.baseColor.z);
            }
            else if(type == "Rigidbody2D") {
                auto &rigidbody = m_Object->GetComponent<Rigidbody2D>();

                lua_newtable(L);
                PushTableKey(L, "obj_id", m_Object->ID.c_str());
                PushTableKey(L, "type", "Rigidbody2D");
                PushTableKey(L, "gravityScale", rigidbody.gravityScale);
                PushTableKey(L, "fixedRotation", rigidbody.fixedRotation);
                PushTableFunction(L, "SetVelocity", SetVelocity);
                PushTableFunction(L, "SetAngularVelocity", SetAngularVelocity);
                PushTableFunction(L, "SetPosition", SetPosition);
                PushTableFunction(L, "Force", Force);
                PushTableFunction(L, "Torque", Torque);
                PushTableFunction(L, "GetVelocity", GetVelocity);
            }
            else if(type == "BoxCollider2D") {
                auto &boxCollider = m_Object->GetComponent<BoxCollider2D>();

                lua_newtable(L);
                PushTableKey(L, "obj_id", m_Object->ID.c_str());
                PushTableKey(L, "type", "BoxCollider2D");
                PushTableKey(L, "sizeX", boxCollider.size.x);
                PushTableKey(L, "sizeY", boxCollider.size.y);
                PushTableKey(L, "offsetX", boxCollider.offset.x);
                PushTableKey(L, "offsetY", boxCollider.offset.y);
                PushTableKey(L, "density", boxCollider.density);
                PushTableKey(L, "friction", boxCollider.friction);
                PushTableKey(L, "restitution", boxCollider.restitution);
                PushTableKey(L, "restitutionThreshold", boxCollider.restitutionThreshold);
            }
            else if(type == "Rigidbody3D") {
                auto &rigidbody = m_Object->GetComponent<Rigidbody3D>();

                lua_newtable(L);
                PushTableKey(L, "obj_id", m_Object->ID.c_str());
                PushTableKey(L, "type", "Rigidbody3D");
                PushTableKey(L, "mass", rigidbody.mass);
                PushTableKey(L, "friction", rigidbody.friction);
                PushTableKey(L, "trigger", rigidbody.trigger);
                PushTableKey(L, "fixedRotation", rigidbody.fixedRotation);
                PushTableFunction(L, "AddForce", AddForce3D);
                PushTableFunction(L, "AddTorque", AddTorque3D);
                PushTableFunction(L, "AddForceAtPosition", AddForceAtPosition3D);
                PushTableFunction(L, "SetVelocity", SetVelocity3D);
                PushTableFunction(L, "SetAngularVelocity", SetAngularVelocity3D);
                PushTableFunction(L, "GetVelocity", GetVelocity3D);
                PushTableFunction(L, "GetAngularVelocity", GetAngularVelocity3D);
            }
            else if(type == "BoxCollider3D") {
                auto &collider = m_Object->GetComponent<BoxCollider3D>();

                lua_newtable(L);
                PushTableKey(L, "obj_id", m_Object->ID.c_str());
                PushTableKey(L, "type", "BoxCollider3D");
                PushTableKey(L, "sizeX", collider.size.x);
                PushTableKey(L, "sizeY", collider.size.y);
                PushTableKey(L, "sizeZ", collider.size.z);
            }
            else if(type == "MeshCollider3D") {
                auto &collider = m_Object->GetComponent<MeshCollider3D>();

                lua_newtable(L);
                PushTableKey(L, "obj_id", m_Object->ID.c_str());
                PushTableKey(L, "type", "BoxCollider3D");
                PushTableKey(L, "sizeX", collider.size.x);
                PushTableKey(L, "sizeY", collider.size.y);
                PushTableKey(L, "sizeZ", collider.size.z);
            }
            else if(type == "CameraComponent") {
                auto &cam = m_Object->GetComponent<CameraComponent>();

                lua_newtable(L);
                PushTableKey(L, "obj_id", m_Object->ID.c_str());
                PushTableKey(L, "type", "CameraComponent");
                // push an array key
                lua_pushstring(L, "layers");
                lua_newtable(L);
                for(auto &layer : cam.camera->layers) {
                    // string bruh
                    lua_pushstring(L, layer.c_str());
                    lua_rawseti(L, -2, layer.size());
                }
                lua_rawset(L, -3);
                PushTableKey(L, "fov", cam.camera->cam_fov);
                PushTableKey(L, "near", cam.camera->cam_near);
                PushTableKey(L, "far", cam.camera->cam_far);
                PushTableFunction(L, "MouseInput", MouseInput);
            }

            return 1;
        }

        int HasComponent(lua_State *L) {
            std::string type = (std::string)lua_tostring(L, 1);

            if(type == "Transform") {
                bool has = m_Object->HasComponent<Transform>();
                lua_pushboolean(L, has);
            } else {
                lua_pushboolean(L, false);
            }
            
            return 1;
        }

        int UpdateComponent(lua_State *L) {
            if(m_Object != nullptr) {
                // get table from the argument
                std::string type;
                GetTableArg(L, "type", type);
                std::string id;
                GetTableArg(L, "obj_id", id);

                for(auto &gameObject : HyperAPI::Scene::m_GameObjects) {
                    if(gameObject->ID == id) {
                        LuaUpdateComponentValues(L, type, gameObject);
                        break;
                    }
                }

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

        int IsKeyPressed(lua_State *L) {
            int key = (int)lua_tointeger(L, 1);
            bool pressed = HyperAPI::Input::IsKeyPressed(key);
            lua_pushboolean(L, pressed);

            return 1;
        }
        
        int IsKeyReleased(lua_State *L) {
            int key = (int)lua_tointeger(L, 1);
            bool released = HyperAPI::Input::IsKeyReleased(key);
            lua_pushboolean(L, released);

            return 1;
        }

        int IsMouseButtonPressed(lua_State *L) {
            int button = (int)lua_tointeger(L, 1);
            bool pressed = HyperAPI::Input::IsMouseButtonPressed(button);
            lua_pushboolean(L, pressed);

            return 1;
        }

        int IsMouseButtonReleased(lua_State *L) {
            int button = (int)lua_tointeger(L, 1);
            bool released = HyperAPI::Input::IsMouseButtonReleased(button);
            lua_pushboolean(L, released);

            return 1;
        }

        int GetHorizontalAxis(lua_State *L) {
            float axis = HyperAPI::Input::GetHorizontalAxis();
            lua_pushnumber(L, axis);

            return 1;
        }

        int GetVerticalAxis(lua_State *L) {
            float axis = HyperAPI::Input::GetVerticalAxis();
            lua_pushnumber(L, axis);

            return 1;
        }

        int GetMouseXAxis(lua_State *L) {
            float axis = HyperAPI::Input::GetMouseXAxis();
            lua_pushnumber(L, axis);

            return 1;
        }

        int GetMouseYAxis(lua_State *L) {
            float axis = HyperAPI::Input::GetMouseYAxis();
            lua_pushnumber(L, axis);

            return 1;
        }

        int SetMouseHidden(lua_State *L) {
            bool hidden = (bool)lua_toboolean(L, 1);
            HyperAPI::Input::SetMouseHidden(hidden);

            return 1;
        }

        int SetMousePosition(lua_State *L) {
            float x = (float)lua_tonumber(L, 1);
            float y = (float)lua_tonumber(L, 2);
            HyperAPI::Input::SetMousePosition(x, y);

            return 1;
        }

        int SetVelocity(lua_State *L) {
            float x = (float)lua_tonumber(L, 1);
            float y = (float)lua_tonumber(L, 2);
            m_Object->GetComponent<Rigidbody2D>().SetVelocity(x, y);

            return 1;
        }

        int SetAngularVelocity(lua_State *L) {
            float angularVelocity = (float)lua_tonumber(L, 1);
            m_Object->GetComponent<Rigidbody2D>().SetAngularVelocity(angularVelocity);

            return 1;
        }
        int SetPosition(lua_State *L) {
            float x = (float)lua_tonumber(L, 1);
            float y = (float)lua_tonumber(L, 2);
            m_Object->GetComponent<Rigidbody2D>().SetPosition(x, y);

            return 1;
        }

        int Force(lua_State *L) {
            lua_getfield(L, 1, "gravityScale");
            float gravityScale = (float)lua_tonumber(L, -1);
            lua_pop(L, 1);

            float x = (float)lua_tonumber(L, 2);
            float y = (float)lua_tonumber(L, 3);
            m_Object->GetComponent<Rigidbody2D>().Force(x, y);

            return 1;
        }
        int Torque(lua_State *L) {
            float torque = (float)lua_tonumber(L, 1);
            m_Object->GetComponent<Rigidbody2D>().Torque(torque);

            return 1;
        }
        int GetVelocity(lua_State *L) {
            auto &rigidbody = m_Object->GetComponent<Rigidbody2D>();

            lua_newtable(L);
            PushTableKey(L, "x", ((b2Body*)rigidbody.body)->GetLinearVelocity().x);
            PushTableKey(L, "y", ((b2Body*)rigidbody.body)->GetLinearVelocity().y);

            return 1;
        }

        int FindGameObjectByName(lua_State *L) {
            std::string name = (std::string)lua_tostring(L, 1);

            for(auto &gameObject : HyperAPI::Scene::m_GameObjects) {
                if(gameObject->name == name) {
                    lua_newtable(L);
                    PushTableKey(L, "obj_id", gameObject->ID.c_str());
                    PushTableKey(L, "name", gameObject->name.c_str());
                    PushTableKey(L, "tag", gameObject->tag.c_str());
                    PushTableFunction(L, "GetComponent", GetEntComponent);
                    PushTableFunction(L, "UpdateComponent", UpdateEntComponent);
                    PushTableFunction(L, "SetActive", SetActive);
                    PushTableKey(L, "active", gameObject->enabled);
                }
            }
            return 1;
        }

        int FindGameObjectByTag(lua_State *L) {
            std::string tag = (std::string)lua_tostring(L, 1);

            for(auto &gameObject : HyperAPI::Scene::m_GameObjects) {
                if(gameObject->tag == tag) {
                    lua_newtable(L);
                    PushTableKey(L, "obj_id", gameObject->ID.c_str());
                    PushTableKey(L, "name", gameObject->name.c_str());
                    PushTableKey(L, "tag", gameObject->tag.c_str());
                    PushTableFunction(L, "GetComponent", GetEntComponent);
                    PushTableFunction(L, "UpdateComponent", UpdateEntComponent);
                    PushTableFunction(L, "SetActive", SetActive);
                    PushTableKey(L, "active", gameObject->enabled);
                }
            }

            return 1;
        }

        int FindGameObjectsByName(lua_State *L) {
            std::string name = (std::string)lua_tostring(L, 1);

            lua_newtable(L);
            int i = 1;
            for(auto &gameObject : HyperAPI::Scene::m_GameObjects) {
                if(gameObject->name == name) {
                    lua_newtable(L);
                    PushTableKey(L, "obj_id", gameObject->ID.c_str());
                    PushTableKey(L, "name", gameObject->name.c_str());
                    PushTableKey(L, "tag", gameObject->tag.c_str());
                    PushTableFunction(L, "GetComponent", GetEntComponent);
                    PushTableFunction(L, "UpdateComponent", UpdateEntComponent);
                    PushTableFunction(L, "SetActive", SetActive);
                    PushTableKey(L, "active", gameObject->enabled);
                    lua_rawseti(L, -2, i);
                    i++;
                }
            }
            return 1;
        }

        int FindGameObjectsByTag(lua_State *L) {
            std::string tag = (std::string)lua_tostring(L, 1);

            lua_newtable(L);
            int i = 1;
            for(auto &gameObject : HyperAPI::Scene::m_GameObjects) {
                if(gameObject->tag == tag) {
                    lua_newtable(L);
                    PushTableKey(L, "obj_id", gameObject->ID.c_str());
                    PushTableKey(L, "name", gameObject->name.c_str());
                    PushTableKey(L, "tag", gameObject->tag.c_str());
                    PushTableFunction(L, "GetComponent", GetEntComponent);
                    PushTableFunction(L, "UpdateComponent", UpdateEntComponent);
                    PushTableFunction(L, "SetActive", SetActive);
                    PushTableKey(L, "active", gameObject->enabled);
                    lua_rawseti(L, -2, i);
                    i++;
                }
            }
            return 1;
        }

        int GetEntComponent(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string obj_id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);
            std::string type = (std::string)lua_tostring(L, 2);

            GameObject *gameObject;
            for(auto &obj : HyperAPI::Scene::m_GameObjects) {
                if(obj->ID == obj_id) {
                    gameObject = obj;
                    break;
                }
            }

            if(gameObject) {
                if(type == "Transform") {
                    Transform &transform = gameObject->GetComponent<Transform>();

                    lua_newtable(L);
                    PushTableKey(L, "obj_id", obj_id.c_str());
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
                }
                else if(type == "MeshRenderer") {
                    MeshRenderer &meshRenderer = gameObject->GetComponent<MeshRenderer>();
                    HyperAPI::Material &material = meshRenderer.m_Mesh->material;

                    lua_newtable(L);
                    PushTableKey(L, "obj_id", obj_id.c_str());
                    PushTableKey(L, "type", "MeshRenderer");
                    PushTableKey(L, "colorX", material.baseColor.x);
                    PushTableKey(L, "colorY", material.baseColor.y);
                    PushTableKey(L, "colorZ", material.baseColor.z);

                    PushTableKey(L, "metallic", material.metallic);
                    PushTableKey(L, "roughness", material.roughness);

                    PushTableKey(L, "uvX", material.texUVs.x);
                    PushTableKey(L, "uvY", material.texUVs.y);
                }
                else if(type == "DirectionalLight") {
                    c_DirectionalLight &light = gameObject->GetComponent<c_DirectionalLight>();

                    lua_newtable(L);
                    PushTableKey(L, "obj_id", obj_id.c_str());
                    PushTableKey(L, "type", "DirectionalLight");
                    PushTableKey(L, "colorX", light.color.x);
                    PushTableKey(L, "colorY", light.color.y);
                    PushTableKey(L, "colorZ", light.color.z);
                }
                else if(type == "PointLight") {
                    c_PointLight &light = gameObject->GetComponent<c_PointLight>();

                    lua_newtable(L);
                    PushTableKey(L, "obj_id", obj_id.c_str());
                    PushTableKey(L, "type", "PointLight");
                    PushTableKey(L, "colorX", light.color.x);
                    PushTableKey(L, "colorY", light.color.y);
                    PushTableKey(L, "colorZ", light.color.z);
                    PushTableKey(L, "intensity", light.intensity);
                }
                else if(type == "SpotLight") {
                    c_SpotLight &light = gameObject->GetComponent<c_SpotLight>();

                    lua_newtable(L);
                    PushTableKey(L, "obj_id", obj_id.c_str());
                    PushTableKey(L, "type", "SpotLight");
                    PushTableKey(L, "colorX", light.color.x);
                    PushTableKey(L, "colorY", light.color.y);
                    PushTableKey(L, "colorZ", light.color.z);
                }
                else if(type == "SpriteRenderer") {
                    auto &spriteRenderer = gameObject->GetComponent<SpriteRenderer>();

                    lua_newtable(L);
                    PushTableKey(L, "obj_id", obj_id.c_str());
                    PushTableKey(L, "type", "SpriteRenderer");
                    PushTableKey(L, "colorX", spriteRenderer.mesh->material.baseColor.x);
                    PushTableKey(L, "colorY", spriteRenderer.mesh->material.baseColor.y);
                    PushTableKey(L, "colorZ", spriteRenderer.mesh->material.baseColor.z);
                }
                else if(type == "SpriteAnimation") {
                    auto &spriteAnimation = gameObject->GetComponent<SpriteAnimation>();

                    lua_newtable(L);
                    PushTableKey(L, "obj_id", obj_id.c_str());
                    PushTableKey(L, "type", "SpriteAnimation");
                    PushTableKey(L, "currAnim", spriteAnimation.currAnim);
                }
                else if(type == "SpritesheetAnimation") {
                    auto &spritesheetAnimation = gameObject->GetComponent<c_SpritesheetAnimation>();

                    lua_newtable(L);
                    PushTableKey(L, "obj_id", obj_id.c_str());
                    PushTableKey(L, "type", "SpritesheetAnimation");
                    PushTableKey(L, "currAnim", spritesheetAnimation.currAnim);
                }
                else if(type == "Rigidbody2D") {
                    auto &rigidbody = gameObject->GetComponent<Rigidbody2D>();

                    lua_newtable(L);
                    PushTableKey(L, "obj_id", obj_id.c_str());
                    PushTableKey(L, "type", "Rigidbody2D");
                    PushTableKey(L, "gravityScale", rigidbody.gravityScale);
                    PushTableKey(L, "fixedRotation", rigidbody.fixedRotation);
                    PushTableFunction(L, "SetVelocity", SetVelocity);
                    PushTableFunction(L, "SetAngularVelocity", SetAngularVelocity);
                    PushTableFunction(L, "SetPosition", SetPosition);
                    PushTableFunction(L, "Force", Force);
                    PushTableFunction(L, "Torque", Torque);
                    PushTableFunction(L, "GetVelocity", GetVelocity);
                }
                else if(type == "BoxCollider2D") {
                    auto &boxCollider = gameObject->GetComponent<BoxCollider2D>();

                    lua_newtable(L);
                    PushTableKey(L, "obj_id", obj_id.c_str());
                    PushTableKey(L, "type", "BoxCollider2D");
                    PushTableKey(L, "sizeX", boxCollider.size.x);
                    PushTableKey(L, "sizeY", boxCollider.size.y);
                    PushTableKey(L, "offsetX", boxCollider.offset.x);
                    PushTableKey(L, "offsetY", boxCollider.offset.y);
                    PushTableKey(L, "density", boxCollider.density);
                    PushTableKey(L, "friction", boxCollider.friction);
                    PushTableKey(L, "restitution", boxCollider.restitution);
                    PushTableKey(L, "restitutionThreshold", boxCollider.restitutionThreshold);
                }
            }

            return 1;
        }
    
        int UpdateEntComponent(lua_State *L) {
            std::string type;
            GetTableArg(L, "type", type);
            std::string id;
            GetTableArg(L, "obj_id", id);

            for(auto &gameObject : HyperAPI::Scene::m_GameObjects) {
                if(gameObject->ID == id) {
                    LuaUpdateComponentValues(L, type, gameObject);
                    break;
                }
            }

            return 1;
        }

        int PlayAudio(lua_State *L) {
            std::string path = (std::string)lua_tostring(L, 1);
            float volume = (float)lua_tonumber(L, 2);
            bool loop = (bool)lua_toboolean(L, 3);
            // optional argument
            int channel = -1;
            if(lua_gettop(L) == 4) {
                channel = (int)lua_tonumber(L, 4);
            }

            HyperAPI::AudioEngine::PlaySound(path, volume, loop, channel);

            return 1;
        }

        int StopAudio(lua_State *L) {
            int channel = (int)lua_tonumber(L, 1);
            HyperAPI::AudioEngine::StopSound(channel);
            return 1;
        }

        int PlayMusic(lua_State *L) {
            std::string path = (std::string)lua_tostring(L, 1);
            float volume = (float)lua_tonumber(L, 2);
            bool loop = (bool)lua_toboolean(L, 3);

            HyperAPI::AudioEngine::PlayMusic(path, volume, loop);

            return 1;
        }

        int StopMusic(lua_State *L) {
            HyperAPI::AudioEngine::StopMusic();
            return 1;
        }

        int DisableResizing(lua_State *L) {
            glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
            return 1;
        }

        int LookAt(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            float x = (float)lua_tonumber(L, 1);
            float y = (float)lua_tonumber(L, 2);
            float z = (float)lua_tonumber(L, 3);

            for(auto &gameObject : HyperAPI::Scene::m_GameObjects) {
                if(gameObject->ID == id) {
                    auto &transform = gameObject->GetComponent<Transform>();
                    transform.LookAt(glm::vec3(x, y, z));
                    break;
                }
            }

            return 1;
        }

        int Translate(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            float x = (float)lua_tonumber(L, 1);
            float y = (float)lua_tonumber(L, 2);
            float z = (float)lua_tonumber(L, 3);

            for(auto &gameObject : HyperAPI::Scene::m_GameObjects) {
                if(gameObject->ID == id) {
                    auto &transform = gameObject->GetComponent<Transform>();
                    transform.Translate(glm::vec3(x, y, z));
                    break;
                }
            }
        }

        int Rotate(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            float x = (float)lua_tonumber(L, 1);
            float y = (float)lua_tonumber(L, 2);
            float z = (float)lua_tonumber(L, 3);

            for(auto &gameObject : HyperAPI::Scene::m_GameObjects) {
                if(gameObject->ID == id) {
                    auto &transform = gameObject->GetComponent<Transform>();
                    transform.Rotate(glm::vec3(x, y, z));
                    break;
                }
            }
        }

        int ChangeMeshDiffuse(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            auto texturePath = (std::string)lua_tostring(L, 1);

            for(auto &gameObject : HyperAPI::Scene::m_GameObjects) {
                if(gameObject->ID == id) {
                    auto &meshRenderer = gameObject->GetComponent<MeshRenderer>();
                    if(meshRenderer.m_Mesh->material.diffuse != nullptr) {
                        glDeleteTextures(1, &meshRenderer.m_Mesh->material.diffuse->ID);
                        delete meshRenderer.m_Mesh->material.diffuse;
                    }

                    meshRenderer.m_Mesh->material.diffuse = new HyperAPI::Texture(texturePath.c_str(), 0, "texture_diffuse");
                    break;
                }
            }
        }

        int ChangeMeshSpecular(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            auto texturePath = (std::string)lua_tostring(L, 1);

            for(auto &gameObject : HyperAPI::Scene::m_GameObjects) {
                if(gameObject->ID == id) {
                    auto &meshRenderer = gameObject->GetComponent<MeshRenderer>();
                    if(meshRenderer.m_Mesh->material.specular != nullptr) {
                        glDeleteTextures(1, &meshRenderer.m_Mesh->material.specular->ID);
                        delete meshRenderer.m_Mesh->material.specular;
                    }
                    
                    meshRenderer.m_Mesh->material.specular = new HyperAPI::Texture(texturePath.c_str(), 0, "texture_specular");
                    break;
                }
            }
        }

        int ChangeMeshNormal(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            auto texturePath = (std::string)lua_tostring(L, 1);

            for(auto &gameObject : HyperAPI::Scene::m_GameObjects) {
                if(gameObject->ID == id) {
                    auto &meshRenderer = gameObject->GetComponent<MeshRenderer>();
                    if(meshRenderer.m_Mesh->material.normal != nullptr) {
                        glDeleteTextures(1, &meshRenderer.m_Mesh->material.normal->ID);
                        delete meshRenderer.m_Mesh->material.normal;
                    }
                    
                    meshRenderer.m_Mesh->material.normal = new HyperAPI::Texture(texturePath.c_str(), 0, "texture_normal");
                    break;
                }
            }
        }

        int ChangeSpriteDiffuse(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            auto texturePath = (std::string)lua_tostring(L, 1);

            for(auto &gameObject : HyperAPI::Scene::m_GameObjects) {
                if(gameObject->ID == id) {
                    auto &spriteRenderer = gameObject->GetComponent<SpriteRenderer>();
                    if(spriteRenderer.mesh->material.diffuse != nullptr) {
                        glDeleteTextures(1, &spriteRenderer.mesh->material.diffuse->ID);
                        delete spriteRenderer.mesh->material.diffuse;
                    }
                    
                    spriteRenderer.mesh->material.diffuse = new HyperAPI::Texture(texturePath.c_str(), 0, "texture_diffuse");
                    break;
                }
            }
        }

        int ChangeSpritesheetDiffuse(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            auto texturePath = (std::string)lua_tostring(L, 1);

            for(auto &gameObject : HyperAPI::Scene::m_GameObjects) {
                if(gameObject->ID == id) {
                    auto &spriteRenderer = gameObject->GetComponent<SpriteRenderer>();
                    if(spriteRenderer.mesh->material.diffuse != nullptr) {
                        glDeleteTextures(1, &spriteRenderer.mesh->material.diffuse->ID);
                        delete spriteRenderer.mesh->material.diffuse;
                    }
                    
                    spriteRenderer.mesh->material.diffuse = new HyperAPI::Texture(texturePath.c_str(), 0, "texture_diffuse");
                    break;
                }
            }
        }

        int InstantiatePrefab(lua_State *L) {
            auto prefabPath = (std::string)lua_tostring(L, 1);
            HYPER_LOG("TEST TEST")
            HyperAPI::Scene::LoadPrefab(prefabPath);
            HYPER_LOG("TEST TEST")
            // GameObject *gameObject = HyperAPI::f_GameObject::InstantiatePrefab(prefabPath);


            // lua_newtable(L);
            // PushTableKey(L, "obj_id", gameObject->ID.c_str());
            // PushTableKey(L, "name", gameObject->name.c_str());
            // PushTableKey(L, "tag", gameObject->tag.c_str());
            // PushTableFunction(L, "GetComponent", GetEntComponent);
            // PushTableFunction(L, "UpdateComponent", UpdateEntComponent);
            return 1;
        }

        int InstantiateTransformPrefab(lua_State *L) {
            auto prefabPath = (std::string)lua_tostring(L, 1);
            Vector3 position = {lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)};
            Vector3 rotation = {lua_tonumber(L, 5), lua_tonumber(L, 6), lua_tonumber(L, 7)};
        
            GameObject *gameObject = HyperAPI::f_GameObject::InstantiatePrefab(prefabPath);
            gameObject->GetComponent<Transform>().position = position;
            gameObject->GetComponent<Transform>().rotation = rotation;
           
            lua_newtable(L);
            PushTableKey(L, "obj_id", gameObject->ID.c_str());
            PushTableKey(L, "name", gameObject->name.c_str());
            PushTableKey(L, "tag", gameObject->tag.c_str());
            PushTableFunction(L, "GetComponent", GetEntComponent);
            PushTableFunction(L, "UpdateComponent", UpdateEntComponent);

            return 1;
        }

        int SetVelocity3D(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            auto *gameObject = f_GameObject::FindGameObjectByID(id);

            if(gameObject != nullptr) {
                auto &rigidbody = gameObject->GetComponent<Rigidbody3D>();
                rigidbody.SetVelocity({lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)});
            }

            return 1;
        }

        int SetAngularVelocity3D(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            auto *gameObject = f_GameObject::FindGameObjectByID(id);

            if(gameObject != nullptr) {
                auto &rigidbody = gameObject->GetComponent<Rigidbody3D>();
                rigidbody.SetAngularVelocity({lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)});
            }

            return 1;
        }
        int AddForce3D(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            auto *gameObject = f_GameObject::FindGameObjectByID(id);

            if(gameObject != nullptr) {
                auto &rigidbody = gameObject->GetComponent<Rigidbody3D>();
                rigidbody.AddForce({lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)});
            }

            return 1;
        }
        int AddTorque3D(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            auto *gameObject = f_GameObject::FindGameObjectByID(id);

            if(gameObject != nullptr) {
                auto &rigidbody = gameObject->GetComponent<Rigidbody3D>();
                rigidbody.AddTorque({lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)});
            }

            return 1;
        }
        int GetVelocity3D(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            auto *gameObject = f_GameObject::FindGameObjectByID(id);

            if(gameObject != nullptr) {
                auto &rigidbody = gameObject->GetComponent<Rigidbody3D>();
                auto velocity = rigidbody.GetVelocity();
                lua_newtable(L);
                PushTableKey(L, "x", velocity.x);
                PushTableKey(L, "y", velocity.y);
                PushTableKey(L, "z", velocity.z);
                return 3;
            }

            return 1;
        }
        int GetAngularVelocity3D(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            auto *gameObject = f_GameObject::FindGameObjectByID(id);

            if(gameObject != nullptr) {
                auto &rigidbody = gameObject->GetComponent<Rigidbody3D>();
                auto velocity = rigidbody.GetAngularVelocity();
                lua_newtable(L);
                PushTableKey(L, "x", velocity.x);
                PushTableKey(L, "y", velocity.y);
                PushTableKey(L, "z", velocity.z);
                return 3;
            }

            return 1;
        }
        int AddForceAtPosition3D(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            auto *gameObject = f_GameObject::FindGameObjectByID(id);

            if(gameObject != nullptr) {
                auto &rigidbody = gameObject->GetComponent<Rigidbody3D>();
                rigidbody.AddForceAtPosition({lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)}, {lua_tonumber(L, 5), lua_tonumber(L, 6), lua_tonumber(L, 7)});
            }

            return 1;
        }

        int MouseInput(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            auto *gameObject = f_GameObject::FindGameObjectByID(id);

            if(gameObject != nullptr) {
                auto &cam = gameObject->GetComponent<CameraComponent>();
                cam.camera->m_MouseMovement = true;
            }

            return 1;
        }

        int ToDegrees(lua_State *L) {
            lua_pushnumber(L, glm::degrees(lua_tonumber(L, 1)));
            return 1;
        }

        int ToRadians(lua_State *L) {
            lua_pushnumber(L, glm::radians(lua_tonumber(L, 1)));
            return 1;
        }

        int SetActive(lua_State *L) {
            lua_getfield(L, 1, "obj_id");
            std::string id = (std::string)lua_tostring(L, -1);
            lua_pop(L, 1);

            auto *gameObject = f_GameObject::FindGameObjectByID(id);

            if(gameObject != nullptr) {
                gameObject->SetActive(lua_toboolean(L, 2));
            }

            return 1;
        }

        int LerpFloat(lua_State *L) {
            // get the 3 arguments
            float a = lua_tonumber(L, 1);
            float b = lua_tonumber(L, 2);
            float t = lua_tonumber(L, 3);

            float returnVal = Hyper::LerpFloat(a, b, t);
            lua_pushnumber(L, returnVal);
            return 1;
        }
    }
}