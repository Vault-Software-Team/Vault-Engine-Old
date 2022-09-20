
#include <iostream>
#include <random>
#include <memory>
#include "../api/api.hpp"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

#ifndef _WIN32
#include <unistd.h>
#elif
#include <windows.h>
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

std::string dirPayloadData = "";
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

enum DragType {
    DRAG_DIFFUSE,
    DRAG_SPECULAR,
    DRAG_NORMAL,
    DRAG_MODEL,
    DRAG_SCENE
};

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void ParseStr(std::string &str) {
    str.erase(std::remove(str.begin(), str.end(), '\"'), str.end());
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
        ParseStr(diffusePath);
        material.diffuse = new Texture((char*)diffusePath.c_str(), 0, "texture_diffuse");
    }

    if(JSON[i]["material"]["specular"] != "nullptr") {
        std::string specularPath = JSON[i]["material"]["specular"];
        ParseStr(specularPath);
        material.specular = new Texture((char*)specularPath.c_str(), 1, "texture_specular");
    }

    if(JSON[i]["material"]["normal"] != "nullptr") {
        std::string normalPath = JSON[i]["material"]["normal"];
        ParseStr(normalPath);
        material.normal = new Texture((char*)normalPath.c_str(), 2, "texture_normal");
    }
}

void ApplyTransform(TransformComponent &transform, int i, nlohmann::json JSON) {
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

        for(int i = 0; i < Scene::cameras.size(); i++) {
            Scene::cameras[i]->scriptComponent.scripts.clear();
            delete Scene::cameras[i];
        }
        SpotLights.clear();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    for(int i = 0; i < JSON.size(); i++) {
        std::string type = JSON[i]["type"];
        ParseStr(type);

        std::string name = JSON[i]["name"];
        ParseStr(name);

        TransformComponent transform;

        if(type == "Plane") {
            Plane ent(Vector4(1, 1, 1, 1));

            for(int j = 0; j < JSON[i]["scripts"].size(); j++) {
                ent.entity->scriptComponent.AddScript(JSON[i]["scripts"][j]);
            }

            ent.entity->name = name;
            ent.entity->parentType = type;
            ApplyMaterial(JSON, ent.entity->material, i);
            // Transform trans = ent.GetComponent<Transform>();
            // trans.scale.x = 10;
            ApplyTransform(transform, i, JSON);
            ent.entity->UpdateComponent(transform);
            Scene::entities.push_back(ent.entity);
        }

        if(type == "Cube") {
            Cube ent(Vector4(1, 1, 1, 1));

            for(int j = 0; j < JSON[i]["scripts"].size(); j++) {
                ent.meshes[0]->scriptComponent.AddScript(JSON[i]["scripts"][j]);
            }

            ent.meshes[0]->name = name;
            ent.meshes[0]->parentType = type;
            ApplyMaterial(JSON, ent.meshes[0]->material, i);

            ApplyTransform(transform, i, JSON);
            ent.meshes[0]->UpdateComponent(transform);
            Scene::entities.push_back(ent.meshes[0]);
        }

        if(type == "Sphere") {
            Sphere ent(Vector4(1, 1, 1, 1));

            for(int j = 0; j < JSON[i]["scripts"].size(); j++) {
                ent.meshes[0]->scriptComponent.AddScript(JSON[i]["scripts"][j]);
            }

            ent.meshes[0]->name = name;
            ent.meshes[0]->parentType = type;
            ApplyMaterial(JSON, ent.meshes[0]->material, i);

            ApplyTransform(transform, i, JSON);
            ent.meshes[0]->UpdateComponent(transform);
            Scene::entities.push_back(ent.meshes[0]);
        }

        if(type == "Cylinder") {
            Cylinder ent(Vector4(1, 1, 1, 1));

            for(int j = 0; j < JSON[i]["scripts"].size(); j++) {
                ent.meshes[0]->scriptComponent.AddScript(JSON[i]["scripts"][j]);
            }

            ent.meshes[0]->name = name;
            ent.meshes[0]->parentType = type;
            ApplyMaterial(JSON, ent.meshes[0]->material, i);

            ApplyTransform(transform, i, JSON);
            ent.meshes[0]->UpdateComponent(transform);
            Scene::entities.push_back(ent.meshes[0]);
        }

        if(type == "Capsule") {
            Capsule ent(Vector4(1, 1, 1, 1));

            for(int j = 0; j < JSON[i]["scripts"].size(); j++) {
                ent.meshes[0]->scriptComponent.AddScript(JSON[i]["scripts"][j]);
            }

            ent.meshes[0]->name = name;
            ent.meshes[0]->parentType = type;
            ApplyMaterial(JSON, ent.meshes[0]->material, i);

            ApplyTransform(transform, i, JSON);
            ent.meshes[0]->UpdateComponent(transform);
            Scene::entities.push_back(ent.meshes[0]);
        }

        if(type == "Cone") {
            Cone ent(Vector4(1, 1, 1, 1));

            for(int j = 0; j < JSON[i]["scripts"].size(); j++) {
                ent.meshes[0]->scriptComponent.AddScript(JSON[i]["scripts"][j]);
            }

            ent.meshes[0]->name = name;
            ent.meshes[0]->parentType = type;
            ApplyMaterial(JSON, ent.meshes[0]->material, i);

            ApplyTransform(transform, i, JSON);
            ent.meshes[0]->UpdateComponent(transform);
            Scene::entities.push_back(ent.meshes[0]);
        }
    
        if(type == "Model") {
            std::string modelPath = JSON[i]["modelPath"];
            Model ent((char*)modelPath.c_str(), false);

            for(int j = 0; j < JSON[i]["meshes"].size(); j++) {
                ent.meshes[j]->name = JSON[i]["meshes"][j]["name"];
                ApplyMaterial(JSON[i]["meshes"], ent.meshes[j]->material, j);

                for(int g = 0; g < 0; g++) {
                    ent.meshes[j]->scriptComponent.scripts.push_back(ScriptEngine::LuaScript(JSON[i]["scripts"][g]));
                }

                ApplyTransform(transform, j, JSON[i]["meshes"]);
                ent.meshes[j]->UpdateComponent(transform);
            }

            ent.name = name;
            for(int j = 0; j < JSON[i]["scripts"].size(); j++) {
                ent.scriptComponent.AddScript(JSON[i]["scripts"][j]);
            }
            ApplyTransform(transform, i, JSON);
            ent.UpdateComponent(transform);

            Scene::models.push_back(ent);
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

            DirectionalLight *ent = new DirectionalLight(DirLights, pos, color);

            // for(int j = 0; j < JSON[i]["scripts"].size(); j++) {
            //     ent->scriptComponent.AddScript(JSON[i]["scripts"][j]);
            // }

            ent->name = name;
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

            PointLight *ent = new PointLight(PointLights, pos, color, JSON[i]["intensity"]);
            for(int j = 0; j < JSON[i]["scripts"].size(); j++) {
                ent->scriptComponent.AddScript(JSON[i]["scripts"][j]);
            }
            ent->name = name;
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

            SpotLight *ent = new SpotLight(SpotLights, pos, color);
            ent->angle.x = JSON[i]["angle"]["x"];
            ent->angle.y = JSON[i]["angle"]["y"];
            ent->angle.z = JSON[i]["angle"]["z"];

            for(int j = 0; j < JSON[i]["scripts"].size(); j++) {
                ent->scriptComponent.AddScript(JSON[i]["scripts"][j]);
            }

            ent->name = name;
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

            if(ent->mainCamera) {
                Scene::mainCamera = ent;
            }
        }
    }

    LoadState = false;
}

void SaveScene(const std::string &path) { 
    std::ofstream file(path);
    nlohmann::json JSON;

    int offset = 0;

    for(int i = 0; i < Scene::entities.size(); i++) {
        std::string type = Scene::entities[i]->parentType;
        std::string name = Scene::entities[i]->name;

        if(type == "Plane" || type == "Cube" || type == "Sphere" || type == "Cylinder" || type == "Capsule" || type == "Cone") {
            JSON[i]["scripts"] = nlohmann::json::array();
            for(int j = 0; j < Scene::entities[i]->scriptComponent.scripts.size(); j++) {
                JSON[i]["scripts"][j] = Scene::entities[i]->scriptComponent.scripts[j].pathToScript;
            }

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
                JSON[offset + i][g]["scripts"][g] = model.scriptComponent.scripts[g].pathToScript;
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

std::string cwd = "";
bool DropTargetMat(DragType type, Entity *currEntity) {
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("file"))
        {
            // const char* path = (const char*)payload->Data;
            dirPayloadData.erase(0, cwd.length() + 1);

            if(
                ends_with(dirPayloadData, ".png") ||
                ends_with(dirPayloadData, ".jpg") ||
                ends_with(dirPayloadData, ".jpeg")
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
                ends_with(dirPayloadData, ".obj") ||
                ends_with(dirPayloadData, ".fbx") ||
                ends_with(dirPayloadData, ".gltf")
            ) {
                switch (type)
                {
                    case DRAG_MODEL: {
                        Scene::models.push_back(Model((char*)dirPayloadData.c_str(), false));
                        break;
                    }
                }
            }

            if(
                ends_with(dirPayloadData, ".static") && type == DRAG_SCENE
            ) {
                LoadScene(dirPayloadData);
            }

            return true;
        }

        ImGui::EndDragDropTarget();
    }

    return false;
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
    app.renderOnScreen = true;
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

    Entity *currEntity = nullptr;
    PointLight *currPointLight = nullptr;
    SpotLight *currSpotLight = nullptr;
    Camera *selectedCamera = nullptr;
    DirectionalLight *currDirectionalLight = nullptr;
    int currModel = -1;
    char name[499];

    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, "", ImVec4(1,1,1, 1.0f), ICON_FA_FILE);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, "", ImVec4(1,1,1, 1.0f), ICON_FA_FOLDER);

    LoadScene(config.mainScene);


    std::function<void(unsigned int &PPT, unsigned int &PPFBO)> GUI = [&](unsigned int &PPT, unsigned int &PPFBO) {};
    
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

        if(focusedOnScene && !camera->mode2D) {
            camera->Inputs(app.renderer->window, winPos);
        }

        if(hoveredScene && camera->mode2D) {
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
        for(auto &entity : Scene::entities) {
            if(entity->empty) continue;
            entity->Draw(shader, *Scene::mainCamera);
        }

        for(auto &model : Scene::models) {
            model.Draw(shader, *Scene::mainCamera);
        }
    }, GUI);

    return 0;
}