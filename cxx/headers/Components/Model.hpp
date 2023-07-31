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
        void SetVertexBoneData(Vertex &vertex, int boneID, float weight) {
            for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
                if (vertex.m_BoneIDs[i] < 0) {
                    vertex.m_Weights[i] = weight;
                    vertex.m_BoneIDs[i] = boneID;
                    break;
                }
            }
        }

        void ExtractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh *mesh, const aiScene *scene);

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
                                const std::string &name, int index);

        struct MeshMaterial {
            Mesh *mesh;
            std::string mat_path;
        };
        std::vector<MeshMaterial *> mesh_mats;
        MeshMaterial *mesh_processMesh(aiMesh *mesh, const aiScene *scene,
                                       const std::string &name);

        std::vector<Texture *> textures_loaded;

        std::vector<Texture *> loadMaterialTextures(aiMaterial *mat,
                                                    aiTextureType type,
                                                    std::string typeName);
        bool no_gm;
        int mesh_index;
        Model(char *path, bool AddTexture = false,
              Vector4 color = Vector4(1, 1, 1, 1), bool m_nogm = false, int mesh_index = 0) {
            this->path = std::string(path);
            Color = color;

            no_gm = m_nogm;
            this->mesh_index = mesh_index;

            if (!m_nogm) {
                mainGameObject = new GameObject();
                mainGameObject->name = "Model";
                mainGameObject->ID = uuid::generate_uuid_v4();
                mainGameObject->AddComponent<Transform>();
                Scene::m_GameObjects->push_back(mainGameObject);
            }

            texturesEnabled = AddTexture;

            loadModel(path);
        }

        void Draw(Shader &shader, Camera &camera);
    };

    struct AssimpNodeData {
        glm::mat4 transformation;
        std::string name;
        int childrenCount;
        std::vector<AssimpNodeData> children;
    };

    class Animation {
    public:
        Animation() = default;

        Animation(const std::string &animationPath, Experimental::Model *model) {
            Assimp::Importer importer;
            const aiScene *scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
            assert(scene && scene->mRootNode);
            auto animation = scene->mAnimations[0];
            m_Duration = animation->mDuration;
            m_TicksPerSecond = animation->mTicksPerSecond;
            ReadHeirarchyData(m_RootNode, scene->mRootNode);
            ReadMissingBones(animation, *model);
        }

        ~Animation() {
        }

        Bone *FindBone(const std::string &name) {
            auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
                                     [&](const Bone &Bone) {
                                         return Bone.GetBoneName() == name;
                                     });
            if (iter == m_Bones.end())
                return nullptr;
            else
                return &(*iter);
        }

        inline float GetTicksPerSecond() { return m_TicksPerSecond; }

        inline float GetDuration() { return m_Duration; }

        inline const AssimpNodeData &GetRootNode() { return m_RootNode; }

        inline const std::map<std::string, BoneInfo> &GetBoneIDMap() {
            return m_BoneInfoMap;
        }

    private:
        void ReadMissingBones(const aiAnimation *animation, Model &model) {
            int size = animation->mNumChannels;

            auto &boneInfoMap = model.GetBoneInfoMap(); // getting m_BoneInfoMap from Model class
            int &boneCount = model.GetBoneCounter();    // getting the m_BoneCounter from Model class

            // reading channels(bones engaged in an animation and their keyframes)
            for (int i = 0; i < size; i++) {
                auto channel = animation->mChannels[i];
                std::string boneName = channel->mNodeName.data;

                if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
                    boneInfoMap[boneName].id = boneCount;
                    boneCount++;
                }
                m_Bones.push_back(Bone(channel->mNodeName.data,
                                       boneInfoMap[channel->mNodeName.data].id, channel));
            }

            m_BoneInfoMap = boneInfoMap;
        }

        void ReadHeirarchyData(AssimpNodeData &dest, const aiNode *src) {
            assert(src);

            dest.name = src->mName.data;
            dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
            dest.childrenCount = src->mNumChildren;

            for (int i = 0; i < src->mNumChildren; i++) {
                AssimpNodeData newData;
                ReadHeirarchyData(newData, src->mChildren[i]);
                dest.children.push_back(newData);
            }
        }
        float m_Duration;
        int m_TicksPerSecond;
        std::vector<Bone> m_Bones;
        AssimpNodeData m_RootNode;
        std::map<std::string, BoneInfo> m_BoneInfoMap;
    };

    class Animator {
    public:
        Animator(Animation *Animation) {
            m_CurrentTime = 0.0;
            m_CurrentAnimation = Animation;

            m_FinalBoneMatrices.reserve(100);

            for (int i = 0; i < 100; i++)
                m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
        }

        void UpdateAnimation(float dt) {
            m_DeltaTime = dt;
            if (m_CurrentAnimation) {
                m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
                m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
                CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
            }
        }

        void PlayAnimation(Animation *pAnimation) {
            m_CurrentAnimation = pAnimation;
            m_CurrentTime = 0.0f;
        }

        void CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform) {
            std::string nodeName = node->name;
            glm::mat4 nodeTransform = node->transformation;

            Bone *Bone = m_CurrentAnimation->FindBone(nodeName);

            if (Bone) {
                Bone->Update(m_CurrentTime);
                nodeTransform = Bone->GetLocalTransform();
            }

            glm::mat4 globalTransformation = parentTransform * nodeTransform;

            auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
            if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
                int index = boneInfoMap[nodeName].id;
                glm::mat4 offset = boneInfoMap[nodeName].offset;
                m_FinalBoneMatrices[index] = globalTransformation * offset;
            }

            for (int i = 0; i < node->childrenCount; i++)
                CalculateBoneTransform(&node->children[i], globalTransformation);
        }

        std::vector<glm::mat4> GetFinalBoneMatrices() {
            return m_FinalBoneMatrices;
        }

    private:
        std::vector<glm::mat4> m_FinalBoneMatrices;
        Animation *m_CurrentAnimation;
        float m_CurrentTime;
        float m_DeltaTime;
    };
} // namespace HyperAPI::Experimental