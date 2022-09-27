
#include <iostream>
#include <random>
#include <memory>
#include "../src/lib/api.hpp"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

#ifndef _WIN32
#include <unistd.h>
#else
#include <direct.h>
#endif

using namespace HyperAPI;

class CollisionListener : public b2ContactListener {
public:
    void BeginContact(b2Contact *contact) {
        b2Fixture *fixtureA = contact->GetFixtureA();
        b2Fixture *fixtureB = contact->GetFixtureB();
        // getuser data
        b2BodyUserData &bodyUserDataA = fixtureA->GetBody()->GetUserData();
        b2BodyUserData &bodyUserDataB = fixtureB->GetBody()->GetUserData();

        Experimental::GameObject *gameObjectA = (Experimental::GameObject *)bodyUserDataA.pointer;
        Experimental::GameObject *gameObjectB = (Experimental::GameObject *)bodyUserDataB.pointer;

        if(gameObjectA->HasComponent<Experimental::NativeScriptManager>()) {
            auto &scriptManager = gameObjectA->GetComponent<Experimental::NativeScriptManager>();
            for(auto script : scriptManager.m_StaticScripts) {
                script->Collision2D(gameObjectB);
            }
        }

        if(gameObjectB->HasComponent<Experimental::NativeScriptManager>()) {
            auto &scriptManager = gameObjectB->GetComponent<Experimental::NativeScriptManager>();
            for(auto script : scriptManager.m_StaticScripts) {
                script->Collision2D(gameObjectA);
            }
        }

        if(gameObjectA->HasComponent<Experimental::m_LuaScriptComponent>()) {
            auto &scriptManager = gameObjectA->GetComponent<Experimental::m_LuaScriptComponent>();
            for(auto script : scriptManager.scripts) {
                script.Collision2D(gameObjectB);
            }
        }

        if(gameObjectB->HasComponent<Experimental::m_LuaScriptComponent>()) {
            auto &scriptManager = gameObjectB->GetComponent<Experimental::m_LuaScriptComponent>();
            for(auto script : scriptManager.scripts) {
                script.Collision2D(gameObjectA);
            }
        }
    }

    void EndContact(b2Contact *contact) {
        b2Fixture *fixtureA = contact->GetFixtureA();
        b2Fixture *fixtureB = contact->GetFixtureB();
        // getuser data
        b2BodyUserData &bodyUserDataA = fixtureA->GetBody()->GetUserData();
        b2BodyUserData &bodyUserDataB = fixtureB->GetBody()->GetUserData();

        Experimental::GameObject *gameObjectA = (Experimental::GameObject *)bodyUserDataA.pointer;
        Experimental::GameObject *gameObjectB = (Experimental::GameObject *)bodyUserDataB.pointer;

        if(gameObjectA->HasComponent<Experimental::NativeScriptManager>()) {
            auto &scriptManager = gameObjectA->GetComponent<Experimental::NativeScriptManager>();
            for(auto script : scriptManager.m_StaticScripts) {
                script->CollisionExit2D(gameObjectB);
            }
        }

        if(gameObjectB->HasComponent<Experimental::NativeScriptManager>()) {
            auto &scriptManager = gameObjectB->GetComponent<Experimental::NativeScriptManager>();
            for(auto script : scriptManager.m_StaticScripts) {
                script->CollisionExit2D(gameObjectA);
            }
        }

        if(gameObjectA->HasComponent<Experimental::m_LuaScriptComponent>()) {
            auto &scriptManager = gameObjectA->GetComponent<Experimental::m_LuaScriptComponent>();
            for(auto script : scriptManager.scripts) {
                script.CollisionExit2D(gameObjectB);
            }
        }

        if(gameObjectB->HasComponent<Experimental::m_LuaScriptComponent>()) {
            auto &scriptManager = gameObjectB->GetComponent<Experimental::m_LuaScriptComponent>();
            for(auto script : scriptManager.scripts) {
                script.CollisionExit2D(gameObjectA);
            }
        }
    }
};
CollisionListener *listener = new CollisionListener();

struct Config {
    char name[50];
    std::string mainScene;
    float ambientLight;
    int width, height;
};

struct AddComponentList {
    int selected = 0;
    int length = 1;
    bool showed;
    const char* items[1] = {
        "Lua Scripts Component"
    };
};

Config config = {
    "Static Engine",
    "assets/scenes/main.static",
    0.2
};

AddComponentList AddComponentConfig;

std::string originalName = "";
bool LoadState = false;

bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void DirIter(std::string path) {
    for (const auto & entry : fs::directory_iterator(path)) {
        if(fs::is_directory(entry)) {
            if(ImGui::TreeNodeEx((std::string(ICON_FA_FOLDER) + " " + entry.path().filename().string()).c_str(), ImGuiTreeNodeFlags_SpanAvailWidth)) {
                DirIter(entry.path().string());
                ImGui::TreePop();
            }
        } else {
            // offset for the arrow thing
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 25);
            ImGui::Selectable((std::string(ICON_FA_FILE) + " " + entry.path().filename().string()).c_str());
            // make it draggable
            if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                dirPayloadData = entry.path().string();
                ImGui::SetDragDropPayload("file", dirPayloadData.c_str(), strlen(dirPayloadData.c_str()));
                ImGui::Text(entry.path().filename().string().c_str());
                ImGui::EndDragDropSource();
            }
        }
    }
}

void ApplyMaterial(nlohmann::json JSON, Material &material, int i) {
    material.baseColor = glm::vec4(
        JSON[i]["material"]["baseColor"]["r"],
        JSON[i]["material"]["baseColor"]["g"],
        JSON[i]["material"]["baseColor"]["b"],
        1
    );

    material.roughness = JSON[i]["material"]["roughness"];
    material.metallic = JSON[i]["material"]["metallic"];
    material.texUVs = Vector2(JSON[i]["material"]["texUV"]["x"], JSON[i]["material"]["texUV"]["y"]);

    if(JSON[i]["material"]["diffuse"] != "nullptr") {
        std::string diffusePath = JSON[i]["material"]["diffuse"];
        material.diffuse = new Texture((char*)diffusePath.c_str(), 0, "texture_diffuse");
    }

    if(JSON[i]["material"]["specular"] != "nullptr") {
        std::string specularPath = JSON[i]["material"]["specular"];
        material.specular = new Texture((char*)specularPath.c_str(), 1, "texture_specular");
    }

    if(JSON[i]["material"]["normal"] != "nullptr") {
        std::string normalPath = JSON[i]["material"]["normal"];
        material.normal = new Texture((char*)normalPath.c_str(), 2, "texture_normal");
    }
}

namespace InspecType {
    enum Type {
        None,
        Material
    };
}

struct InspectorMaterial {
    std::string diffuse = "None";
    std::string specular = "None";
    std::string normal = "None";
    float metallic = 0;
    float roughness = 0;
    Vector4 baseColor = Vector4(1, 1, 1, 1);
    Vector2 texUVs = Vector2(0, 0);
};

int main() {
    // ScriptEngine::Init();

    char CWD[1024];
    #ifdef _WIN32
        _getcwd(CWD, sizeof(CWD));
    #else
        getcwd(CWD, sizeof(CWD));
    #endif
    cwd = std::string(CWD);

    // check if game.config exists
    std::ifstream file("assets/game.config");
    bool mainSceneFound = false;

    if(file.is_open()) {
        nlohmann::json JSON = nlohmann::json::parse(file);

        config.width = JSON["width"];
        config.height = JSON["height"];
        strcpy(config.name, ((std::string)JSON["name"]).c_str());
        config.ambientLight = JSON["ambientLight"];
        config.mainScene = JSON["mainScene"];
        mainSceneFound = true;
    } else {
        nlohmann::json j = {
            {"name", config.name},
            {"ambientLight", config.ambientLight},
            {"mainScene", config.mainScene},
            {"width", config.width},
            {"height", config.height}
        };

        std::ofstream o("assets/game.config");
        o << std::setw(4) << j << std::endl;
    }

    Hyper::Application app(config.width, config.height, (const char*)config.name);
    app.renderOnScreen = true;
    Input::window = app.renderer->window;
    // glfw enable sticky mouse buttons
    Shader shader("shaders/default.glsl");
    Shader spriteShader("shaders/sprite.glsl");
    Shader batchShader("shaders/batch.glsl");

    spriteShader.Bind();
    spriteShader.SetUniform1f("ambient", 1);

    shader.Bind();
    shader.SetUniform1f("ambient", 0.2);

    Skybox skybox("assets/skybox/right.jpg", "assets/skybox/left.jpg", "assets/skybox/top.jpg", "assets/skybox/bottom.jpg", "assets/skybox/front.jpg", "assets/skybox/back.jpg");
    Camera *camera = new Camera(false, app.width, app.height, Vector3(0,3,15));
    Scene::mainCamera = camera;

    bool focusedOnScene = false;
    bool hoveredScene = false;

    Vector2 winSize = Vector2(0,0);
    Vector2 winPos = Vector2(0,0);

    ScriptEngine::window = app.renderer->window;

    Mesh *currEntity = nullptr;
    PointLight *currPointLight = nullptr;
    SpotLight *currSpotLight = nullptr;
    Camera *selectedCamera = nullptr;
    DirectionalLight *currDirectionalLight = nullptr;
    int currModel = -1;

    bool openConfig = false;
    bool openDetails = false;
    bool openInspector = false;

    int inspectorType = InspecType::None;

    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, "", ImVec4(1,1,1, 1.0f), ICON_FA_FILE);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, "", ImVec4(1,1,1, 1.0f), ICON_FA_FOLDER);

    InspectorMaterial m_InspectorMaterial;

    Scene::LoadScene(config.mainScene);

    std::function<void(unsigned int &PPT, unsigned int &PPFBO)> GUI_EXP = 
    [&](unsigned int &PPT, unsigned int &PPFBO) {};

    bool calledOnce = false;

    InitScripts();

    for(auto &gameObject : Scene::m_GameObjects) {
        if(gameObject->HasComponent<Experimental::m_LuaScriptComponent>()) {
            gameObject->GetComponent<Experimental::m_LuaScriptComponent>().Start();
        }

        if(gameObject->HasComponent<Experimental::NativeScriptManager>()) {
            gameObject->GetComponent<Experimental::NativeScriptManager>().Start();
        }
    }

    Scene::world = new b2World({ 0.0, -5.8f });
    Scene::world->SetContactListener(listener);
    auto view = Scene::m_Registry.view<Experimental::Rigidbody2D>();
    
    for(auto e : view) {
        Experimental::GameObject *gameObject;
        for(auto &go : Scene::m_GameObjects) {
            if(go->entity == e) {
                gameObject = go;
                break;
            }
        }

        auto &transform = gameObject->GetComponent<Experimental::Transform>();
        auto &rb2d = gameObject->GetComponent<Experimental::Rigidbody2D>();

        b2BodyDef bodyDef;
        bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(gameObject);
        bodyDef.type = rb2d.type;
        bodyDef.position.Set(transform.position.x, transform.position.y);
        bodyDef.angle = glm::radians(transform.rotation.z);
        bodyDef.gravityScale = rb2d.gravityScale;

        b2Body *body = Scene::world->CreateBody(&bodyDef);
        body->SetFixedRotation(rb2d.fixedRotation);
        rb2d.body = body;

        if(gameObject->HasComponent<Experimental::BoxCollider2D>()) {
            auto &boxCollider2D = gameObject->GetComponent<Experimental::BoxCollider2D>();
            b2PolygonShape shape;
            shape.SetAsBox((((boxCollider2D.size.x) / 2) - 0.02) / 2, (((boxCollider2D.size.y) / 2) - 0.02) / 2);
            
            b2FixtureDef fixtureDef;
            fixtureDef.shape = &shape;
            fixtureDef.density = boxCollider2D.density;
            fixtureDef.friction = boxCollider2D.friction;
            fixtureDef.restitution = boxCollider2D.restitution;
            fixtureDef.restitutionThreshold = boxCollider2D.restitutionThreshold;
            boxCollider2D.fixture = body->CreateFixture(&fixtureDef);
        }
    }

    HyperAPI::isRunning = true;
    HyperAPI::isStopped = false;

    app.Run([&] {
        if(Scene::mainCamera == nullptr) {
            Scene::mainCamera = camera;
        }

        if(!calledOnce) {
            auto& colors = ImGui::GetStyle().Colors;
            colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.105f, 0.11f, 1.0f);

            colors[ImGuiCol_Header] = ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.305f, 0.3f, 1.0f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

            colors[ImGuiCol_Button] = ImVec4(0.6f, 0.2f, 0.2f, 1.0f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(1, 0.205f, 0.2f, 1.0f);
            colors[ImGuiCol_ButtonActive] = ImVec4(1, 0.305f, 0.3f, 1.0f);

            colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3, 0.305f, 0.3f, 1.0f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

            colors[ImGuiCol_Tab] = ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
            colors[ImGuiCol_TabHovered] = ImVec4(0.3f, 0.305f, 0.3f, 1.0f);
            colors[ImGuiCol_TabActive] = ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);
            colors[ImGuiCol_TabUnfocused] = ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
            colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

            colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.205f, 0.2f, 1.0f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.255f, 0.25f, 1.0f);
            colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.1505f, 0.15f, 1.0f);

            colors[ImGuiCol_ResizeGrip] = ImVec4(1, 0.15, 0.15, 1);
            colors[ImGuiCol_ResizeGripActive] = ImVec4(1, 0.30, 0.30, 1);
            colors[ImGuiCol_ResizeGripHovered] = ImVec4(1, 0.20, 0.20, 1);
            colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1, 0.15, 0.15, 1);

            colors[ImGuiCol_DockingPreview] = ImVec4(1, 0.15, 0.15, 1);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.5, 2.5));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
        }

        calledOnce = true;

        if(focusedOnScene && !camera->mode2D && Scene::mainCamera == camera) {
            camera->Inputs(app.renderer->window, winPos);
        }
        Input::winPos = Vector3(winPos.x, winPos.y, 0);
        Input::winSize = Vector3(winSize.x, winSize.y, 0);

        if(hoveredScene && camera->mode2D) {
            auto transform = camera->GetComponent<TransformComponent>();
            transform.rotation = glm::vec3(0.0f, 0.0f, -1.0f);
            camera->Inputs(app.renderer->window, winPos);
            camera->UpdateComponent(transform);
        }
        winSize = Vector2(app.width, app.height);

        for(auto &camera : Scene::cameras) {
            camera->updateMatrix(camera->cam_fov, camera->cam_near, camera->cam_far, winSize);
        }
        camera->updateMatrix(camera->cam_fov, camera->cam_near, camera->cam_far, winSize);
        skybox.Draw(*Scene::mainCamera, winSize.x, winSize.y);

        // floor.Draw(shader, *camera);
        shader.Bind();
        shader.SetUniform1f("ambient", config.ambientLight);
        shader.SetUniform1i("shadowMap", 17);
        shader.SetUniformMat4("lightProjection", Scene::projection);

        spriteShader.Bind();
        spriteShader.SetUniform1f("ambient", config.ambientLight);

        // Physics

        if(HyperAPI::isRunning && Scene::world != nullptr) {
            const int32_t velocityIterations = 6;
            const int32_t positionIterations = 2;
            Scene::world->Step(1.0f / 60.0f, velocityIterations, positionIterations);

            auto view = Scene::m_Registry.view<Experimental::Rigidbody2D>();
            for(auto e : view) {
                Experimental::GameObject *m_GameObject;
                for(auto &gameObject : Scene::m_GameObjects) {
                    if(gameObject->entity == e) {
                        m_GameObject = gameObject;
                    }
                }

                auto &transform = m_GameObject->GetComponent<Experimental::Transform>();
                auto &rigidbody = m_GameObject->GetComponent<Experimental::Rigidbody2D>();

                b2Body *body = (b2Body*)rigidbody.body;
                const auto &position = body->GetPosition();
                transform.position.x = position.x;
                transform.position.y = position.y;
                transform.rotation.z = glm::degrees(body->GetAngle());
            }
        }

        // Phyiscs

        for(auto &gameObject : Scene::m_GameObjects) {
            gameObject->Update();

            if(gameObject->HasComponent<Experimental::Transform>()) {
                gameObject->GetComponent<Experimental::Transform>().Update();
            }

            if(gameObject->HasComponent<Experimental::m_LuaScriptComponent>()) {
                gameObject->GetComponent<Experimental::m_LuaScriptComponent>().Update();
            }

            if(gameObject->HasComponent<Experimental::c_PointLight>()) {
                gameObject->GetComponent<Experimental::c_PointLight>().Update();
            }

            if(gameObject->HasComponent<Experimental::c_SpotLight>()) {
                gameObject->GetComponent<Experimental::c_SpotLight>().Update();
            }

            if(gameObject->HasComponent<Experimental::c_DirectionalLight>()) {
                gameObject->GetComponent<Experimental::c_DirectionalLight>().Update();
            }

            if(gameObject->HasComponent<Experimental::m_LuaScriptComponent>()) {
                auto &script = gameObject->GetComponent<Experimental::m_LuaScriptComponent>();
                if(HyperAPI::isRunning) {
                    script.Update();
                }
            }

            if(gameObject->HasComponent<Experimental::NativeScriptManager>()) {
                auto &script = gameObject->GetComponent<Experimental::NativeScriptManager>();
                if(HyperAPI::isRunning) {
                    script.Update();
                }
            }
            
            if(gameObject->HasComponent<Experimental::MeshRenderer>()) {
                // if(gameObject->GetComponcent<Experimental::MeshRenderer>().m_Model) continue;

                auto meshRenderer = gameObject->GetComponent<Experimental::MeshRenderer>();
                auto transform = gameObject->GetComponent<Experimental::Transform>();
                transform.Update();

                glm::mat4 extra = meshRenderer.extraMatrix;

                if(meshRenderer.m_Mesh != nullptr) {
                    if(transform.parentTransform != nullptr) {
                        transform.parentTransform->Update();
                        meshRenderer.m_Mesh->Draw(shader, *Scene::mainCamera, transform.transform * transform.parentTransform->transform * extra);
                    } else {
                        meshRenderer.m_Mesh->Draw(shader, *Scene::mainCamera, transform.transform * extra);
                    }
                }
            }

            if(gameObject->HasComponent<Experimental::SpriteRenderer>()) {
                auto spriteRenderer = gameObject->GetComponent<Experimental::SpriteRenderer>();
                auto transform = gameObject->GetComponent<Experimental::Transform>();
                transform.Update();

                spriteRenderer.mesh->Draw(shader, *Scene::mainCamera, transform.transform);
            }

            if(gameObject->HasComponent<Experimental::SpritesheetRenderer>()) {
                auto spritesheetRenderer = gameObject->GetComponent<Experimental::SpritesheetRenderer>();
                auto transform = gameObject->GetComponent<Experimental::Transform>();
                transform.Update();

                if(spritesheetRenderer.mesh != nullptr) {
                    spritesheetRenderer.mesh->Draw(shader, *Scene::mainCamera, transform.transform);
                }
            }

            if(gameObject->HasComponent<Experimental::SpriteAnimation>()) {
                auto spriteAnimation = gameObject->GetComponent<Experimental::SpriteAnimation>();
                auto transform = gameObject->GetComponent<Experimental::Transform>();
                transform.Update();

                spriteAnimation.Play();

                if(spriteAnimation.currMesh != nullptr) {
                    spriteAnimation.currMesh->Draw(shader, *Scene::mainCamera, transform.transform);
                }
            }
        }
    }, GUI_EXP);

    return 0;
}