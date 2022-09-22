
#include <iostream>
#include <random>
#include <memory>
#include "../api/api.hpp"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

#ifndef _WIN32
#include <unistd.h>
#elif
// #include <direct.h>
#endif

using namespace HyperAPI;

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
bool CheckName(std::string &name, int &repName, bool recursive = false) {
    if(!recursive)
        originalName = name;

    for(auto e : Scene::entities) {
        if(e->name == name) {
            repName++;
            name = originalName + std::string(" (" + std::to_string(repName) + ")");
            CheckName(name, repName, true);
        }
    }

    for(auto &model : Scene::models) {
        for(auto mesh : model.meshes) {
            if(mesh->name == name) {
                repName++;
                name = originalName + std::string(" (" + std::to_string(repName) + ")");
                CheckName(name, repName, true);
            }
        }
    }

    for(auto &e : DirLights) {
       if(e->name == name) {
            repName++;
            name = originalName + std::string(" (" + std::to_string(repName) + ")");
            CheckName(name, repName, true);
        }
    }

    for(auto &e : SpotLights) {
       if(e->name == name) {
            repName++;
            name = originalName + std::string(" (" + std::to_string(repName) + ")");
            CheckName(name, repName, true);
        }
    }

    for(auto &e : PointLights) {
       if(e->name == name) {
            repName++;
            name = originalName + std::string(" (" + std::to_string(repName) + ")");
            CheckName(name, repName, true);
        }
    }

    for(auto &e : Scene::cameras) {
       if(e->name == name) {
            repName++;
            name = originalName + std::string(" (" + std::to_string(repName) + ")");
            CheckName(name, repName, true);
        }
    }
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

int main() {
    ScriptEngine::Init();

    char CWD[1024];
    getcwd(CWD, sizeof(CWD));
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

    Hyper::Application app(1280, 720, "Static Engine");
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

    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, "", ImVec4(1,1,1, 1.0f), ICON_FA_FILE);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, "", ImVec4(1,1,1, 1.0f), ICON_FA_FOLDER);

    Scene::LoadScene(config.mainScene);
    std::function<void(unsigned int &PPT, unsigned int &PPFBO)> GUI_EXP = 
    [&](unsigned int &PPT, unsigned int &PPFBO) {
        if(ImGui::BeginMainMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("Save Scene")) {
                    ImGuiFileDialog::Instance()->OpenDialog("SaveSceneDialog", ICON_FA_FLOPPY_DISK " Save Scene", ".static", ".");
                }

                if(ImGui::MenuItem("Config")) {
                    openConfig = true;
                }
                // if(ImGui::MenuItem("Build")) {
                    // ImGuiFileDialog::Instance()->OpenDialog("BuildDialog", "Build", nullptr, cwd + "/builds");
                // }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Info")) {
                if(ImGui::MenuItem("Details")) {
                    openDetails = true;
                }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (ImGuiFileDialog::Instance()->Display("SaveSceneDialog")) 
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                // remove cwd from filePathName
                filePathName.erase(0, cwd.length() + 1);
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                
                Scene::SaveScene(filePathName);
            }

            ImGuiFileDialog::Instance()->Close();
        }

        if(openConfig) {
            ImGui::OpenPopup("Edit Config");
            openConfig = false;
        }

        if(openDetails) {
            ImGui::OpenPopup("Details");
            openDetails = false;
        }

        ImGui::SetNextWindowSize(ImVec2(500, 0));
        if(ImGui::BeginPopup("Edit Config")) {
            ImGui::InputText("Game Name", config.name, 500);
            ImGui::DragFloat("Ambient Lightning", &config.ambientLight, 0.01f, 0, 1);
            ImGui::DragInt("Width", &config.width, 1, 0, 1920);
            ImGui::DragInt("Height", &config.height, 1, 0, 1080);
            if(ImGui::Button("Main Scene", ImVec2(500, 0))) {
                ImGuiFileDialog::Instance()->OpenDialog("ChooseMainScene", "Choose Main Scene", ".static", ".");
                // ImGui::CloseCurrentPopup();
            }
            ImGui::Separator();
            if(ImGui::Button("Save", ImVec2(500, 0))) {
                nlohmann::json j = {
                    {"name", config.name},
                    {"ambientLight", config.ambientLight},
                    {"mainScene", config.mainScene},
                    {"width", config.width},
                    {"height", config.height}
                };

                std::ofstream o("assets/game.config");
                o << std::setw(4) << j << std::endl;

                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGuiFileDialog::Instance()->Display("ChooseMainScene")) 
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                // remove cwd from filePathName
                filePathName.erase(0, cwd.length() + 1);
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                config.mainScene = filePathName;
            }

            
            // close
            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        // make window closeable

        if(ImGui::BeginPopup("Details")) {
            ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
            ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
            ImGui::Text("Version: %s", glGetString(GL_VERSION));
            ImGui::Text("Shading Language Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

            if(ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::End();
        }

        if(ImGui::Begin(ICON_FA_GAMEPAD" Scene")) {
            Scene::DropTargetMat(Scene::DRAG_SCENE, nullptr);

            ImVec2 w_s = ImGui::GetWindowSize();
            winSize = Vector2(w_s.x, w_s.y);
            // ImGui::SetWindowSize(ImVec2(w_s.x, w_s.y - 50));
            ImGui::BeginChild("View");
                if(camera->mode2D) {
                    if(ImGui::Button(ICON_FA_CAMERA " 3D View")) {
                        camera->mode2D = false;
                    }
                } else {
                    if(ImGui::Button(ICON_FA_CAMERA " 2D View")) {
                        camera->mode2D = true;
                    }
                }
                // ImGui::SameLine();
                // center the button
                // center it
                // if(!HyperAPI::isRunning) {
                //     ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - 25 / 2);
                //     if(ImGui::Button(ICON_FA_PLAY, ImVec2(25, 0))) {
                //         HyperAPI::isRunning = true;
                //         HyperAPI::isStopped = false;
                //         for(auto &camera : Scene::cameras) {
                //             if(camera->mainCamera) {
                //                 Scene::mainCamera = camera;
                //                 break;
                //             }
                //         }
                //     }
                // } else {
                //     // move it more left
                //     ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - 25 / 2 - (25/ 2));
                //     if(ImGui::Button(ICON_FA_PAUSE, ImVec2(25, 0))) {
                //         HyperAPI::isRunning = false;
                //         Scene::mainCamera = camera;
                //     }
                // }

                app.width = w_s.x;
                app.height = w_s.y;

                ImVec2 w_p = ImGui::GetWindowPos();
                winPos = Vector2(w_p.x, w_p.y);

                glActiveTexture(GL_TEXTURE15);
                glBindTexture(GL_TEXTURE_2D, PPT);

                // check window hovered
                if(ImGui::IsWindowHovered() && ImGui::IsMouseDragging(0)) {
                    focusedOnScene = true;
                } else {
                    focusedOnScene = false;
                }

                if(ImGui::IsWindowHovered()) {
                    hoveredScene = true;
                } else {
                    hoveredScene = false;
                }

                ImGui::Image((void*)PPT, ImVec2(w_s.x, w_s.y - 40), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::EndChild();
            ImGui::End();
        }
    
        if(ImGui::Begin(ICON_FA_CUBES " Hierarchy")) {
            Scene::DropTargetMat(Scene::DRAG_MODEL, nullptr);
            ImVec2 win_size = ImGui::GetWindowSize();

            if(ImGui::Button(ICON_FA_PLUS " Add GameObject", ImVec2(win_size.x, 0))) {
                Experimental::GameObject *go = new Experimental::GameObject();
                go->AddComponent<Experimental::Transform>();
            }

            for(int i = 0; i < Scene::m_GameObjects.size(); i++) {
                if(Scene::m_GameObjects[i]->parentID != "NO_PARENT") continue;
                Scene::m_GameObjects[i]->GUI();
            }

            ImGui::End();
        }

        if(ImGui::Begin(ICON_FA_SHARE_NODES " Components")) {
            if(Scene::m_Object != nullptr) {
                ImGui::InputText("Name", Scene::name, 500);
                Scene::m_Object->name = Scene::name;

                if(Scene::m_Object->HasComponent<Experimental::Transform>()) {
                    Scene::m_Object->GetComponent<Experimental::Transform>().GUI();
                }

                if(Scene::m_Object->HasComponent<Experimental::CameraComponent>()) {
                    Scene::m_Object->GetComponent<Experimental::CameraComponent>().GUI();
                }

                if(Scene::m_Object->HasComponent<Experimental::MeshRenderer>()) {
                    Scene::m_Object->GetComponent<Experimental::MeshRenderer>().GUI();
                }

                if(Scene::m_Object->HasComponent<Experimental::m_LuaScriptComponent>()) {
                    Scene::m_Object->GetComponent<Experimental::m_LuaScriptComponent>().GUI();
                }

                if(Scene::m_Object->HasComponent<Experimental::c_PointLight>()) {
                    Scene::m_Object->GetComponent<Experimental::c_PointLight>().GUI();
                }

                if(Scene::m_Object->HasComponent<Experimental::c_SpotLight>()) {
                    Scene::m_Object->GetComponent<Experimental::c_SpotLight>().GUI();
                }

                if(Scene::m_Object->HasComponent<Experimental::c_DirectionalLight>()) {
                    Scene::m_Object->GetComponent<Experimental::c_DirectionalLight>().GUI();
                }

                ImGui::Separator();

                ImVec2 win_size = ImGui::GetWindowSize();
                if(ImGui::Button(ICON_FA_PLUS " Add Component", ImVec2(win_size.x, 0))) {
                    ImGui::OpenPopup("Add Component");
                }
            }

            if(ImGui::BeginPopup("Add Component")) {
                if(ImGui::Button("Transform", ImVec2(200, 0))) {
                    Scene::m_Object->AddComponent<Experimental::Transform>();
                    ImGui::CloseCurrentPopup();
                }

                if(ImGui::Button("Mesh Renderer", ImVec2(200, 0))) {
                    Scene::m_Object->AddComponent<Experimental::MeshRenderer>();
                    ImGui::CloseCurrentPopup();
                }

                if(ImGui::Button("Lua Scripts", ImVec2(200, 0))) {
                    Scene::m_Object->AddComponent<Experimental::m_LuaScriptComponent>();
                    ImGui::CloseCurrentPopup();
                }

                if(ImGui::Button("Camera", ImVec2(200, 0))) {
                    // CameraComponent has one argument of type entt::entity
                    Scene::m_Object->AddComponent<Experimental::CameraComponent>(Scene::m_Object->entity);
                    ImGui::CloseCurrentPopup();
                }

                if(ImGui::Button("Point Light", ImVec2(200, 0))) {
                    Scene::m_Object->AddComponent<Experimental::c_PointLight>();
                    ImGui::CloseCurrentPopup();
                }

                if(ImGui::Button("Spot Light", ImVec2(200, 0))) {
                    Scene::m_Object->AddComponent<Experimental::c_SpotLight>();
                    ImGui::CloseCurrentPopup();
                }

                if(ImGui::Button("Directional Light", ImVec2(200, 0))) {
                    Scene::m_Object->AddComponent<Experimental::c_DirectionalLight>();
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            ImGui::End();
        }
        
        if(ImGui::Begin(ICON_FA_FOLDER " Assets")) {
            DirIter(cwd + std::string("/assets"));
            ImGui::End();
        }


        if(ImGui::Begin(ICON_FA_TERMINAL " Console")) {
            if(ImGui::Button(ICON_FA_TRASH " Clear")) {
                Scene::logs.clear();
            }

            ImGui::Separator();

            for(auto &log : Scene::logs) {
                log.GUI();
            }

            ImGui::End();
        }
    };

    bool calledOnce = false;

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

        if(hoveredScene && camera->mode2D && Scene::mainCamera == camera) {
            auto transform = camera->GetComponent<TransformComponent>();
            transform.rotation = glm::vec3(0.0f, 0.0f, -1.0f);
            camera->Inputs(app.renderer->window, winPos);
            camera->UpdateComponent(transform);
        }
        winSize = Vector2(app.width, app.height);

        for(auto &camera : Scene::cameras) {
            camera->updateMatrix(camera->fov, camera->near, camera->far, winSize);
        }
        camera->updateMatrix(camera->fov, camera->near, camera->far, winSize);
        skybox.Draw(*Scene::mainCamera, winSize.x, winSize.y);

        // floor.Draw(shader, *camera);
        shader.Bind();
        shader.SetUniform1f("ambient", config.ambientLight);
        shader.SetUniform1i("shadowMap", 17);
        shader.SetUniformMat4("lightProjection", Scene::projection);
        
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
                script.Update();
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
        }
    }, GUI_EXP);

    return 0;
}