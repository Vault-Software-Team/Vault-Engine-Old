#include "scene.hpp"
#include "api.hpp"

bool G_END_WITH(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void ApplyTransform(HyperAPI::Experimental::Transform &transform, nlohmann::json JSON) {
    transform.position = Vector3(
        JSON["position"]["x"],
        JSON["position"]["y"],
        JSON["position"]["z"]
    );

    transform.rotation = Vector3(
        JSON["rotation"]["x"],
        JSON["rotation"]["y"],
        JSON["rotation"]["z"]
    );

    transform.scale = Vector3(
        JSON["scale"]["x"],
        JSON["scale"]["y"],
        JSON["scale"]["z"]
    );
}

namespace HyperAPI {
    namespace Scene {
        void LoadScene(const std::string &scenePath) {
            try {
                for(auto &gameObject : m_GameObjects) {
                    //delete entt entity
                    if(gameObject->HasComponent<Experimental::MeshRenderer>()) {
                        auto &meshRenderer = gameObject->GetComponent<Experimental::MeshRenderer>();
                        if(meshRenderer.m_Mesh != nullptr) {
                            // if(meshRenderer.m_Mesh->material.diffuse != nullptr) {
                            //     delete meshRenderer.m_Mesh->material.diffuse;
                            // }

                            // if(meshRenderer.m_Mesh->material.specular != nullptr) {
                            //     delete meshRenderer.m_Mesh->material.specular;
                            // }

                            // if(meshRenderer.m_Mesh->material.normal != nullptr) {
                            //     delete meshRenderer.m_Mesh->material.normal;
                            // }

                            meshRenderer.m_Mesh->material.textures.clear();
                            delete meshRenderer.m_Mesh;
                        }
                    }

                    m_Registry.destroy(gameObject->entity);
                    delete gameObject;
                }

                for(auto &light : DirLights) {
                    delete light;
                }
                DirLights.clear();

                for(auto &light : PointLights) {
                    delete light;
                }
                PointLights.clear();

                for(auto &light : SpotLights) {
                    delete light;
                }
                SpotLights.clear();

                for(auto &light : Lights2D) {
                    delete light;
                }
                Lights2D.clear();

                m_GameObjects.clear();
            }
            catch(...) {
            }

            std::ifstream file(scenePath);
            // get content
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            nlohmann::json JSON;
            JSON = nlohmann::json::parse(content);

            try {
                for(int i = 0; i < Scene::cameras.size(); i++) {
                    if(Scene::mainCamera == Scene::cameras[i]) {
                        Scene::mainCamera = nullptr;
                    }
                    delete Scene::cameras[i];
                }
                Scene::cameras.clear();
            } catch (std::exception &e) {
                std::cout << e.what() << std::endl;
            }


            for(int i = 0; i < JSON.size(); i++) {
                Experimental::GameObject *gameObject = new Experimental::GameObject();
                std::string name = JSON[i]["name"];
                std::string ID = JSON[i]["ID"];
                std::string parentID = JSON[i]["parentID"];
                std::string tag = JSON[i]["tag"];
                
                gameObject->name = name;
                gameObject->tag = tag;
                gameObject->ID = ID;
                gameObject->parentID = parentID;
                nlohmann::json components = JSON[i]["components"];

                std::string meshType = "";

                Experimental::Transform transform;

                for(auto &component : components) {
                    std::string type = component["type"];

                    if(type == "Transform") {
                        gameObject->AddComponent<Experimental::Transform>();
                        ApplyTransform(gameObject->GetComponent<Experimental::Transform>(), component);
                        transform.position = gameObject->GetComponent<Experimental::Transform>().position;
                        transform.rotation = gameObject->GetComponent<Experimental::Transform>().rotation;
                        transform.scale = gameObject->GetComponent<Experimental::Transform>().scale;
                    }

                    if(type == "MeshRenderer") {
                        gameObject->AddComponent<Experimental::MeshRenderer>();
                        auto &meshRenderer = gameObject->GetComponent<Experimental::MeshRenderer>();
                        nlohmann::json meshConfig = component["mesh"];

                        if(!meshConfig["custom"]) {
                            std::string meshType = meshConfig["mesh"];
                            meshRenderer.meshType = meshType;
                            
                            if(meshType == "Plane") {
                                meshRenderer.m_Mesh = Plane(Vector4(1,1,1,1)).m_Mesh;
                            }
                            if(meshType == "Cube") {
                                meshRenderer.m_Mesh = Cube(Vector4(1,1,1,1)).meshes[0];
                            }
                            if(meshType == "Sphere") {
                                meshRenderer.m_Mesh = Sphere(Vector4(1,1,1,1)).meshes[0];
                            }
                            if(meshType == "Cone") {
                                meshRenderer.m_Mesh = Cone(Vector4(1,1,1,1)).meshes[0];
                            }
                            if(meshType == "Capsule") {
                                meshRenderer.m_Mesh = Capsule(Vector4(1,1,1,1)).meshes[0];
                            }
                            if(meshType == "Torus") {
                                meshRenderer.m_Mesh = Torus(Vector4(1,1,1,1)).meshes[0];
                            }
                            if(meshType == "Cylinder") {
                                meshRenderer.m_Mesh = Cylinder(Vector4(1,1,1,1)).meshes[0];
                            }

                            if(component["material"] != "") {
                                std::ifstream file(component["material"]);
                                nlohmann::json JSON = nlohmann::json::parse(file);

                                const std::string diffuseTexture = JSON["diffuse"];
                                const std::string specularTexture = JSON["specular"];
                                const std::string normalTexture = JSON["normal"];
                                    
                                if(diffuseTexture != "nullptr") {
                                    if(meshRenderer.m_Mesh->material.diffuse != nullptr) {
                                        delete meshRenderer.m_Mesh->material.diffuse;
                                    }

                                    meshRenderer.m_Mesh->material.diffuse = new Texture(diffuseTexture.c_str(), 0, "texture_diffuse");
                                }

                                if(specularTexture != "nullptr") {
                                    if(meshRenderer.m_Mesh->material.specular != nullptr) {
                                        delete meshRenderer.m_Mesh->material.specular;
                                    }

                                    meshRenderer.m_Mesh->material.specular = new Texture(specularTexture.c_str(), 1, "texture_specular");
                                }

                                if(normalTexture != "nullptr") {
                                    if(meshRenderer.m_Mesh->material.normal != nullptr) {
                                        delete meshRenderer.m_Mesh->material.normal;
                                    }

                                    meshRenderer.m_Mesh->material.normal = new Texture(normalTexture.c_str(), 2, "texture_normal");
                                }

                                meshRenderer.m_Mesh->material.baseColor = Vector4(
                                    JSON["baseColor"]["r"],
                                    JSON["baseColor"]["g"],
                                    JSON["baseColor"]["b"],
                                    JSON["baseColor"]["a"]
                                );

                                meshRenderer.m_Mesh->material.roughness = JSON["roughness"];
                                meshRenderer.m_Mesh->material.metallic = JSON["metallic"];
                                meshRenderer.m_Mesh->material.texUVs = Vector2(JSON["texUV"]["x"], JSON["texUV"]["y"]);

                                meshRenderer.matPath = component["material"]; 
                            }
                        }
                        else {
                            meshType = meshConfig["mesh"];
                        }

                    }
                
                    if(type == "DirectionalLight") {
                        gameObject->AddComponent<Experimental::c_DirectionalLight>();
                        auto &light = gameObject->GetComponent<Experimental::c_DirectionalLight>();

                        light.color = Vector3(
                            component["color"]["r"],
                            component["color"]["g"],
                            component["color"]["b"]
                        );
                        
                        light.intensity = component["intensity"];
                    }

                    if(type == "PointLight") {
                        gameObject->AddComponent<Experimental::c_PointLight>();
                        auto &light = gameObject->GetComponent<Experimental::c_PointLight>();

                        light.color = Vector3(
                            component["color"]["r"],
                            component["color"]["g"],
                            component["color"]["b"]
                        );
                        
                        light.intensity = component["intensity"];
                    }

                    if(type == "SpotLight") {
                        gameObject->AddComponent<Experimental::c_SpotLight>();
                        auto &light = gameObject->GetComponent<Experimental::c_SpotLight>();

                        light.color = Vector3(
                            component["color"]["r"],
                            component["color"]["g"],
                            component["color"]["b"]
                        );
                    }

                    if(type == "CameraComponent") {
                        gameObject->AddComponent<Experimental::CameraComponent>();
                        auto &transformg = gameObject->GetComponent<Experimental::Transform>();
                        transformg.rotation = transform.rotation;
                        auto &camera = gameObject->GetComponent<Experimental::CameraComponent>();

                        camera.camera->cam_fov = component["fov"];
                        camera.camera->cam_near = component["near"];
                        camera.camera->cam_far = component["far"];
                        camera.camera->mainCamera = component["mainCamera"];
                        camera.camera->mode2D = component["mode2D"];

                        if(camera.camera->mainCamera) {
                            mainCamera = camera.camera;
                        }                        
                    }

                    if(type == "LuaScriptComponent") {
                        gameObject->AddComponent<Experimental::m_LuaScriptComponent>();
                        auto &m_script = gameObject->GetComponent<Experimental::m_LuaScriptComponent>();
                        for(auto &scriptPath : component["scripts"]) {
                            ScriptEngine::m_LuaScript script(scriptPath);
                            script.m_GameObject = gameObject;
                            script.ID = ID;
                            // script.Init();
                            m_script.scripts.push_back(script);
                        }
                    }
                }

                if(parentID != "NO_PARENT") {
                    Experimental::Transform childTransform;

                    delete gameObject;
                    //erase back
                    Scene::m_GameObjects.erase(Scene::m_GameObjects.begin() + Scene::m_GameObjects.size() - 1);
                    int amountOfChildren = 0;
                    std::vector<std::string> childNames;
                    for(int j = 0; j < Scene::m_GameObjects.size(); j++) {
                        if(Scene::m_GameObjects[j]->ID == parentID) {
                            Experimental::Model *newEntity = new Experimental::Model((char*)meshType.c_str(), false);
                            auto &m_Transform = newEntity->mainGameObject->GetComponent<Experimental::Transform>();

                            for(int g = 0; g < JSON.size(); g++) {
                                std::string p_ID = JSON[g]["ID"];
                                std::string name = JSON[g]["name"];

                                if(parentID == p_ID) {
                                    for(int b = 0; b < JSON.size(); b++) {
                                        std::string c_ID = JSON[b]["parentID"];
                                        std::string c_name = JSON[b]["name"];

                                        if(p_ID == c_ID) {
                                            childNames.push_back(c_name);
                                            amountOfChildren++;
                                        }
                                    }

                                    for(auto &component : JSON[g]["components"]) {
                                        std::string type = component["type"];

                                        if(type == "Transform") {
                                            ApplyTransform(m_Transform, component);
                                            newEntity->mainGameObject->name = name;
                                        }
                                    }
                                }
                            }

                            for(int a = 0; a < amountOfChildren; a++) {
                                newEntity->m_gameObjects[a]->name = childNames[a];
                                newEntity->m_gameObjects[a]->GetComponent<Experimental::Transform>().position = transform.position;
                                newEntity->m_gameObjects[a]->GetComponent<Experimental::Transform>().rotation = transform.rotation;
                                newEntity->m_gameObjects[a]->GetComponent<Experimental::Transform>().scale = transform.scale;
                            }

                            delete Scene::m_GameObjects[j];
                            Scene::m_GameObjects.erase(Scene::m_GameObjects.begin() + j);

                            break;
                        }
                    }
                }

            }
        }
        void SaveScene(const std::string &path) { 
            std::ofstream file(path);
            nlohmann::json JSON;

            int offset = 0;

            for(int i = 0; i < Scene::m_GameObjects.size(); i++) {
                auto &gameObject = Scene::m_GameObjects[i];

                std::string name = gameObject->name; 
                std::string ID = gameObject->ID; 
                std::string parentID = gameObject->parentID; 
                std::string tag = gameObject->tag;

                JSON[i]["name"] = name;
                JSON[i]["ID"] = ID;
                JSON[i]["tag"] = tag;
                JSON[i]["parentID"] = parentID;
                JSON[i]["components"] = nlohmann::json::array();

                int componentOffset = 0;
                if(gameObject->HasComponent<Experimental::Transform>()) {
                    auto &transform = gameObject->GetComponent<Experimental::Transform>();
                    JSON[i]["components"][componentOffset]["type"] = "Transform";
                    JSON[i]["components"][componentOffset]["position"]["x"] = transform.position.x;
                    JSON[i]["components"][componentOffset]["position"]["y"] = transform.position.y;
                    JSON[i]["components"][componentOffset]["position"]["z"] = transform.position.z;

                    JSON[i]["components"][componentOffset]["rotation"]["x"] = transform.rotation.x;
                    JSON[i]["components"][componentOffset]["rotation"]["y"] = transform.rotation.y;
                    JSON[i]["components"][componentOffset]["rotation"]["z"] = transform.rotation.z;

                    JSON[i]["components"][componentOffset]["scale"]["x"] = transform.scale.x;
                    JSON[i]["components"][componentOffset]["scale"]["y"] = transform.scale.y;
                    JSON[i]["components"][componentOffset]["scale"]["z"] = transform.scale.z;

                    componentOffset++;
                }

                if(gameObject->HasComponent<Experimental::MeshRenderer>()) {
                    auto &meshRenderer = gameObject->GetComponent<Experimental::MeshRenderer>();

                    JSON[i]["components"][componentOffset]["type"] = "MeshRenderer";
                    JSON[i]["components"][componentOffset]["mesh"]["custom"] = meshRenderer.m_Model;
                    JSON[i]["components"][componentOffset]["mesh"]["mesh"] = meshRenderer.meshType;
                    JSON[i]["components"][componentOffset]["material"] = meshRenderer.matPath;

                    componentOffset++;
                }

                if(gameObject->HasComponent<Experimental::c_DirectionalLight>()) {
                    auto &light = gameObject->GetComponent<Experimental::c_DirectionalLight>();

                    JSON[i]["components"][componentOffset]["type"] = "DirectionalLight";
                    JSON[i]["components"][componentOffset]["color"]["r"] = light.color.x;
                    JSON[i]["components"][componentOffset]["color"]["g"] = light.color.y;
                    JSON[i]["components"][componentOffset]["color"]["b"] = light.color.z;
                    JSON[i]["components"][componentOffset]["intensity"] = light.intensity;

                    componentOffset++;
                }

                if(gameObject->HasComponent<Experimental::c_PointLight>()) {
                    auto &light = gameObject->GetComponent<Experimental::c_PointLight>();

                    JSON[i]["components"][componentOffset]["type"] = "PointLight";
                    JSON[i]["components"][componentOffset]["color"]["r"] = light.color.x;
                    JSON[i]["components"][componentOffset]["color"]["g"] = light.color.y;
                    JSON[i]["components"][componentOffset]["color"]["b"] = light.color.z;
                    JSON[i]["components"][componentOffset]["intensity"] = light.intensity;

                    componentOffset++;
                }

                if(gameObject->HasComponent<Experimental::c_SpotLight>()) {
                    auto &light = gameObject->GetComponent<Experimental::c_SpotLight>();

                    JSON[i]["components"][componentOffset]["type"] = "SpotLight";
                    JSON[i]["components"][componentOffset]["color"]["r"] = light.color.x;
                    JSON[i]["components"][componentOffset]["color"]["g"] = light.color.y;
                    JSON[i]["components"][componentOffset]["color"]["b"] = light.color.z;

                    componentOffset++;
                }

                if(gameObject->HasComponent<Experimental::CameraComponent>()) {
                    std::cout << "Camera" << std::endl;
                    auto &camera = gameObject->GetComponent<Experimental::CameraComponent>();

                    JSON[i]["components"][componentOffset]["type"] = "CameraComponent";
                    JSON[i]["components"][componentOffset]["mainCamera"] = camera.camera->mainCamera;
                    JSON[i]["components"][componentOffset]["mode2D"] = camera.camera->mode2D;
                    JSON[i]["components"][componentOffset]["fov"] = camera.camera->cam_fov;
                    JSON[i]["components"][componentOffset]["near"] = camera.camera->cam_near;
                    JSON[i]["components"][componentOffset]["far"] = camera.camera->cam_far;

                    componentOffset++;
                }

                if(gameObject->HasComponent<Experimental::m_LuaScriptComponent>()) {
                    auto &script = gameObject->GetComponent<Experimental::m_LuaScriptComponent>();

                    JSON[i]["components"][componentOffset]["type"] = "LuaScriptComponent";
                    for(int scr = 0; scr < script.scripts.size(); scr++) {
                        JSON[i]["components"][componentOffset]["scripts"][scr] = script.scripts[scr].pathToScript;
                    }

                    componentOffset++;
                }
            }
            
            file << JSON;
        }

        void DropTargetMat(DragType type, Mesh *currEntity) {
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("file"))
                {
                    // const char* path = (const char*)payload->Data;
                    dirPayloadData.erase(0, cwd.length() + 1);

                    if(
                        G_END_WITH(dirPayloadData, ".png") ||
                        G_END_WITH(dirPayloadData, ".jpg") ||
                        G_END_WITH(dirPayloadData, ".jpeg")
                    ) {
                        switch (type)
                        {
                            case DRAG_DIFFUSE: {
                                if(currEntity->material.diffuse != nullptr) {
                                    glDeleteTextures(1, &currEntity->material.diffuse->ID);
                                }

                                currEntity->material.diffuse = new Texture((char*)dirPayloadData.c_str(), 0, "texture_diffuse");
                                break;
                            }
                            case DRAG_SPECULAR: {
                                if(currEntity->material.specular != nullptr) {
                                    glDeleteTextures(1, &currEntity->material.specular->ID);
                                }
                                currEntity->material.specular = new Texture((char*)dirPayloadData.c_str(), 1, "texture_specular");
                                break;
                            }
                            case DRAG_NORMAL: {
                                if(currEntity->material.normal != nullptr) {
                                    glDeleteTextures(1, &currEntity->material.normal->ID);
                                }
                                currEntity->material.normal = new Texture((char*)dirPayloadData.c_str(), 2, "texture_normal");
                                break;
                            }
                        }
                    }

                    if(
                        G_END_WITH(dirPayloadData, ".obj") ||
                        G_END_WITH(dirPayloadData, ".fbx") ||
                        G_END_WITH(dirPayloadData, ".gltf")
                    ) {
                        switch (type)
                        {
                            case DRAG_MODEL: {
                                Experimental::Model *newEntity = new Experimental::Model((char*)dirPayloadData.c_str(), false);
                                break;
                            }
                        }
                    }

                    if(
                        G_END_WITH(dirPayloadData, ".static") && type == DRAG_SCENE
                    ) {
                        LoadScene(dirPayloadData);
                    }
                }

                ImGui::EndDragDropTarget();
            }
        }

        entt::registry m_Registry;
        std::vector<Experimental::GameObject*> m_GameObjects = {};

        Experimental::GameObject *m_Object = nullptr;
        char name[499];
        char tag[499];

        std::vector<Mesh*> entities = {};
        std::vector<Model> models = {};
        Camera *mainCamera;
        std::vector<Camera*> cameras = {};
        std::vector<Log> logs = {};
        glm::mat4 projection = glm::mat4(1.0f);

        std::vector<entt::entity> backup_entities = {};

        std::vector<HyperAPI::PointLight*> PointLights = {};
        std::vector<HyperAPI::Light2D*> Lights2D = {};
        std::vector<HyperAPI::SpotLight*> SpotLights = {};
        std::vector<HyperAPI::DirectionalLight*> DirLights = {};
        std::vector<HyperAPI::Mesh*> hyperEntities = {};
    }
}

