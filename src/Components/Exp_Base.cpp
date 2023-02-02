#include "Exp_Base.hpp"

#include "LuaScriptComponent.hpp"
#include "Model.hpp"
#include "NativeScriptManager.hpp"
#include "CppScriptManager.hpp"
#include "CsharpScriptManager.hpp"
#include "Rigidbody2D.hpp"
#include "Rigidbody3D.hpp"
#include "BoxCollider2D.hpp"
#include "Colliders3D.hpp"
#include "PathfindingAI.hpp"
#include "SpriteAnimation.hpp"
#include "SpritesheetAnimation.hpp"

namespace HyperAPI::Experimental {
    nlohmann::json stateScene = nlohmann::json::array();
    bool bulletPhysicsStarted = false;

    void StartWorld(b2ContactListener *listener) {
        InitScripts();

        for (auto &gameObject : Scene::m_GameObjects) {
            if (gameObject->prefab)
                continue;

            if (gameObject->HasComponent<m_LuaScriptComponent>()) {
                gameObject->GetComponent<m_LuaScriptComponent>().Start();
            }

            if (gameObject->HasComponent<NativeScriptManager>()) {
                gameObject->GetComponent<NativeScriptManager>().Start();
            }

            if (gameObject->HasComponent<CppScriptManager>()) {
                gameObject->GetComponent<CppScriptManager>().Start();
            }

            if (gameObject->HasComponent<CsharpScriptManager>()) {
                gameObject->GetComponent<CsharpScriptManager>().Start();
            }
        }

        Scene::world = new b2World({0.0, -5.8f});
        Scene::world->SetContactListener(listener);
        auto view = Scene::m_Registry.view<Rigidbody2D>();
        auto view3D = Scene::m_Registry.view<Rigidbody3D>();

        for (auto e : view) {
            GameObject *gameObject;
            for (auto &go : Scene::m_GameObjects) {
                if (go->entity == e) {
                    gameObject = go;
                    break;
                }
            }

            auto &transform = gameObject->GetComponent<Transform>();
            auto &rb2d = gameObject->GetComponent<Rigidbody2D>();

            b2BodyDef bodyDef;
            bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(gameObject);
            bodyDef.type = rb2d.type;
            bodyDef.position.Set(transform.position.x, transform.position.y);
            bodyDef.angle = glm::radians(transform.rotation.z);
            bodyDef.gravityScale = rb2d.gravityScale;

            b2Body *body = Scene::world->CreateBody(&bodyDef);
            body->SetFixedRotation(rb2d.fixedRotation);
            rb2d.body = body;

            if (gameObject->HasComponent<BoxCollider2D>()) {
                auto &boxCollider2D = gameObject->GetComponent<BoxCollider2D>();
                b2PolygonShape shape;
                shape.SetAsBox((((boxCollider2D.size.x) / 2) - 0.02) / 2,
                               (((boxCollider2D.size.y) / 2) - 0.02) / 2);

                b2FixtureDef fixtureDef;
                fixtureDef.isSensor = boxCollider2D.trigger;
                fixtureDef.shape = &shape;
                fixtureDef.density = boxCollider2D.density;
                fixtureDef.friction = boxCollider2D.friction;
                fixtureDef.restitution = boxCollider2D.restitution;
                fixtureDef.restitutionThreshold =
                    boxCollider2D.restitutionThreshold;
                boxCollider2D.fixture = body->CreateFixture(&fixtureDef);
            }
        }

        BulletPhysicsWorld::Init();

        for (auto e : view3D) {
            GameObject *gameObject;
            for (auto &go : Scene::m_GameObjects) {
                if (go->entity == e) {
                    gameObject = go;
                    break;
                }
            }

            auto &rigidbody = gameObject->GetComponent<Rigidbody3D>();
            rigidbody.transform = &gameObject->GetComponent<Transform>();

            if (gameObject->HasComponent<BoxCollider3D>()) {
                auto &collider = gameObject->GetComponent<BoxCollider3D>();
                collider.CreateShape();
                rigidbody.CreateBody(collider.shape);
            }

            if (gameObject->HasComponent<MeshCollider3D>()) {
                auto &collider = gameObject->GetComponent<MeshCollider3D>();
                collider.CreateShape(&gameObject->GetComponent<MeshRenderer>());
                rigidbody.CreateBody(collider.shape);
            }
        }

        auto jointView = Scene::m_Registry.view<FixedJoint3D>();

        for (auto e : jointView) {
            GameObject *gameObject;
            for (auto &go : Scene::m_GameObjects) {
                if (go->entity == e) {
                    gameObject = go;
                    break;
                }
            }

            auto &joint = gameObject->GetComponent<FixedJoint3D>();
            joint.CreateJoint();
        }

        auto pathfindingView = Scene::m_Registry.view<PathfindingAI>();

        for (auto e : pathfindingView) {
            GameObject *gameObject;
            for (auto &go : Scene::m_GameObjects) {
                if (go->entity == e) {
                    gameObject = go;
                    break;
                }
            }

            auto &pathfinding = gameObject->GetComponent<PathfindingAI>();
            pathfinding.CreateGrid();
        }

        bulletPhysicsStarted = true;
    }

    void DeleteWorld() {
        bulletPhysicsStarted = false;

        // halt
        Mix_HaltChannel(-1);
        Mix_HaltMusic();

        for (auto &gameObject : Scene::m_GameObjects) {
            if (gameObject->HasComponent<FixedJoint3D>()) {
                auto &fixedJoint = gameObject->GetComponent<FixedJoint3D>();
                fixedJoint.DeleteJoint();
            }

            if (gameObject->HasComponent<NativeScriptManager>()) {
                auto &script = gameObject->GetComponent<NativeScriptManager>();
                for (auto &script : script.m_StaticScripts) {
                    delete script;
                }

                gameObject->RemoveComponent<NativeScriptManager>();
            }

            if (gameObject->HasComponent<Rigidbody3D>()) {
                auto &component = gameObject->GetComponent<Rigidbody3D>();
                component.DeleteBody();
                if (component.ref) {
                    delete component.ref;
                }
            }

            if (gameObject->HasComponent<PathfindingAI>()) {
                auto &component = gameObject->GetComponent<PathfindingAI>();
                component.DeleteGrid();
            }
        }

        for (auto instance : CsharpScriptEngine::instances) {
            if (instance.second != nullptr)
                delete instance.second;
        }
        CsharpScriptEngine::instances.clear();

        BulletPhysicsWorld::Delete();
    }

    void DrawVec3Control(const std::string &label, Vector3 &values,
                         float resetValue, float columnWidth) {
        ImGui::PushID(label.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 100);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

        float lineHeight =
            GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = {lineHeight + 2.0f, lineHeight};

        // disable button rounded corners
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.8f, 0.1f, 0.15f, 1.0f});

        if (ImGui::Button("X", buttonSize)) {
            values.x = resetValue;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.2f, 0.7f, 0.2f, 1.0f});

        if (ImGui::Button("Y", buttonSize)) {
            values.y = resetValue;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
        if (ImGui::Button("Z", buttonSize)) {
            values.z = resetValue;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::Columns(1);
        ImGui::PopStyleVar(2);
        ImGui::PopID();
    }

    void DrawVec2Control(const std::string &label, Vector2 &values,
                         float resetValue, float columnWidth) {
        ImGui::PushID(label.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 100);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

        float lineHeight =
            GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = {lineHeight + 2.0f, lineHeight};

        // disable button rounded corners
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.8f, 0.1f, 0.15f, 1.0f});

        if (ImGui::Button("X", buttonSize)) {
            values.x = resetValue;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4{0.2f, 0.7f, 0.2f, 1.0f});

        if (ImGui::Button("Y", buttonSize)) {
            values.y = resetValue;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::Columns(1);
        ImGui::PopStyleVar(2);
        ImGui::PopID();
    }

    std::vector<m_SpritesheetAnimationData>
    GetAnimationsFromXML(const char *texPath, float delay, Vector2 sheetSize,
                         const std::string &xmlFile) {
        tinyxml2::XMLDocument doc;
        doc.LoadFile(xmlFile.c_str());

        tinyxml2::XMLElement *root = doc.FirstChildElement("TextureAtlas");
        tinyxml2::XMLElement *subTexture =
            root->FirstChildElement("SubTexture");

        std::vector<m_SpritesheetAnimationData> animations;

        std::string currAnimation = "";
        m_SpritesheetAnimationData animationData;
        int animIndex = -1;
        // animationData has a Frame, which has size and offset, size is
        // width and height, offset is x and y

        while (subTexture) {
            std::string name = subTexture->Attribute("name");
            std::string x = subTexture->Attribute("x");
            std::string y = subTexture->Attribute("y");
            std::string width = subTexture->Attribute("width");
            std::string height = subTexture->Attribute("height");

            // remove last 4 digits???
            std::string animationName = name.substr(0, name.size() - 4);

            if (currAnimation != animationName) {
                animations.push_back(animationData);

                currAnimation = animationName;
                animationData.loop = true;
                animationData = {};
                animIndex = animations.size() - 1;
                strcpy(animationData.name, currAnimation.c_str());
                animationData.delay = delay;
            }

            m_SpritesheetAnimationData::Frame frame;
            frame.size = Vector2(std::stof(width), std::stof(height));
            frame.offset = Vector2(std::stof(x), std::stof(y));
            animationData.frames.push_back(frame);
            if (animIndex != -1) {
                animations[animIndex] = animationData;
            }

            subTexture = subTexture->NextSiblingElement("SubTexture");
        }

        return animations;
    }

    void Model::Draw(Shader &shader, Camera &camera) {
        Transform &mainTransform = mainGameObject->GetComponent<Transform>();
        mainTransform.Update();

        for (uint32_t i = 0; i < m_gameObjects.size(); i++) {
            MeshRenderer &meshRenderer =
                m_gameObjects[i]->GetComponent<MeshRenderer>();
            meshRenderer.m_Mesh->Draw(shader, camera, mainTransform.transform);
        }
    }

    void Model::loadModel(std::string path) {
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(
            path, aiProcess_Triangulate | aiProcess_CalcTangentSpace |
                      aiProcess_PreTransformVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
            !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString()
                      << std::endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode *node, const aiScene *scene) {
        // process all the node's meshes (if any)
        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            // get name
            std::string name = mesh->mName.C_Str();

            // get matricies
            aiMatrix4x4 aiTransform = node->mTransformation;
            transform = glm::mat4(1.0f);
            transform[0][0] = aiTransform.a1;
            transform[1][0] = aiTransform.b1;
            transform[2][0] = aiTransform.c1;
            transform[3][0] = aiTransform.d1;
            transform[0][1] = aiTransform.a2;
            transform[1][1] = aiTransform.b2;
            transform[2][1] = aiTransform.c2;
            transform[3][1] = aiTransform.d2;
            transform[0][2] = aiTransform.a3;
            transform[1][2] = aiTransform.b3;
            transform[2][2] = aiTransform.c3;
            transform[3][2] = aiTransform.d3;
            transform[0][3] = aiTransform.a4;
            transform[1][3] = aiTransform.b4;
            transform[2][3] = aiTransform.c4;
            transform[3][3] = aiTransform.d4;

            m_gameObjects.push_back(processMesh(mesh, scene, name));
            auto &meshRenderer = m_gameObjects[i]->GetComponent<MeshRenderer>();
            meshRenderer.extraMatrix = transform;
            meshRenderer.mesh_index = i;
        }
        // then do the same for each of its children
        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    GameObject *Model::processMesh(aiMesh *mesh, const aiScene *scene,
                                   const std::string &name) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<Texture *> textures;

        for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            SetVertexBoneDataToDefault(vertex);
            vertex.position =
                glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y,
                          mesh->mVertices[i].z);
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y,
                                      mesh->mNormals[i].z);
            if (mesh->mTextureCoords[0]) {
                vertex.texUV = glm::vec2(mesh->mTextureCoords[0][i].x,
                                         mesh->mTextureCoords[0][i].y);
            } else {
                vertex.texUV = glm::vec2(0.0f, 0.0f);
            }
            vertex.tangent =
                glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y,
                          mesh->mTangents[i].z);
            vertex.bitangent =
                glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y,
                          mesh->mBitangents[i].z);
            vertices.push_back(vertex);
        }
        ExtractBoneWeightForVertices(vertices, mesh, scene);

        // indices
        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        Texture *diffuse = nullptr;
        Texture *specular = nullptr;
        Texture *normal = nullptr;

        if (mesh->mMaterialIndex >= 0 && texturesEnabled) {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<Texture *> diffuseMaps = loadMaterialTextures(
                material, aiTextureType_DIFFUSE, "texture_diffuse");
            diffuse = diffuseMaps[0];
            textures.insert(textures.end(), diffuseMaps.begin(),
                            diffuseMaps.end());
            std::vector<Texture *> specularMaps = loadMaterialTextures(
                material, aiTextureType_SPECULAR, "texture_specular");
            specular = specularMaps[0];
            textures.insert(textures.end(), specularMaps.begin(),
                            specularMaps.end());
        }

        if (texturesEnabled) {
            Material material(Color);
            material.diffuse = diffuse;
            material.specular = specular;

            GameObject *gameObject = new GameObject();
            gameObject->name = name;
            gameObject->parentID = mainGameObject->ID;
            gameObject->AddComponent<Transform>();
            gameObject->AddComponent<MeshRenderer>();

            MeshRenderer &meshRenderer =
                gameObject->GetComponent<MeshRenderer>();
            meshRenderer.m_Mesh = new Mesh(vertices, indices, material);
            meshRenderer.m_Model = true;
            meshRenderer.meshType = std::string(path);
            Scene::m_GameObjects.push_back(gameObject);

            return gameObject;
        } else {
            Material material(Color);
            material.diffuse = diffuse;
            material.specular = specular;

            GameObject *gameObject = new GameObject();
            gameObject->name = name;
            gameObject->parentID = mainGameObject->ID;
            gameObject->AddComponent<Transform>();
            gameObject->AddComponent<MeshRenderer>();

            MeshRenderer &meshRenderer =
                gameObject->GetComponent<MeshRenderer>();
            meshRenderer.m_Mesh = new Mesh(vertices, indices, material);
            meshRenderer.m_Model = true;
            meshRenderer.meshType = std::string(path);
            Scene::m_GameObjects.push_back(gameObject);

            return gameObject;
        }
    }

    std::vector<Texture *> Model::loadMaterialTextures(aiMaterial *mat,
                                                       aiTextureType type,
                                                       std::string typeName) {
        std::vector<Texture *> textures;
        for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            std::string texPath = directory + '/' + str.C_Str();
            for (uint32_t j = 0; j < textures_loaded.size(); j++) {
                if (std::strcmp(textures_loaded[j]->tex->texStarterPath,
                                texPath.c_str()) == 0) {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }

            if (!skip) { // if texture hasn't been loaded already, load it
                std::string texPath = directory + '/' + str.C_Str();
                Texture *texture =
                    new Texture(texPath.c_str(), i, typeName.c_str());
                textures.push_back(texture);
                textures_loaded.push_back(texture); // add to loaded textures
                currSlot++;
            }
        }
        return textures;
    }
} // namespace HyperAPI::Experimental