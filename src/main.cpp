
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

void ApplyTransform(Experimental::Transform &transform, int i, nlohmann::json JSON) {
    transform.position.x = JSON[i]["components"]["TransformComponent"]["position"]["x"];
    transform.position.y = JSON[i]["components"]["TransformComponent"]["position"]["y"];
    transform.position.z = JSON[i]["components"]["TransformComponent"]["position"]["z"];

    transform.rotation.x = JSON[i]["components"]["TransformComponent"]["rotation"]["x"];
    transform.rotation.y = JSON[i]["components"]["TransformComponent"]["rotation"]["y"];
    transform.rotation.z = JSON[i]["components"]["TransformComponent"]["rotation"]["z"];

    transform.scale.x = JSON[i]["components"]["TransformComponent"]["scale"]["x"];
    transform.scale.y = JSON[i]["components"]["TransformComponent"]["scale"]["y"];
    transform.scale.z = JSON[i]["components"]["TransformComponent"]["scale"]["z"];
}

void LoadScene(const std::string &scenePath) {
    // json file
    LoadState = true;

    std::ifstream file(scenePath);
    // get content
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    nlohmann::json JSON;
    JSON = nlohmann::json::parse(content);

    try {
        // delete the pointer stuff
        for (int i = 0; i < Scene::entities.size(); i++) {  
            Scene::entities[i]->scriptComponent.scripts.clear();
            delete Scene::entities[i];
        }

        Scene::entities.clear();

        // delete models
        for (int i = 0; i < Scene::models.size(); i++) {
            Scene::models[i].scriptComponent.scripts.clear();
            for(int j = 0; j < Scene::models[i].meshes.size(); j++) {
                delete Scene::models[i].meshes[j];
            }
            Scene::models[i].meshes.clear();
        }

        Scene::models.clear();
        
        // delete DirLights
        for (int i = 0; i < DirLights.size(); i++) {
            DirLights[i]->scriptComponent.scripts.clear();
            delete DirLights[i];
        }
        DirLights.clear();

        // delete PointLights
        for (int i = 0; i < PointLights.size(); i++) {
            PointLights[i]->scriptComponent.scripts.clear();
            delete PointLights[i];
        }
        PointLights.clear();

        // delete SpotLights
        for (int i = 0; i < SpotLights.size(); i++) {
            SpotLights[i]->scriptComponent.scripts.clear();
            delete SpotLights[i];
        }
        SpotLights.clear();

        for(int i = 0; i < Scene::cameras.size(); i++) {
            Scene::cameras[i]->scriptComponent.scripts.clear();
            delete Scene::cameras[i];
        }
        Scene::cameras.clear();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    for(int i = 0; i < JSON.size(); i++) {
        std::string type = JSON[i]["type"];
        std::string name = JSON[i]["name"];

        if(type == "Plane") {
            Experimental::GameObject *obj = new Experimental::GameObject();
            Scene::m_GameObjects.push_back(obj);
            obj->AddComponent<Experimental::Transform>();
            obj->AddComponent<Experimental::MeshRenderer>();

            auto &renderer = obj->GetComponent<Experimental::MeshRenderer>();
            auto &transform = obj->GetComponent<Experimental::Transform>();
            renderer.m_Mesh = Plane(Vector4(1,1,1,1)).m_Mesh;

            renderer.m_Mesh->name = name;
            renderer.m_Mesh->parentType = type;
            ApplyMaterial(JSON, renderer.m_Mesh->material, i);
            // Transform trans = ent.GetComponent<Transform>();
            // trans.scale.x = 10;
            ApplyTransform(transform, i, JSON);
        }

        if(type == "Cube") {
            Experimental::GameObject *obj = new Experimental::GameObject();
            Scene::m_GameObjects.push_back(obj);
            obj->AddComponent<Experimental::Transform>();
            obj->AddComponent<Experimental::MeshRenderer>();

            auto &renderer = obj->GetComponent<Experimental::MeshRenderer>();
            auto &transform = obj->GetComponent<Experimental::Transform>();
            renderer.m_Mesh = Cube(Vector4(1,1,1,1)).meshes[0];

            renderer.m_Mesh->name = name;
            renderer.m_Mesh->parentType = type;
            ApplyMaterial(JSON, renderer.m_Mesh->material, i);
            // Transform trans = ent.GetComponent<Transform>();
            // trans.scale.x = 10;
            ApplyTransform(transform, i, JSON);
        }

        if(type == "Sphere") {
            Experimental::GameObject *obj = new Experimental::GameObject();
            Scene::m_GameObjects.push_back(obj);
            obj->AddComponent<Experimental::Transform>();
            obj->AddComponent<Experimental::MeshRenderer>();

            auto &renderer = obj->GetComponent<Experimental::MeshRenderer>();
            auto &transform = obj->GetComponent<Experimental::Transform>();
            renderer.m_Mesh = Sphere(Vector4(1,1,1,1)).meshes[0];

            renderer.m_Mesh->name = name;
            renderer.m_Mesh->parentType = type;
            ApplyMaterial(JSON, renderer.m_Mesh->material, i);
            // Transform trans = ent.GetComponent<Transform>();
            // trans.scale.x = 10;
            ApplyTransform(transform, i, JSON);
        }

        if(type == "Cylinder") {
            Experimental::GameObject *obj = new Experimental::GameObject();
            Scene::m_GameObjects.push_back(obj);
            obj->AddComponent<Experimental::Transform>();
            obj->AddComponent<Experimental::MeshRenderer>();

            auto &renderer = obj->GetComponent<Experimental::MeshRenderer>();
            auto &transform = obj->GetComponent<Experimental::Transform>();
            renderer.m_Mesh = Cylinder(Vector4(1,1,1,1)).meshes[0];

            renderer.m_Mesh->name = name;
            renderer.m_Mesh->parentType = type;
            ApplyMaterial(JSON, renderer.m_Mesh->material, i);
            // Transform trans = ent.GetComponent<Transform>();
            // trans.scale.x = 10;
            ApplyTransform(transform, i, JSON);
        }

        if(type == "Capsule") {
            Experimental::GameObject *obj = new Experimental::GameObject();
            Scene::m_GameObjects.push_back(obj);
            obj->AddComponent<Experimental::Transform>();
            obj->AddComponent<Experimental::MeshRenderer>();

            auto &renderer = obj->GetComponent<Experimental::MeshRenderer>();
            auto &transform = obj->GetComponent<Experimental::Transform>();
            renderer.m_Mesh = Capsule(Vector4(1,1,1,1)).meshes[0];

            renderer.m_Mesh->name = name;
            renderer.m_Mesh->parentType = type;
            ApplyMaterial(JSON, renderer.m_Mesh->material, i);
            // Transform trans = ent.GetComponent<Transform>();
            // trans.scale.x = 10;
            ApplyTransform(transform, i, JSON);
        }

        if(type == "Cone") {
            Experimental::GameObject *obj = new Experimental::GameObject();
            Scene::m_GameObjects.push_back(obj);
            obj->AddComponent<Experimental::Transform>();
            obj->AddComponent<Experimental::MeshRenderer>();

            auto &renderer = obj->GetComponent<Experimental::MeshRenderer>();
            auto &transform = obj->GetComponent<Experimental::Transform>();
            renderer.m_Mesh = Cone(Vector4(1,1,1,1)).meshes[0];

            renderer.m_Mesh->name = name;
            renderer.m_Mesh->parentType = type;
            ApplyMaterial(JSON, renderer.m_Mesh->material, i);
            // Transform trans = ent.GetComponent<Transform>();
            // trans.scale.x = 10;
            ApplyTransform(transform, i, JSON);
        }
    
        if(type == "Model") {
            std::string modelPath = JSON[i]["modelPath"];
            Experimental::Model ent((char*)modelPath.c_str(), false);
            auto &transform = ent.mainGameObject->GetComponent<Experimental::Transform>();

            for(int j = 0; j < JSON[i]["meshes"].size(); j++) {
                auto &transform = ent.m_gameObjects[j]->GetComponent<Experimental::Transform>();
                auto &renderer = ent.m_gameObjects[j]->GetComponent<Experimental::MeshRenderer>();

                ent.m_gameObjects[j]->name = JSON[i]["meshes"][j]["name"];
                ApplyMaterial(JSON[i]["meshes"], renderer.m_Mesh->material, j);

                ApplyTransform(transform, j, JSON[i]["meshes"]);
            }

            ent.mainGameObject->name = name;
            ApplyTransform(transform, i, JSON);
        }

        if(type == "DirectionalLight") {
            Vector3 pos = Vector3(
                JSON[i]["lightPos"]["x"],
                JSON[i]["lightPos"]["y"],
                JSON[i]["lightPos"]["z"]
            );

            Vector3 color = Vector3(
                JSON[i]["color"]["r"],
                JSON[i]["color"]["g"],
                JSON[i]["color"]["b"]
            );

            Experimental::GameObject *obj = new Experimental::GameObject();
            obj->AddComponent<Experimental::Transform>();
            obj->AddComponent<Experimental::c_DirectionalLight>();

            auto &transform = obj->GetComponent<Experimental::Transform>();
            auto &light = obj->GetComponent<Experimental::c_DirectionalLight>();
            transform.position = pos;
            light.color = color;

            // for(int j = 0; j < JSON[i]["scripts"].size(); j++) {
            //     ent->scriptComponent.AddScript(JSON[i]["scripts"][j]);
            // }

            obj->name = name;
        }

        if(type == "PointLight") {
            Vector3 pos = Vector3(
                JSON[i]["lightPos"]["x"],
                JSON[i]["lightPos"]["y"],
                JSON[i]["lightPos"]["z"]
            );

            Vector3 color = Vector3(
                JSON[i]["color"]["r"],
                JSON[i]["color"]["g"],
                JSON[i]["color"]["b"]
            );

            Experimental::GameObject *obj = new Experimental::GameObject();
            obj->AddComponent<Experimental::Transform>();
            obj->AddComponent<Experimental::c_PointLight>();

            auto &transform = obj->GetComponent<Experimental::Transform>();
            auto &light = obj->GetComponent<Experimental::c_PointLight>();
            transform.position = pos;
            light.color = color;

            // for(int j = 0; j < JSON[i]["scripts"].size(); j++) {
            //     ent->scriptComponent.AddScript(JSON[i]["scripts"][j]);
            // }

            obj->name = name;
        }

        if(type == "SpotLight") {
            Vector3 pos = Vector3(
                JSON[i]["lightPos"]["x"],
                JSON[i]["lightPos"]["y"],
                JSON[i]["lightPos"]["z"]
            );

            Vector3 color = Vector3(
                JSON[i]["color"]["r"],
                JSON[i]["color"]["g"],
                JSON[i]["color"]["b"]
            );

            Experimental::GameObject *obj = new Experimental::GameObject();
            obj->AddComponent<Experimental::Transform>();
            obj->AddComponent<Experimental::c_SpotLight>();

            auto &transform = obj->GetComponent<Experimental::Transform>();
            auto &light = obj->GetComponent<Experimental::c_SpotLight>();
            transform.position = pos;
            transform.rotation = Vector3(
                JSON[i]["angle"]["x"],
                JSON[i]["angle"]["y"],
                JSON[i]["angle"]["z"]
            );
            light.color = color;

            // for(int j = 0; j < JSON[i]["scripts"].size(); j++) {
            //     ent->scriptComponent.AddScript(JSON[i]["scripts"][j]);
            // }

            obj->name = name;
        }

        if(type == "Camera") {
            float fov = JSON[i]["fov"];
            float near = JSON[i]["near"];
            float far = JSON[i]["far"];
            Vector3 position = Vector3(
                JSON[i]["position"]["x"],
                JSON[i]["position"]["y"],
                JSON[i]["position"]["z"]
            );

            Vector3 rotation = Vector3(
                JSON[i]["rotation"]["x"],
                JSON[i]["rotation"]["y"],
                JSON[i]["rotation"]["z"]
            );

            bool isMain = JSON[i]["isMain"];
            bool mode2D = JSON[i]["mode2D"];

            Camera *ent = new Camera(mode2D, 1280, 720, position);
            ent->fov = fov;
            ent->near = near;
            ent->far = far;
            TransformComponent transform = ent->GetComponent<TransformComponent>();
            transform.rotation = rotation;
            ent->UpdateComponent(transform);
            ent->name = name;
            ent->mainCamera = isMain;

            for(int j = 0; j < JSON[i]["scripts"].size(); j++) {
                ent->scriptComponent.AddScript(JSON[i]["scripts"][j]);
            }

            Scene::cameras.push_back(ent);
        }
    }

    LoadState = false;
}

void SaveScene(const std::string &path) { 
    std::ofstream file(path);
    nlohmann::json JSON;

    int offset = 0;

    for(int i = 0; i < Scene::m_GameObjects.size(); i++) {
        std::string type = Scene::entities[i]->parentType;
        std::string name = Scene::entities[i]->name;

        if(type == "Plane" || type == "Cube" || type == "Sphere" || type == "Cylinder" || type == "Capsule" || type == "Cone") {
            JSON[i]["type"] = type;
            JSON[i]["name"] = name;
            JSON[i]["material"]["baseColor"]["r"] = Scene::entities[i]->material.baseColor.x;
            JSON[i]["material"]["baseColor"]["g"] = Scene::entities[i]->material.baseColor.y;
            JSON[i]["material"]["baseColor"]["b"] = Scene::entities[i]->material.baseColor.z;

            if(Scene::entities[i]->material.diffuse != nullptr) {
                JSON[i]["material"]["diffuse"] = Scene::entities[i]->material.diffuse->texPath;
            } else {
                JSON[i]["material"]["diffuse"] = std::string("nullptr");
            }

            if(Scene::entities[i]->material.specular != nullptr) {
                JSON[i]["material"]["specular"] = Scene::entities[i]->material.specular->texPath;
            } else {
                JSON[i]["material"]["specular"] = std::string("nullptr");
            }

            if(Scene::entities[i]->material.normal != nullptr) {
                JSON[i]["material"]["normal"] = Scene::entities[i]->material.normal->texPath;
            } else {
                JSON[i]["material"]["normal"] = std::string("nullptr");
            }

            JSON[i]["material"]["metallic"] = Scene::entities[i]->material.metallic;
            JSON[i]["material"]["roughness"] = Scene::entities[i]->material.roughness;
            JSON[i]["material"]["texUV"]["x"] = Scene::entities[i]->material.texUVs.x;
            JSON[i]["material"]["texUV"]["y"] = Scene::entities[i]->material.texUVs.y;

            TransformComponent transform = Scene::entities[i]->GetComponent<TransformComponent>();
            JSON[i]["components"]["TransformComponent"]["position"]["x"] = transform.position.x;
            JSON[i]["components"]["TransformComponent"]["position"]["y"] = transform.position.y;
            JSON[i]["components"]["TransformComponent"]["position"]["z"] = transform.position.z;
            JSON[i]["components"]["TransformComponent"]["rotation"]["x"] = transform.rotation.x;
            JSON[i]["components"]["TransformComponent"]["rotation"]["y"] = transform.rotation.y;
            JSON[i]["components"]["TransformComponent"]["rotation"]["z"] = transform.rotation.z;
            JSON[i]["components"]["TransformComponent"]["scale"]["x"] = transform.scale.x;
            JSON[i]["components"]["TransformComponent"]["scale"]["y"] = transform.scale.y;
            JSON[i]["components"]["TransformComponent"]["scale"]["z"] = transform.scale.z;
        }   
    }
    offset += Scene::entities.size();

    for(int i = 0; i < DirLights.size(); i++) {
        DirectionalLight *light = DirLights[i];

        int offset = JSON.size();

        JSON[offset + i]["scripts"] = nlohmann::json::array();
        for(int j = 0; j < light->scriptComponent.scripts.size(); j++) {
            JSON[offset + i]["scripts"][j] = light->scriptComponent.scripts[j].pathToScript;
        }
        JSON[offset + i]["type"] = "DirectionalLight";
        JSON[offset + i]["name"] = light->name;
        JSON[offset + i]["lightPos"]["x"] = light->lightPos.x;
        JSON[offset + i]["lightPos"]["y"] = light->lightPos.y;
        JSON[offset + i]["lightPos"]["z"] = light->lightPos.z;
        JSON[offset + i]["color"]["r"] = light->color.x;
        JSON[offset + i]["color"]["g"] = light->color.y;
        JSON[offset + i]["color"]["b"] = light->color.z; 
    }
    offset += DirLights.size();

    for(int i = 0; i < PointLights.size(); i++) {
        PointLight *light = PointLights[i];

        int offset = JSON.size();
        offset -= i;

        JSON[offset + i]["scripts"] = nlohmann::json::array();
        for(int j = 0; j < light->scriptComponent.scripts.size(); j++) {
            JSON[offset + i]["scripts"][j] = light->scriptComponent.scripts[j].pathToScript;
        }

        JSON[offset + i]["type"] = "PointLight";
        JSON[offset + i]["name"] = light->name;
        JSON[offset + i]["lightPos"]["x"] = light->lightPos.x;
        JSON[offset + i]["lightPos"]["y"] = light->lightPos.y;
        JSON[offset + i]["lightPos"]["z"] = light->lightPos.z;
        JSON[offset + i]["color"]["r"] = light->color.x;
        JSON[offset + i]["color"]["g"] = light->color.y;
        JSON[offset + i]["color"]["b"] = light->color.z;
        JSON[offset + i]["intensity"] = light->intensity; 
    }
    offset += PointLights.size();

    for(int i = 0; i < SpotLights.size(); i++) {
        SpotLight *light = SpotLights[i];

        int offset = JSON.size();
        offset -= i;

        JSON[offset + i]["scripts"] = nlohmann::json::array();
        for(int j = 0; j < light->scriptComponent.scripts.size(); j++) {
            JSON[offset + i]["scripts"][j] = light->scriptComponent.scripts[j].pathToScript;
        }

        JSON[offset + i]["type"] = "SpotLight";
        JSON[offset + i]["name"] = light->name;
        JSON[offset + i]["lightPos"]["x"] = light->lightPos.x;
        JSON[offset + i]["lightPos"]["y"] = light->lightPos.y;
        JSON[offset + i]["lightPos"]["z"] = light->lightPos.z;
        JSON[offset + i]["color"]["r"] = light->color.x;
        JSON[offset + i]["color"]["g"] = light->color.y;
        JSON[offset + i]["color"]["b"] = light->color.z;
        JSON[offset + i]["angle"]["x"] = light->angle.x;
        JSON[offset + i]["angle"]["y"] = light->angle.y;
        JSON[offset + i]["angle"]["z"] = light->angle.z;
    }

    for(int i = 0; i < Scene::models.size(); i++) {
        Model model = Scene::models[i];

        int offset = JSON.size();
        offset -= i;

        JSON[offset + i]["scripts"] = nlohmann::json::array();
        for(int j = 0; j < model.scriptComponent.scripts.size(); j++) {
            JSON[offset + i]["scripts"][j] = model.scriptComponent.scripts[j].pathToScript;
        }

        JSON[offset + i]["type"] = "Model";
        JSON[offset + i]["name"] = model.name;
        JSON[offset + i]["modelPath"] = model.path;

        TransformComponent transform = model.GetComponent<TransformComponent>();
        JSON[offset + i]["components"]["TransformComponent"]["position"]["x"] = transform.position.x;
        JSON[offset + i]["components"]["TransformComponent"]["position"]["y"] = transform.position.y;
        JSON[offset + i]["components"]["TransformComponent"]["position"]["z"] = transform.position.z;
        JSON[offset + i]["components"]["TransformComponent"]["rotation"]["x"] = transform.rotation.x;
        JSON[offset + i]["components"]["TransformComponent"]["rotation"]["y"] = transform.rotation.y;
        JSON[offset + i]["components"]["TransformComponent"]["rotation"]["z"] = transform.rotation.z;
        JSON[offset + i]["components"]["TransformComponent"]["scale"]["x"] = transform.scale.x;
        JSON[offset + i]["components"]["TransformComponent"]["scale"]["y"] = transform.scale.y;
        JSON[offset + i]["components"]["TransformComponent"]["scale"]["z"] = transform.scale.z;

        for(int j = 0; j < model.meshes.size(); j++) {
            JSON[offset + i][j]["scripts"] = nlohmann::json::array();
            
            for(int g = 0; g < model.meshes[j]->scriptComponent.scripts.size(); g++) {
                JSON[offset + i][j]["scripts"][g] = model.scriptComponent.scripts[g].pathToScript;
            }

            JSON[offset + i][j]["meshes"][j]["name"] = model.meshes[j]->name;

            if(model.meshes[j]->material.diffuse != nullptr)
                JSON[offset + i]["meshes"][j]["material"]["diffuse"] = model.meshes[j]->material.diffuse->texPath;
            else 
                JSON[offset + i]["meshes"][j]["material"]["diffuse"] = std::string("nullptr");

            if(model.meshes[j]->material.specular != nullptr)
                JSON[offset + i]["meshes"][j]["material"]["specular"] = model.meshes[j]->material.specular->texPath;
            else 
                JSON[offset + i]["meshes"][j]["material"]["specular"] = std::string("nullptr");

            if(model.meshes[j]->material.normal != nullptr)
                JSON[offset + i]["meshes"][j]["material"]["normal"] = model.meshes[j]->material.normal->texPath;
            else 
                JSON[offset + i]["meshes"][j]["material"]["normal"] = std::string("nullptr");

            JSON[offset + i]["meshes"][j]["material"]["metallic"] = model.meshes[j]->material.metallic;
            JSON[offset + i]["meshes"][j]["material"]["roughness"] = model.meshes[j]->material.roughness;
            JSON[offset + i]["meshes"][j]["material"]["texUV"]["x"] = model.meshes[j]->material.texUVs.x;
            JSON[offset + i]["meshes"][j]["material"]["texUV"]["y"] = model.meshes[j]->material.texUVs.y;

            // base color
            JSON[offset + i]["meshes"][j]["material"]["baseColor"]["r"] = model.meshes[j]->material.baseColor.x;
            JSON[offset + i]["meshes"][j]["material"]["baseColor"]["g"] = model.meshes[j]->material.baseColor.y;
            JSON[offset + i]["meshes"][j]["material"]["baseColor"]["b"] = model.meshes[j]->material.baseColor.z;
            JSON[offset + i]["meshes"][j]["material"]["baseColor"]["a"] = 1;

            TransformComponent transform = model.meshes[j]->GetComponent<TransformComponent>();
            JSON[offset + i]["meshes"][j]["components"]["TransformComponent"]["position"]["x"] = transform.position.x;
            JSON[offset + i]["meshes"][j]["components"]["TransformComponent"]["position"]["y"] = transform.position.y;
            JSON[offset + i]["meshes"][j]["components"]["TransformComponent"]["position"]["z"] = transform.position.z;
            JSON[offset + i]["meshes"][j]["components"]["TransformComponent"]["rotation"]["x"] = transform.rotation.x;
            JSON[offset + i]["meshes"][j]["components"]["TransformComponent"]["rotation"]["y"] = transform.rotation.y;
            JSON[offset + i]["meshes"][j]["components"]["TransformComponent"]["rotation"]["z"] = transform.rotation.z;
            JSON[offset + i]["meshes"][j]["components"]["TransformComponent"]["scale"]["x"] = transform.scale.x;
            JSON[offset + i]["meshes"][j]["components"]["TransformComponent"]["scale"]["y"] = transform.scale.y;
            JSON[offset + i]["meshes"][j]["components"]["TransformComponent"]["scale"]["z"] = transform.scale.z;
        }
    }

    for(int i = 0; i < Scene::cameras.size(); i++) {
        Camera *cam = Scene::cameras[i];
        TransformComponent transform = cam->GetComponent<TransformComponent>();

        int offset = JSON.size();

        JSON[offset + i]["scripts"] = nlohmann::json::array();
        for(int j = 0; j < cam->scriptComponent.scripts.size(); j++) {
            JSON[offset + i]["scripts"][j] = cam->scriptComponent.scripts[j].pathToScript;
        }

        JSON[offset + i]["name"] = cam->name;
        JSON[offset + i]["type"] = "Camera";
        JSON[offset + i]["fov"] = cam->fov;
        JSON[offset + i]["near"] = cam->near;
        JSON[offset + i]["far"] = cam->far;
        
        JSON[offset + i]["position"]["x"] = transform.position.x;
        JSON[offset + i]["position"]["y"] = transform.position.y;
        JSON[offset + i]["position"]["z"] = transform.position.z;

        JSON[offset + i]["rotation"]["x"] = transform.rotation.x;
        JSON[offset + i]["rotation"]["y"] = transform.rotation.y;
        JSON[offset + i]["rotation"]["z"] = transform.rotation.z;

        JSON[offset + i]["isMain"] = cam->mainCamera;
        JSON[offset + i]["mode2D"] = cam->mode2D;
    }
    
    file << JSON;
}

int main() {
    ScriptEngine::Init();

    char CWD[1024];
    getcwd(CWD, sizeof(CWD));
    cwd = std::string(CWD);

    setenv("LD_LIBRARY_PATH", (cwd + "/lib").c_str(), 1);

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
    Camera *camera = new Camera(false, app.width, app.height, Vector3(0,5,15));
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

    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, "", ImVec4(1,1,1, 1.0f), ICON_FA_FILE);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, "", ImVec4(1,1,1, 1.0f), ICON_FA_FOLDER);

    // LoadScene(config.mainScene);
    std::function<void(unsigned int &PPT, unsigned int &PPFBO)> GUI_EXP = 
    [&](unsigned int &PPT, unsigned int &PPFBO) {
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        if(ImGui::Begin("Scene")) {
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
                    Scene::m_Object->AddComponent<Experimental::CameraComponent>();
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