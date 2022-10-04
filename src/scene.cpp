#include "lib/scene.hpp"
#include "lib/api.hpp"

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
        void SavePrefab(const std::string &path, Experimental::GameObject *gameObject) {
            std::ofstream file(path);
            nlohmann::json JSON;

            int offset = 0;

            std::string name = gameObject->name; 
            std::string ID = gameObject->ID; 
            std::string parentID = gameObject->parentID; 
            std::string tag = gameObject->tag;

            JSON[0]["name"] = name;
            JSON[0]["ID"] = ID;
            JSON[0]["tag"] = tag;
            JSON[0]["parentID"] = parentID;
            JSON[0]["components"] = nlohmann::json::array();

            int componentOffset = 0;
            if(gameObject->HasComponent<Experimental::Transform>()) {
                auto &transform = gameObject->GetComponent<Experimental::Transform>();
                JSON[0]["components"][componentOffset]["type"] = "Transform";
                JSON[0]["components"][componentOffset]["position"]["x"] = transform.position.x;
                JSON[0]["components"][componentOffset]["position"]["y"] = transform.position.y;
                JSON[0]["components"][componentOffset]["position"]["z"] = transform.position.z;

                JSON[0]["components"][componentOffset]["rotation"]["x"] = transform.rotation.x;
                JSON[0]["components"][componentOffset]["rotation"]["y"] = transform.rotation.y;
                JSON[0]["components"][componentOffset]["rotation"]["z"] = transform.rotation.z;

                JSON[0]["components"][componentOffset]["scale"]["x"] = transform.scale.x;
                JSON[0]["components"][componentOffset]["scale"]["y"] = transform.scale.y;
                JSON[0]["components"][componentOffset]["scale"]["z"] = transform.scale.z;

                componentOffset++;
            }

            if(gameObject->HasComponent<Experimental::MeshRenderer>()) {
                auto &meshRenderer = gameObject->GetComponent<Experimental::MeshRenderer>();

                JSON[0]["components"][componentOffset]["type"] = "MeshRenderer";
                JSON[0]["components"][componentOffset]["mesh"]["custom"] = meshRenderer.m_Model;
                JSON[0]["components"][componentOffset]["mesh"]["mesh"] = meshRenderer.meshType;
                JSON[0]["components"][componentOffset]["material"] = meshRenderer.matPath;

                componentOffset++;
            }

            if(gameObject->HasComponent<Experimental::c_DirectionalLight>()) {
                auto &light = gameObject->GetComponent<Experimental::c_DirectionalLight>();

                JSON[0]["components"][componentOffset]["type"] = "DirectionalLight";
                JSON[0]["components"][componentOffset]["color"]["r"] = light.color.x;
                JSON[0]["components"][componentOffset]["color"]["g"] = light.color.y;
                JSON[0]["components"][componentOffset]["color"]["b"] = light.color.z;
                JSON[0]["components"][componentOffset]["intensity"] = light.intensity;

                componentOffset++;
            }

            if(gameObject->HasComponent<Experimental::c_PointLight>()) {
                auto &light = gameObject->GetComponent<Experimental::c_PointLight>();

                JSON[0]["components"][componentOffset]["type"] = "PointLight";
                JSON[0]["components"][componentOffset]["color"]["r"] = light.color.x;
                JSON[0]["components"][componentOffset]["color"]["g"] = light.color.y;
                JSON[0]["components"][componentOffset]["color"]["b"] = light.color.z;
                JSON[0]["components"][componentOffset]["intensity"] = light.intensity;

                componentOffset++;
            }

            if(gameObject->HasComponent<Experimental::c_SpotLight>()) {
                auto &light = gameObject->GetComponent<Experimental::c_SpotLight>();

                JSON[0]["components"][componentOffset]["type"] = "SpotLight";
                JSON[0]["components"][componentOffset]["color"]["r"] = light.color.x;
                JSON[0]["components"][componentOffset]["color"]["g"] = light.color.y;
                JSON[0]["components"][componentOffset]["color"]["b"] = light.color.z;

                componentOffset++;
            }

            if(gameObject->HasComponent<Experimental::CameraComponent>()) {
                auto &camera = gameObject->GetComponent<Experimental::CameraComponent>();

                JSON[0]["components"][componentOffset]["type"] = "CameraComponent";
                JSON[0]["components"][componentOffset]["mainCamera"] = camera.camera->mainCamera;
                JSON[0]["components"][componentOffset]["mode2D"] = camera.camera->mode2D;
                JSON[0]["components"][componentOffset]["fov"] = camera.camera->cam_fov;
                JSON[0]["components"][componentOffset]["near"] = camera.camera->cam_near;
                JSON[0]["components"][componentOffset]["far"] = camera.camera->cam_far;

                componentOffset++;
            }

            if(gameObject->HasComponent<Experimental::m_LuaScriptComponent>()) {
                auto &script = gameObject->GetComponent<Experimental::m_LuaScriptComponent>();

                JSON[0]["components"][componentOffset]["type"] = "LuaScriptComponent";
                for(int scr = 0; scr < script.scripts.size(); scr++) {
                    JSON[0]["components"][componentOffset]["scripts"][scr] = script.scripts[scr].pathToScript;
                }

                componentOffset++;
            }

            if(gameObject->HasComponent<Experimental::SpriteRenderer>()) {
                auto &spriteRenderer = gameObject->GetComponent<Experimental::SpriteRenderer>();

                JSON[0]["components"][componentOffset]["type"] = "SpriteRenderer";
                JSON[0]["components"][componentOffset]["sprite"] = spriteRenderer.mesh->material.diffuse == nullptr ? "" : spriteRenderer.mesh->material.diffuse->texPath;
                JSON[0]["components"][componentOffset]["color"]["r"] = spriteRenderer.mesh->material.baseColor.x;
                JSON[0]["components"][componentOffset]["color"]["g"] = spriteRenderer.mesh->material.baseColor.y;
                JSON[0]["components"][componentOffset]["color"]["b"] = spriteRenderer.mesh->material.baseColor.z;

                componentOffset++;
            }

            if(gameObject->HasComponent<Experimental::Rigidbody2D>()) {
                auto &rigidbody = gameObject->GetComponent<Experimental::Rigidbody2D>();

                JSON[0]["components"][componentOffset]["type"] = "Rigidbody2D";
                JSON[0]["components"][componentOffset]["gravityScale"] = rigidbody.gravityScale;
                JSON[0]["components"][componentOffset]["bodyType"] = rigidbody.type;

                componentOffset++;
            }

            if(gameObject->HasComponent<Experimental::BoxCollider2D>()) {
                auto &collider = gameObject->GetComponent<Experimental::BoxCollider2D>();

                JSON[0]["components"][componentOffset]["type"] = "BoxCollider2D";
                JSON[0]["components"][componentOffset]["density"] = collider.density;
                JSON[0]["components"][componentOffset]["friction"] = collider.friction;
                JSON[0]["components"][componentOffset]["restitution"] = collider.restitution;
                JSON[0]["components"][componentOffset]["restitutionThreshold"] = collider.restitutionThreshold;
                JSON[0]["components"][componentOffset]["size"] = {
                    {"x", collider.size.x},
                    {"y", collider.size.y}
                };

                componentOffset++;
            }

            if(gameObject->HasComponent<Experimental::SpritesheetRenderer>()) {
                auto &spritesheetRenderer = gameObject->GetComponent<Experimental::SpritesheetRenderer>();

                JSON[0]["components"][componentOffset]["type"] = "SpritesheetRenderer";
                JSON[0]["components"][componentOffset]["color"]["r"] = spritesheetRenderer.material.baseColor.x;
                JSON[0]["components"][componentOffset]["color"]["g"] = spritesheetRenderer.material.baseColor.y;
                JSON[0]["components"][componentOffset]["color"]["b"] = spritesheetRenderer.material.baseColor.z;

                JSON[0]["components"][componentOffset]["spritesheetSize"] = {
                    {"x", spritesheetRenderer.spritesheetSize.x},
                    {"y", spritesheetRenderer.spritesheetSize.y}
                };

                JSON[0]["components"][componentOffset]["spriteSize"] = {
                    {"x", spritesheetRenderer.spriteSize.x},
                    {"y", spritesheetRenderer.spriteSize.y}
                };

                JSON[0]["components"][componentOffset]["spriteOffset"] = {
                    {"x", spritesheetRenderer.spriteOffset.x},
                    {"y", spritesheetRenderer.spriteOffset.y}
                };

                JSON[0]["components"][componentOffset]["spritesheet"] = spritesheetRenderer.material.diffuse == nullptr ? "" : spritesheetRenderer.material.diffuse->texPath;

                componentOffset++;
            }
        
            for(int i = 0; i < m_GameObjects.size(); i++) {
                auto &gameObject = m_GameObjects[i];
                if(gameObject->parentID != ID) continue;

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

                if(gameObject->HasComponent<Experimental::SpriteRenderer>()) {
                    auto &spriteRenderer = gameObject->GetComponent<Experimental::SpriteRenderer>();

                    JSON[i]["components"][componentOffset]["type"] = "SpriteRenderer";
                    JSON[i]["components"][componentOffset]["sprite"] = spriteRenderer.mesh->material.diffuse == nullptr ? "" : spriteRenderer.mesh->material.diffuse->texPath;
                    JSON[i]["components"][componentOffset]["color"]["r"] = spriteRenderer.mesh->material.baseColor.x;
                    JSON[i]["components"][componentOffset]["color"]["g"] = spriteRenderer.mesh->material.baseColor.y;
                    JSON[i]["components"][componentOffset]["color"]["b"] = spriteRenderer.mesh->material.baseColor.z;

                    componentOffset++;
                }

                if(gameObject->HasComponent<Experimental::Rigidbody2D>()) {
                    auto &rigidbody = gameObject->GetComponent<Experimental::Rigidbody2D>();

                    JSON[i]["components"][componentOffset]["type"] = "Rigidbody2D";
                    JSON[i]["components"][componentOffset]["gravityScale"] = rigidbody.gravityScale;
                    JSON[i]["components"][componentOffset]["bodyType"] = rigidbody.type;

                    componentOffset++;
                }

                if(gameObject->HasComponent<Experimental::BoxCollider2D>()) {
                    auto &collider = gameObject->GetComponent<Experimental::BoxCollider2D>();

                    JSON[i]["components"][componentOffset]["type"] = "BoxCollider2D";
                    JSON[i]["components"][componentOffset]["density"] = collider.density;
                    JSON[i]["components"][componentOffset]["friction"] = collider.friction;
                    JSON[i]["components"][componentOffset]["restitution"] = collider.restitution;
                    JSON[i]["components"][componentOffset]["restitutionThreshold"] = collider.restitutionThreshold;
                    JSON[i]["components"][componentOffset]["size"] = {
                        {"x", collider.size.x},
                        {"y", collider.size.y}
                    };

                    componentOffset++;
                }

                if(gameObject->HasComponent<Experimental::SpritesheetRenderer>()) {
                    auto &spritesheetRenderer = gameObject->GetComponent<Experimental::SpritesheetRenderer>();

                    JSON[i]["components"][componentOffset]["type"] = "SpritesheetRenderer";
                    JSON[i]["components"][componentOffset]["color"]["r"] = spritesheetRenderer.material.baseColor.x;
                    JSON[i]["components"][componentOffset]["color"]["g"] = spritesheetRenderer.material.baseColor.y;
                    JSON[i]["components"][componentOffset]["color"]["b"] = spritesheetRenderer.material.baseColor.z;

                    JSON[i]["components"][componentOffset]["spritesheetSize"] = {
                        {"x", spritesheetRenderer.spritesheetSize.x},
                        {"y", spritesheetRenderer.spritesheetSize.y}
                    };

                    JSON[i]["components"][componentOffset]["spriteSize"] = {
                        {"x", spritesheetRenderer.spriteSize.x},
                        {"y", spritesheetRenderer.spriteSize.y}
                    };

                    JSON[i]["components"][componentOffset]["spriteOffset"] = {
                        {"x", spritesheetRenderer.spriteOffset.x},
                        {"y", spritesheetRenderer.spriteOffset.y}
                    };

                    JSON[i]["components"][componentOffset]["spritesheet"] = spritesheetRenderer.material.diffuse == nullptr ? "" : spritesheetRenderer.material.diffuse->texPath;

                    componentOffset++;
                }
            }
        
            file << JSON;
        }

        void LoadScene(const std::string &scenePath) {
            LoadingScene = true;

            currentScenePath = scenePath;
            try {
                for(auto &gameObject : m_GameObjects) {
                    //delete entt entity
                    if(gameObject->HasComponent<Experimental::MeshRenderer>()) {
                        auto &meshRenderer = gameObject->GetComponent<Experimental::MeshRenderer>();
                        if(meshRenderer.m_Mesh != nullptr) {
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
                std::string layer = JSON[i]["layer"];

                gameObject->name = name;
                gameObject->tag = tag;
                gameObject->ID = ID;
                gameObject->parentID = parentID;
                gameObject->layer = layer;
                nlohmann::json components = JSON[i]["components"];

                std::string meshType = "";

                Experimental::Transform transform;
                m_GameObjects.push_back(gameObject);

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

                    if(type == "SpriteRenderer") {
                        gameObject->AddComponent<Experimental::SpriteRenderer>();
                        auto &spriteRenderer = gameObject->GetComponent<Experimental::SpriteRenderer>();
                        if(component["sprite"] != "") {
                            spriteRenderer.mesh->material.diffuse = new Texture(((std::string)component["sprite"]).c_str(), 0, "texture_diffuse");
                            spriteRenderer.mesh->material.baseColor = Vector4(
                                component["color"]["r"],
                                component["color"]["g"],
                                component["color"]["b"],
                                1
                            );
                        }
                    }

                    if(type == "Rigidbody2D") {
                        gameObject->AddComponent<Experimental::Rigidbody2D>();
                        auto &rigidbody = gameObject->GetComponent<Experimental::Rigidbody2D>();

                        rigidbody.type = component["bodyType"];
                        rigidbody.gravityScale = component["gravityScale"];
                        rigidbody.fixedRotation = component["fixedRotation"];
                    }

                    if(type == "BoxCollider2D") {
                        gameObject->AddComponent<Experimental::BoxCollider2D>();
                        auto &boxCollider = gameObject->GetComponent<Experimental::BoxCollider2D>();

                        boxCollider.size = Vector2(
                            component["size"]["x"],
                            component["size"]["y"]
                        );
                        boxCollider.friction = component["friction"];
                        boxCollider.density = component["density"];
                        boxCollider.restitution = component["restitution"];
                        boxCollider.restitutionThreshold = component["restitutionThreshold"];
                        boxCollider.trigger = component["trigger"];
                    }

                    if(type == "SpritesheetRenderer") {
                        gameObject->AddComponent<Experimental::SpritesheetRenderer>();
                        auto &spritesheetRenderer = gameObject->GetComponent<Experimental::SpritesheetRenderer>();
                        spritesheetRenderer.mesh->material.baseColor = Vector4(
                            component["color"]["r"],
                            component["color"]["g"],
                            component["color"]["b"],
                            1
                        );

                        spritesheetRenderer.spriteOffset = Vector2(
                            component["spriteOffset"]["x"],
                            component["spriteOffset"]["y"]
                        );

                        spritesheetRenderer.spriteSize = Vector2(
                            component["spriteSize"]["x"],
                            component["spriteSize"]["y"]
                        );

                        spritesheetRenderer.spritesheetSize = Vector2(
                            component["spritesheetSize"]["x"],
                            component["spritesheetSize"]["y"]
                        );

                        delete spritesheetRenderer.mesh;
                        delete spritesheetRenderer.sp;
                        if(component["spritesheet"] != "") {
                            spritesheetRenderer.material.diffuse = new Texture(((std::string)component["spritesheet"]).c_str(), 0, "texture_diffuse");
                        }
                        spritesheetRenderer.sp = new Spritesheet("", spritesheetRenderer.material, spritesheetRenderer.spritesheetSize, spritesheetRenderer.spriteSize, spritesheetRenderer.spriteOffset);
                        spritesheetRenderer.mesh = spritesheetRenderer.sp->m_Mesh;
                    }

                    if(type == "Rigidbody3D") {
                        gameObject->AddComponent<Experimental::Rigidbody3D>();
                        auto &rigidbody = gameObject->GetComponent<Experimental::Rigidbody3D>();

                        rigidbody.mass = component["mass"];
                        rigidbody.friction = component["friction"];
                        rigidbody.restitution = component["restitution"];
                        rigidbody.fixedRotation = component["fixedRotation"];
                        rigidbody.trigger = component["trigger"];
                    }

                    if(type == "BoxCollider3D") {
                        gameObject->AddComponent<Experimental::BoxCollider3D>();
                        auto &boxCollider = gameObject->GetComponent<Experimental::BoxCollider3D>();

                        boxCollider.size = Vector3(
                            component["size"]["x"],
                            component["size"]["y"],
                            component["size"]["z"]
                        );
                    }

                    if(type == "MeshCollider3D") {
                        gameObject->AddComponent<Experimental::MeshCollider3D>();
                        auto &meshCollider = gameObject->GetComponent<Experimental::MeshCollider3D>();

                        meshCollider.size = Vector3(
                            component["size"]["x"],
                            component["size"]["y"],
                            component["size"]["z"]
                        );
                    }
                }
                if(parentID != "NO_PARENT") {
                    Experimental::Transform childTransform;

                    delete gameObject;
                    //erase back
                    m_GameObjects.erase(m_GameObjects.begin() + m_GameObjects.size() - 1);
                    int amountOfChildren = 0;
                    std::vector<std::string> childNames;
                    for(int j = 0; j < m_GameObjects.size(); j++) {
                        if(m_GameObjects[j]->ID == parentID) {
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

                            delete m_GameObjects[j];
                            m_GameObjects.erase(m_GameObjects.begin() + j);

                            break;
                        }
                    }
                }

            }

            LoadingScene = false;
            HYPER_LOG("Loaded scene: " + scenePath);
        }
        Experimental::GameObject *LoadPrefab(const std::string &scenePath) {
            std::ifstream file(scenePath);
            // get content
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            nlohmann::json JSON;
            JSON = nlohmann::json::parse(content);

            Experimental::GameObject *parentObject = nullptr;
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
                if(gameObject->parentID == "NO_PARENT") {
                    parentObject = gameObject;
                }

                nlohmann::json components = JSON[i]["components"];

                std::string meshType = "";

                Experimental::Transform transform;
                m_GameObjects.push_back(gameObject);

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

                    if(type == "SpriteRenderer") {
                        gameObject->AddComponent<Experimental::SpriteRenderer>();
                        auto &spriteRenderer = gameObject->GetComponent<Experimental::SpriteRenderer>();
                        if(component["sprite"] != "") {
                            spriteRenderer.mesh->material.diffuse = new Texture(((std::string)component["sprite"]).c_str(), 0, "texture_diffuse");
                            spriteRenderer.mesh->material.baseColor = Vector4(
                                component["color"]["r"],
                                component["color"]["g"],
                                component["color"]["b"],
                                1
                            );
                        }
                    }

                    if(type == "Rigidbody2D") {
                        gameObject->AddComponent<Experimental::Rigidbody2D>();
                        auto &rigidbody = gameObject->GetComponent<Experimental::Rigidbody2D>();

                        rigidbody.type = component["bodyType"];
                        rigidbody.gravityScale = component["gravityScale"];
                    }

                    if(type == "BoxCollider2D") {
                        gameObject->AddComponent<Experimental::BoxCollider2D>();
                        auto &boxCollider = gameObject->GetComponent<Experimental::BoxCollider2D>();

                        boxCollider.size = Vector2(
                            component["size"]["x"],
                            component["size"]["y"]
                        );
                        boxCollider.friction = component["friction"];
                        boxCollider.density = component["density"];
                        boxCollider.restitution = component["restitution"];
                        boxCollider.restitutionThreshold = component["restitutionThreshold"];
                    }

                    if(type == "SpritesheetRenderer") {
                        gameObject->AddComponent<Experimental::SpritesheetRenderer>();
                        auto &spritesheetRenderer = gameObject->GetComponent<Experimental::SpritesheetRenderer>();
                        spritesheetRenderer.mesh->material.baseColor = Vector4(
                            component["color"]["r"],
                            component["color"]["g"],
                            component["color"]["b"],
                            1
                        );

                        spritesheetRenderer.spriteOffset = Vector2(
                            component["spriteOffset"]["x"],
                            component["spriteOffset"]["y"]
                        );

                        spritesheetRenderer.spriteSize = Vector2(
                            component["spriteSize"]["x"],
                            component["spriteSize"]["y"]
                        );

                        spritesheetRenderer.spritesheetSize = Vector2(
                            component["spritesheetSize"]["x"],
                            component["spritesheetSize"]["y"]
                        );

                        delete spritesheetRenderer.mesh;
                        delete spritesheetRenderer.sp;
                        if(component["spritesheet"] != "") {
                            spritesheetRenderer.material.diffuse = new Texture(((std::string)component["spritesheet"]).c_str(), 0, "texture_diffuse");
                        }
                        spritesheetRenderer.sp = new Spritesheet("", spritesheetRenderer.material, spritesheetRenderer.spritesheetSize, spritesheetRenderer.spriteSize, spritesheetRenderer.spriteOffset);
                        spritesheetRenderer.mesh = spritesheetRenderer.sp->m_Mesh;
                    }
                }

                if(parentID != "NO_PARENT") {
                    Experimental::Transform childTransform;

                    delete gameObject;
                    //erase back
                    m_GameObjects.erase(m_GameObjects.begin() + m_GameObjects.size() - 1);
                    int amountOfChildren = 0;
                    std::vector<std::string> childNames;
                    for(int j = 0; j < m_GameObjects.size(); j++) {
                        if(m_GameObjects[j]->ID == parentID) {
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

                            delete m_GameObjects[j];
                            m_GameObjects.erase(m_GameObjects.begin() + j);

                            break;
                        }
                    }
                }

            }
        
            HYPER_LOG("Loaded Prefab: " + scenePath);
            return parentObject;
        }
       
        void SaveScene(const std::string &path) { 
            std::ofstream file(path);
            nlohmann::json JSON;

            int offset = 0;

            for(int i = 0; i < m_GameObjects.size(); i++) {
                auto &gameObject = m_GameObjects[i];

                std::string name = gameObject->name; 
                std::string ID = gameObject->ID; 
                std::string parentID = gameObject->parentID; 
                std::string tag = gameObject->tag;
                std::string layer = gameObject->layer;

                JSON[i]["name"] = name;
                JSON[i]["ID"] = ID;
                JSON[i]["tag"] = tag;
                JSON[i]["parentID"] = parentID;
                JSON[i]["layer"] = layer;
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

                if(gameObject->HasComponent<Experimental::SpriteRenderer>()) {
                    auto &spriteRenderer = gameObject->GetComponent<Experimental::SpriteRenderer>();

                    JSON[i]["components"][componentOffset]["type"] = "SpriteRenderer";
                    JSON[i]["components"][componentOffset]["sprite"] = spriteRenderer.mesh->material.diffuse == nullptr ? "" : spriteRenderer.mesh->material.diffuse->texPath;
                    JSON[i]["components"][componentOffset]["color"]["r"] = spriteRenderer.mesh->material.baseColor.x;
                    JSON[i]["components"][componentOffset]["color"]["g"] = spriteRenderer.mesh->material.baseColor.y;
                    JSON[i]["components"][componentOffset]["color"]["b"] = spriteRenderer.mesh->material.baseColor.z;

                    componentOffset++;
                }

                if(gameObject->HasComponent<Experimental::Rigidbody2D>()) {
                    auto &rigidbody = gameObject->GetComponent<Experimental::Rigidbody2D>();

                    JSON[i]["components"][componentOffset]["type"] = "Rigidbody2D";
                    JSON[i]["components"][componentOffset]["gravityScale"] = rigidbody.gravityScale;
                    JSON[i]["components"][componentOffset]["bodyType"] = rigidbody.type;
                    JSON[i]["components"][componentOffset]["fixedRotation"] = rigidbody.fixedRotation;

                    componentOffset++;
                }

                if(gameObject->HasComponent<Experimental::BoxCollider2D>()) {
                    auto &collider = gameObject->GetComponent<Experimental::BoxCollider2D>();

                    JSON[i]["components"][componentOffset]["type"] = "BoxCollider2D";
                    JSON[i]["components"][componentOffset]["density"] = collider.density;
                    JSON[i]["components"][componentOffset]["friction"] = collider.friction;
                    JSON[i]["components"][componentOffset]["trigger"] = collider.trigger;
                    JSON[i]["components"][componentOffset]["restitution"] = collider.restitution;
                    JSON[i]["components"][componentOffset]["restitutionThreshold"] = collider.restitutionThreshold;
                    JSON[i]["components"][componentOffset]["size"] = {
                        {"x", collider.size.x},
                        {"y", collider.size.y}
                    };

                    componentOffset++;
                }

                if(gameObject->HasComponent<Experimental::SpritesheetRenderer>()) {
                    auto &spritesheetRenderer = gameObject->GetComponent<Experimental::SpritesheetRenderer>();

                    JSON[i]["components"][componentOffset]["type"] = "SpritesheetRenderer";
                    JSON[i]["components"][componentOffset]["color"]["r"] = spritesheetRenderer.material.baseColor.x;
                    JSON[i]["components"][componentOffset]["color"]["g"] = spritesheetRenderer.material.baseColor.y;
                    JSON[i]["components"][componentOffset]["color"]["b"] = spritesheetRenderer.material.baseColor.z;

                    JSON[i]["components"][componentOffset]["spritesheetSize"] = {
                        {"x", spritesheetRenderer.spritesheetSize.x},
                        {"y", spritesheetRenderer.spritesheetSize.y}
                    };

                    JSON[i]["components"][componentOffset]["spriteSize"] = {
                        {"x", spritesheetRenderer.spriteSize.x},
                        {"y", spritesheetRenderer.spriteSize.y}
                    };

                    JSON[i]["components"][componentOffset]["spriteOffset"] = {
                        {"x", spritesheetRenderer.spriteOffset.x},
                        {"y", spritesheetRenderer.spriteOffset.y}
                    };

                    JSON[i]["components"][componentOffset]["spritesheet"] = spritesheetRenderer.material.diffuse == nullptr ? "" : spritesheetRenderer.material.diffuse->texPath;

                    componentOffset++;
                }

                if(gameObject->HasComponent<Experimental::Rigidbody3D>()) {
                    auto &rigidbody = gameObject->GetComponent<Experimental::Rigidbody3D>();

                    JSON[i]["components"][componentOffset]["type"] = "Rigidbody3D";
                    JSON[i]["components"][componentOffset]["mass"] = rigidbody.mass;
                    JSON[i]["components"][componentOffset]["friction"] = rigidbody.friction;
                    JSON[i]["components"][componentOffset]["restitution"] = rigidbody.restitution;
                    JSON[i]["components"][componentOffset]["fixedRotation"] = rigidbody.fixedRotation;
                    JSON[i]["components"][componentOffset]["trigger"] = rigidbody.trigger;

                    componentOffset++;
                }

                if(gameObject->HasComponent<Experimental::BoxCollider3D>()) {
                    auto &collider = gameObject->GetComponent<Experimental::BoxCollider3D>();

                    JSON[i]["components"][componentOffset]["type"] = "BoxCollider3D";
                    JSON[i]["components"][componentOffset]["size"] = {
                        {"x", collider.size.x},
                        {"y", collider.size.y},
                        {"z", collider.size.z}
                    };


                    componentOffset++;
                }

                if(gameObject->HasComponent<Experimental::MeshCollider3D>()) {
                    auto &collider = gameObject->GetComponent<Experimental::MeshCollider3D>();
                    JSON[i]["components"][componentOffset]["type"] = "MeshCollider3D";
                    JSON[i]["components"][componentOffset]["size"] = {
                            {"x", collider.size.x},
                            {"y", collider.size.y},
                            {"z", collider.size.z}
                    };

                    componentOffset++;
                }
             }
            
            file << JSON;
            HYPER_LOG("Scene saved to " + path);
        }

        bool DropTargetMat(DragType type, Mesh *currEntity, Texture *otherData) {
            if (ImGui::BeginDragDropTarget())
            {
                if(type == DragType::DRAG_GAMEOBJECT) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("game_object"))
                    {
                        for(auto &obj : m_GameObjects) {
                            if(obj->ID == dirPayloadData && type == DRAG_GAMEOBJECT) {
                                SavePrefab((std::string("assets/") + obj->name + std::string(".prefab")).c_str(), obj);
                                break;
                            }
                        }
                    }
                }

                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("file"))
                {
                    // const char* path = (const char*)payload->Data;
                    if(type != DRAG_GAMEOBJECT) {
                        dirPayloadData.erase(0, cwd.length() + 1);
                    }

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
                            case DRAG_SPRITE: {
                                if(currEntity->material.diffuse != nullptr) {
                                    glDeleteTextures(1, &currEntity->material.diffuse->ID);
                                }

                                currEntity->material.diffuse = new Texture((char*)dirPayloadData.c_str(), 0, "texture_diffuse");

                                break;
                            }
                            case DRAG_SPRITE_NO_MESH: {
                                if(otherData != nullptr) {
                                    glDeleteTextures(1, &otherData->ID);
                                }

                                otherData = new Texture((char*)dirPayloadData.c_str(), 0, "texture_diffuse");
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

                    if(
                        G_END_WITH(dirPayloadData, ".prefab") && type == DRAG_MODEL
                    ) {
                        LoadPrefab(dirPayloadData);
                    }

                    for(auto &obj : m_GameObjects) {
                        if(obj->ID == dirPayloadData && type == DRAG_GAMEOBJECT) {
                            SavePrefab((std::string("assets/") + obj->name + std::string(".prefab")).c_str(), obj);
                            break;
                        }
                    }

                    return true;
                }

                ImGui::EndDragDropTarget();
            }

            return false;
        }

        std::string currentScenePath = "";
        entt::registry m_Registry;
        std::map<std::string, bool> layers;
        std::vector<Experimental::GameObject*> m_GameObjects = {};

        Experimental::GameObject *m_Object = nullptr;
        char name[499];
        char tag[499];
        char layer[32];

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
        std::map<std::string, std::map<std::string, int>> currFrames;
        std::map<std::string, std::map<std::string, float>> currDelays;

        bool LoadingScene = false;

        b2World *world = nullptr;
        std::vector<Experimental::GameObject*> m_UIObjects;
    }
}
