#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"
#include "../Renderer/DecomposeTransform.hpp"

#include "GameObject.hpp"
#include "Bone.hpp"
#include "Transform.hpp"

namespace HyperAPI::Experimental {
    class Model {
    private:
        int currSlot = 0;

    public:
        std::map<std::string, BoneInfo> m_BoneInfoMap;
        std::vector<Experimental::GameObject *> bones;
        int m_BoneCounter = 0;
        auto &GetBoneInfoMap() { return m_BoneInfoMap; }

        int &GetBoneCounter() { return m_BoneCounter; }

        void SetVertexBoneDataToDefault(Vertex &vertex) {
            for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
                vertex.m_BoneIDs[i] = -1;
                vertex.m_Weights[i] = 0.0f;
            }
        }
        int bonesStuffIdkFuckThisShit = 0;
        void SetVertexBoneData(Vertex &vertex, int boneID, float weight,
                               const std::string &boneName) {
            for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
                if (vertex.m_BoneIDs[i] == -1) {
                    vertex.m_BoneIDs[i] = boneID;
                    vertex.m_Weights[i] = weight;
                    return;
                }
            }
        }

        void ExtractBoneWeightForVertices(std::vector<Vertex> &vertices,
                                          aiMesh *mesh, const aiScene *scene) {
            for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
                int boneID = -1;
                std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
                if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
                    BoneInfo newBoneInfo;
                    newBoneInfo.id = m_BoneCounter;
                    newBoneInfo.offset =
                        AssimpGLMHelpers::ConvertMatrixToGLMFormat(
                            mesh->mBones[boneIndex]->mOffsetMatrix);
                    m_BoneInfoMap[boneName] = newBoneInfo;
                    boneID = m_BoneCounter;

                    Experimental::GameObject *bone =
                        new Experimental::GameObject();
                    bone->name = boneName;
                    bone->parentID = mainGameObject->ID;
                    bone->AddComponent<Transform>();
                    auto &transform = bone->GetComponent<Transform>();
                    glm::vec3 pos, rot, scal;
                    DecomposeTransform(newBoneInfo.offset, pos, rot, scal);
                    transform.position = pos;
                    transform.rotation = rot;
                    Scene::m_GameObjects.push_back(bone);
                    bones.push_back(bone);

                    m_BoneCounter++;
                } else {
                    boneID = m_BoneInfoMap[boneName].id;
                }
                assert(boneID != -1);
                auto weights = mesh->mBones[boneIndex]->mWeights;
                int numWeights = mesh->mBones[boneIndex]->mNumWeights;

                for (int weightIndex = 0; weightIndex < numWeights;
                     ++weightIndex) {
                    int vertexID = weights[weightIndex].mVertexId;
                    float weight = weights[weightIndex].mWeight;
                    SetVertexBoneData(vertices[vertexID], boneID, weight,
                                      boneName);
                }
            }
        }

        GameObject *mainGameObject;
        std::vector<GameObject *> m_gameObjects;
        std::string directory;

        bool texturesEnabled = true;
        Vector4 Color;

        std::string path;

        glm::mat4 transform;

        void loadModel(std::string path);

        void processNode(aiNode *node, const aiScene *scene);

        GameObject *processMesh(aiMesh *mesh, const aiScene *scene,
                                const std::string &name);

        std::vector<Texture *> textures_loaded;

        std::vector<Texture *> loadMaterialTextures(aiMaterial *mat,
                                                    aiTextureType type,
                                                    std::string typeName);

        Model(char *path, bool AddTexture = false,
              Vector4 color = Vector4(1, 1, 1, 1)) {
            this->path = std::string(path);
            Color = color;

            mainGameObject = new GameObject();
            mainGameObject->name = "Model";
            mainGameObject->ID = uuid::generate_uuid_v4();
            mainGameObject->AddComponent<Transform>();
            Scene::m_GameObjects.push_back(mainGameObject);

            texturesEnabled = AddTexture;

            loadModel(path);
        }

        void Draw(Shader &shader, Camera &camera,
                  std::vector<glm::mat4> &transforms);
    };
} // namespace HyperAPI::Experimental