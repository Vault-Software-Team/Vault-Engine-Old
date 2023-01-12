#pragma once
#include <libs.hpp>
#include "Material.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Structures.hpp"
#include "AssimpGLMHelpers.hpp"

namespace HyperAPI {
    class Model : public ComponentSystem {
    private:
        int currSlot = 0;

    public:
        std::map<std::string, BoneInfo> m_BoneInfoMap;
        int m_BoneCounter = 0;

        auto &GetBoneInfoMap() { return m_BoneInfoMap; }

        int &GetBoneCounter() { return m_BoneCounter; }

        void SetVertexBoneDataToDefault(Vertex &vertex) {
            for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
                vertex.m_BoneIDs[i] = -1;
                vertex.m_Weights[i] = 0.0f;
            }
        }

        void SetVertexBoneData(Vertex &vertex, int boneID, float weight) {
            for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
                if (vertex.m_BoneIDs[i] < 0) {
                    vertex.m_Weights[i] = weight;
                    vertex.m_BoneIDs[i] = boneID;

                    break;
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
                    m_BoneCounter++;
                } else {
                    boneID = m_BoneInfoMap[boneName].id;
                }
                assert(boneID != -1);
                auto weights = mesh->mBones[boneIndex]->mWeights;
                int numWeights = mesh->mBones[boneIndex]->mNumWeights;

                for (int weightIndex = 0; weightIndex < numWeights;
                     ++weightIndex) {
                    int vertexId = weights[weightIndex].mVertexId;
                    float weight = weights[weightIndex].mWeight;
                    assert(vertexId <= vertices.size());
                    SetVertexBoneData(vertices[vertexId], boneID, weight);
                }
            }
        }

        std::vector<Mesh *> meshes;
        std::string directory;

        bool texturesEnabled = true;
        Vector4 Color;

        std::string path;

        glm::mat4 transform;

        void loadModel(std::string path);

        void processNode(aiNode *node, const aiScene *scene);

        Mesh *processMesh(aiMesh *mesh, const aiScene *scene,
                          const std::string &name);

        std::vector<Texture> textures_loaded;

        std::vector<Texture> loadMaterialTextures(aiMaterial *mat,
                                                  aiTextureType type,
                                                  std::string typeName);

        Model(char *path, bool AddTexture = false,
              Vector4 color = Vector4(1, 1, 1, 1)) {
            this->path = std::string(path);
            Color = color;
            this->name = "Model";
            texturesEnabled = AddTexture;
            TransformComponent m_transform;
            AddComponent(m_transform);
            loadModel(path);
            // scriptComponent.componentSystem = this;
        }
        //
        //        ~Model() {
        ////            for(auto &mesh : meshes) {
        ////                if(mesh != nullptr) {
        ////                    delete mesh;
        ////                }
        ////            }
        //        }

        void Draw(Shader &shader, Camera &camera);
    };
} // namespace HyperAPI