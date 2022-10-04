#pragma once

#include "../vendor/glad/include/glad/glad.h"
#include "../vendor/GLFW/glfw3.h"
#include <ImGuizmo/ImGuizmo.h>
#include <ImGuizmo/ImSequencer.h>
#include <fstream>
#include <iostream>
#include "../vendor/discord-rpc/discord_rpc.h"
#include "../vendor/stb_image/stb_image.h"
// #include "../vendor/stb_image/stb_image.h"
#include "../vendor/glm/glm.hpp"
#include "../vendor/tinyxml/tinyxml2.h"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/glm/gtc/type_ptr.hpp"
#include "../vendor/glm/ext.hpp"
#include "../vendor/glm/gtx/quaternion.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include "../vendor/glm/gtx/matrix_decompose.hpp"
#include "../vendor/glm/gtx/rotate_vector.hpp"
#include "../vendor/glm/gtx/vector_angle.hpp"
#include "../vendor/json/json.hpp"
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_internal.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"
#include "../vendor/icons/icons.h"
#include "../vendor/entt/entt.hpp"
#include "../vendor/ImGuiFileDialog/ImGuiFileDialog.h"
#include <vector>
#include <functional>
#include <experimental/filesystem>
#include "../vendor/assimp/Importer.hpp"
#include "../vendor/assimp/scene.h"
//import glm helpers
#include "../vendor/box2d/box2d.h"
#include "../vendor/assimp/postprocess.h"
#include <any>
#include "scene.hpp"
#include "ScriptEngine.hpp"
#include "InputEvents.hpp"
#include "scripts.hpp"
#include "../vendor/SDL2/SDL_mixer.h"
#include <random>

// bullet physics
#include "../vendor/bullet/bullet/btBulletDynamicsCommon.h"
#include "../vendor/bullet/bullet/BulletCollision/CollisionDispatch/btGhostObject.h"

#ifndef _WIN32

#include <unistd.h>

#else
#include <windows.h>
#endif

#define Vector2 glm::vec2
#define Vector3 glm::vec3
#define Vector4 glm::vec4

#define MAX_BONE_INFLUENCE 4

// command prompt colors
#ifdef _WIN32
#define RED SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12)
#define GREEN SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10)
#define BLUE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9)
#define YELLOW SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14)
#define WHITE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15)
#define PURPLE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13)
#define CYAN SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11)
#define GREY SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8)
#define RESET SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7)
#define HYPER_LOG(x) RED; std::cout << "[STATIC] - "; RESET; std::cout << x << std::endl;
#else
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"
#define RESET "\033[0m"
#define HYPER_LOG(x) std::cout << RED "[STATIC] - " RESET << x << std::endl;
#endif

using json = nlohmann::json;

namespace uuid {
    extern std::random_device rd;
    extern std::mt19937 gen;
    extern std::uniform_int_distribution<> dis;
    extern std::uniform_int_distribution<> dis2;

    std::string generate_uuid_v4();
}

namespace HyperAPI {
    extern std::string cwd;
    extern std::string dirPayloadData;
    extern bool isRunning;
    extern bool isStopped;

    bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &translation, glm::vec3 &rotation, glm::vec3 &scale);

    namespace AudioEngine {
        void PlaySound(const std::string &path, float volume = 1.0f, bool loop = false, int channel = -1);

        void StopSound(int channel = -1);

        void PlayMusic(const std::string &path, float volume = 1.0f, bool loop = false);

        void StopMusic();
    }

    namespace BulletPhysicsWorld {
        extern btDiscreteDynamicsWorld *dynamicsWorld;
        extern btBroadphaseInterface *broadphase;
        extern btDefaultCollisionConfiguration *collisionConfiguration;
        extern btCollisionDispatcher *dispatcher;
        extern btSequentialImpulseConstraintSolver *solver;
        extern btGhostPairCallback *ghostPairCallback;
        extern btAlignedObjectArray<btCollisionShape *> collisionShapes;
        extern btAlignedObjectArray<btRigidBody *> rigidBodies;
        extern btAlignedObjectArray<btPairCachingGhostObject *> ghostObjects;
        extern btAlignedObjectArray<btTypedConstraint *> constraints;
        extern btAlignedObjectArray<btCollisionObject *> collisionObjects;

        void Delete();
        void Init();
        void UpdatePhysics();

        void CollisionCallback(std::function<void(const std::string&, const std::string&)> HandleEntities);
    }

    namespace Timestep {
        extern float deltaTime;
        extern float lastFrame;
        extern float currentFrame;
    }

    class AssimpGLMHelpers {
    public:

        static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4 &from) {
            glm::mat4 to;
            //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
            to[0][0] = from.a1;
            to[1][0] = from.a2;
            to[2][0] = from.a3;
            to[3][0] = from.a4;
            to[0][1] = from.b1;
            to[1][1] = from.b2;
            to[2][1] = from.b3;
            to[3][1] = from.b4;
            to[0][2] = from.c1;
            to[1][2] = from.c2;
            to[2][2] = from.c3;
            to[3][2] = from.c4;
            to[0][3] = from.d1;
            to[1][3] = from.d2;
            to[2][3] = from.d3;
            to[3][3] = from.d4;
            return to;
        }

        static inline glm::vec3 GetGLMVec(const aiVector3D &vec) {
            return glm::vec3(vec.x, vec.y, vec.z);
        }

        static inline glm::quat GetGLMQuat(const aiQuaternion &pOrientation) {
            return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
        }
    };

    namespace Scene {
        void DropTargetMat(DragType type, Mesh *currEntity);
    }

    struct Component {
        bool IsPubliclyAddable = false;
    };

    class ComponentSystem {
    public:
        std::string ID = uuid::generate_uuid_v4();
        std::string name = "GameObject";

        std::string Tag;
        // HyperAPI::LuaScriptComponent scriptComponent;
        std::vector<std::any> Components;

        ComponentSystem() = default;

        void SetTag(const std::string &str) {
            Tag = str;
        }

        template<typename T>
        void AddComponent(T component) {
            Components.push_back(std::any_cast<T>(component));
        }

        template<typename T>
        T GetComponent() {
            for (auto component : Components) {
                try {
                    if (typeid(T) == typeid(std::any_cast<T>(component))) {
                        return std::any_cast<T>(component);
                    }
                }
                catch (const std::bad_any_cast &e) {}
            }
        }

        template<typename T>
        bool HasComponent() {
            for (auto component : Components) {
                if (typeid(T) == typeid(std::any_cast<T>(component))) {
                    return true;
                }
            }
            return false;
        }

        template<typename T>
        void UpdateComponent(T component) {
            for (auto &comp : Components) {
                if (typeid(T) == typeid(std::any_cast<T>(comp))) {
                    comp = component;
                }
            }
        }
    };

    struct KeyPosition {
        glm::vec3 position;
        float timeStamp;
    };

    struct KeyRotation {
        glm::quat orientation;
        float timeStamp;
    };

    struct KeyScale {
        glm::vec3 scale;
        float timeStamp;
    };

    struct TransformComponent : public Component {
        glm::mat4 transform = glm::mat4(1.0f);
        glm::vec3 position = glm::vec3(0, 0, 0);
        glm::vec3 rotation = glm::vec3(0, 0, 0);
        glm::vec3 scale = glm::vec3(1, 1, 1);

        void GUI() {
            // OLD ui
//           ImGui::DragFloat3("Position", &position.x, 0.1f);
//           ImGui::DragFloat3("Rotation", &rotation.x, 0.1f);
//           ImGui::DragFloat3("Scale", &scale.x, 0.1f);
// ---------------------------------------------------------------------------------------------------------
//            NEW UI HERE
// ---------------------------------------------------------------------------------------------------------
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, 100);
            ImGui::Text("Position");
            ImGui::NextColumn();
            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        }
    };

    struct Character {
        unsigned int TextureID;  // ID handle of the glyph texture
        glm::ivec2 Size;       // Size of glyph
        glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };


    enum LOG_TYPE {
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR
    };

    struct Log {
        std::string message;
        LOG_TYPE type;

        Log(const std::string &message, LOG_TYPE type) {
            this->message = message;
            this->type = type;
            Scene::logs.push_back(*this);
        }

        void GUI() {
            switch (type) {
                case LOG_INFO:
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ICON_FA_MESSAGE);
                    // same line
                    ImGui::SameLine();
                    ImGui::Text((std::string(" - ") + message).c_str());
                    break;
                case LOG_WARNING:
                    ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), ICON_FA_TRIANGLE_EXCLAMATION);
                    // same line
                    ImGui::SameLine();
                    ImGui::Text((std::string(" - ") + message).c_str());
                    break;
                case LOG_ERROR:
                    ImGui::TextColored(ImVec4(1.2f, 0.0f, 0.0f, 1.0f), ICON_FA_CIRCLE_EXCLAMATION);
                    // same line
                    ImGui::SameLine();
                    ImGui::Text((std::string(" - ") + message).c_str());
                    break;
            }
        }
    };

    class Shader {
    public:
        unsigned int ID;

        Shader(const char *shaderPath);

        void Bind();

        void Unbind();

        // set uniforms
        void SetUniform1i(const char *name, int value);

        void SetUniform1f(const char *name, float value);

        void SetUniform2f(const char *name, float v0, float v1);

        void SetUniform3f(const char *name, float v0, float v1, float v2);

        void SetUniform4f(const char *name, float v0, float v1, float v2, float v3);

        void SetUniformMat4(const char *name, glm::mat4 value);
    };

    struct CameraPosDec {
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    };

    class Camera : public ComponentSystem {
    public:
        float cursorWinW, cursorWinH;
        // glm::vec3 Position;
        // glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        // glm::vec3 RotationValue = glm::vec3(0.0f, 0.0f, 0.0f);
        bool EnttComp = false;
        bool m_MouseMovement = false;
        entt::entity entity = entt::null;

        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 view, projection;

        float cam_fov = 45;
        float cam_near = 0.1f;
        float cam_far = 100.0f;

        float rotX, rotY;

        bool firstClick = true;
        bool mode2D = false;
        bool mainCamera = false;

        int width;
        int height;

        float speed = 0.00000000000001f;
        float sensitivity = 100.0f;

        glm::mat4 camMatrix;

        Camera(bool mode2D, int width, int height, glm::vec3 position, entt::entity entity = entt::null);

        void updateMatrix(float FOVdeg, float nearPlane, float farPlane, Vector2 winSize, Vector2 prespectiveSize = Vector2(-15, -15));
        void Matrix(Shader &shader, const char *uniform);
        void Inputs(GLFWwindow *window, Vector2 winPos);
        void MouseMovement(Vector2 winPos);
    };

    class Texture {
    public:
        unsigned int ID;
        int width, height, nrChannels;
        unsigned char *data;
        const char *texType;
        unsigned int slot;
        std::string texPath;
        const char *texStarterPath;

        Texture(const char *texturePath, unsigned int slot, const char *textureType);

        void Bind(unsigned int slot = -1);

        void Unbind();
    };

    struct BoneInfo {
        int id;
        glm::mat4 offset;
    };

    struct Vertex {
        glm::vec3 position;
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec2 texUV = glm::vec2(0.0f, 0.0f);
        int m_BoneIDs[MAX_BONE_INFLUENCE] = {-1};
        float m_Weights[MAX_BONE_INFLUENCE] = {0.0f};
        // float texID = -1;
        // glm::mat4 model = glm::mat4(1.0f);
    };

    struct Vertex_Batch {
        glm::vec3 position;
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec2 texUV = glm::vec2(0.0f, 0.0f);

        int diffuse = -1;
        int specular = -1;
        int normalMap = -1;

        float metallic = 0;
        float roughness = 0;
        Vector2 texUVs = Vector2(0, 0);

        glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

        // float texID = -1;
        // glm::mat4 model = glm::mat4(1.0f);
    };

    struct PointLight : public ComponentSystem {
        glm::vec3 lightPos;
        glm::vec3 color;
        float intensity;

        PointLight(std::vector<HyperAPI::PointLight *> &lights, glm::vec3 lightPos, glm::vec3 color, float intensity) {
            this->lightPos = lightPos;
            this->color = color;
            this->intensity = intensity;

            TransformComponent transform;
            transform.position = lightPos;
            AddComponent(transform);

            lights.push_back(this);
            // scriptComponent.componentSystem = this;
        }
    };

    struct SpotLight : public ComponentSystem {
        glm::vec3 lightPos;
        glm::vec3 color;
        float outerCone;
        float innerCone;
        Vector3 angle = Vector3(0.0f, -1.0f, 0.0f);

        SpotLight(std::vector<HyperAPI::SpotLight *> &lights, glm::vec3 lightPos, glm::vec3 color,
                  float outerCone = 0.9, float innerCone = 0.95) {
            this->lightPos = lightPos;
            this->color = color;
            this->outerCone = outerCone;
            this->innerCone = innerCone;

            TransformComponent transform;
            transform.position = lightPos;
            transform.rotation.y = -1;
            AddComponent(transform);

            lights.push_back(this);
            // scriptComponent.componentSystem = this;
        }
    };

    struct DirectionalLight : public ComponentSystem {
        glm::vec3 lightPos;
        glm::vec3 color;
        float intensity = 1;

        DirectionalLight(std::vector<HyperAPI::DirectionalLight *> &lights, glm::vec3 lightPos, glm::vec3 color) {
            this->lightPos = lightPos;
            this->color = color;

            TransformComponent transform;
            transform.position = lightPos;
            AddComponent(transform);

            lights.push_back(this);
            // scriptComponent.componentSystem = this;
        }
    };

    struct Light2D : public ComponentSystem {
        glm::vec2 lightPos;
        glm::vec3 color;
        float range;

        Light2D(std::vector<Light2D *> &lights, Vector2 lightPos, Vector4 color, float range) {
            this->lightPos = lightPos;
            this->color = color;
            this->range = range;

            TransformComponent transform;
            AddComponent(transform);

            lights.push_back(this);
            // scriptComponent.componentSystem = this;
        }
    };

    struct Instanced {
        bool isInstanced = false;
        std::vector<TransformComponent> transforms = {};
        int count = 1;

        Instanced(bool instanced, int count = 1, std::vector<TransformComponent> transforms = {}) {
            isInstanced = instanced;
            this->count = count;
            this->transforms = transforms;
        }
    };

    class Material {
    public:
        std::vector<Texture> textures;

        Texture *diffuse = nullptr;
        Texture *specular = nullptr;
        Texture *normal = nullptr;

        Vector4 baseColor;
        float shininess;
        float metallic;
        float roughness;
        Vector2 texUVs = Vector2(0, 0);

        Material(Vector4 baseColor = Vector4(1, 1, 1, 1), std::vector<Texture> textures = {}, float shininess = 0,
                 float metallic = 0, float roughness = 0);

        ~Material() {
            if (diffuse != nullptr) {
                delete diffuse;
            }

            if (specular != nullptr) {
                delete specular;
            }

            if (normal != nullptr) {
                delete normal;
            }
        }

        void Bind(Shader &shader);

        void Unbind(Shader &shader);
    };

    class Mesh : public ComponentSystem {
    public:
        std::string parentType = "None";
        Material material{Vector4(1, 1, 1, 1)};

        unsigned int VBO, VAO, IBO;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        glm::mat4 model = glm::mat4(1.0f);

        Vector4 Color;
        bool physics = true;
        bool empty = false;
        bool modelMesh = false;
        bool batched;

        bool hasMaterial = true;

        Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, Material &material, bool empty = false,
             bool batched = false);

        void Draw(
                Shader &shader,
                Camera &camera,
                glm::mat4 matrix = glm::mat4(1.0f),
                glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
                glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
        );
    };

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

        void ExtractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh *mesh, const aiScene *scene) {
            for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
                int boneID = -1;
                std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
                if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
                    BoneInfo newBoneInfo;
                    newBoneInfo.id = m_BoneCounter;
                    newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(
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

                for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
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

        Mesh *processMesh(aiMesh *mesh, const aiScene *scene, const std::string &name);

        std::vector<Texture> textures_loaded;

        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                  std::string typeName);

        Model(char *path, bool AddTexture = false, Vector4 color = Vector4(1, 1, 1, 1)) {
            this->path = std::string(path);
            Color = color;
            this->name = "Model";
            texturesEnabled = AddTexture;
            TransformComponent m_transform;
            AddComponent(m_transform);
            loadModel(path);
            // scriptComponent.componentSystem = this;
        }

        void Draw(Shader &shader, Camera &camera);
    };

    class Skybox {
    public:
        unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
        Shader *shader;
        unsigned int cubemapTexture;
        std::vector<std::string> facesCubemap;

        Skybox(const std::string &right, const std::string &left, const std::string &top, const std::string &bottom,
               const std::string &front, const std::string &back);

        void Draw(Camera &camera, int width, int height);
    };

    class Renderer {
    public:
        bool wireframe;
        unsigned int postProcessingTexture;
        unsigned int postProcessingFBO;
        Shader *framebufferShader;
        unsigned int FBO;
        unsigned int bufferTexture;
        unsigned int rbo;
        unsigned int rectVAO, rectVBO;
        int width, height;

        const char *title;

        unsigned int samples = 8;

        std::vector<PointLight> PointLights;

        GLFWwindow *window;

        Renderer(int width, int height, const char *title, Vector2 g_gravity, unsigned int samples = 8,
                 bool fullscreen = false, bool resizable = true, bool wireframe = false);

        void Render(Camera &camera);

        void Swap(HyperAPI::Shader &framebufferShader, unsigned int FBO, unsigned int rectVAO,
                  unsigned int postProcessingTexture, unsigned int postProcessingFBO);

        void NewFrame();
    };

    class Sprite {
    public:
        Mesh *m_Mesh;

        Sprite(const char *texPath);

        template<typename T>
        void AddComponent(T component) {
            m_Mesh->AddComponent(component);
        }

        template<typename T>
        T GetComponent() {
            return m_Mesh->GetComponent<T>();
        }

        template<typename T>
        void UpdateComponent(T component) {
            m_Mesh->UpdateComponent<T>(component);
        }

        void Draw(Shader &shader, Camera &camera, glm::mat4 trans = glm::mat4(1));
    };

    class Spritesheet {
    public:
        Mesh *m_Mesh;

        Spritesheet(const char *texPath, Material &mat, Vector2 sheetSize, Vector2 spriteSize, Vector2 spriteCoords);

        template<typename T>
        void AddComponent(T component) {
            m_Mesh->AddComponent(component);
        }

        template<typename T>
        T GetComponent() {
            return m_Mesh->GetComponent<T>();
        }

        template<typename T>
        void UpdateComponent(T component) {
            m_Mesh->UpdateComponent<T>(component);
        }

        void Draw(Shader &shader, Camera &camera);
    };

    struct Animation {
        std::vector<Spritesheet> frames;
        int currentFrame = 0;
        float delay = 0.1;
        std::string keyframe;
    };

    class SpritesheetAnimation {
    public:
        std::vector<Animation *> animations;
        Animation *currentAnimation = nullptr;

        float time = 0.1;
        float prevTime = 0;

        SpritesheetAnimation() = default;

        void AddAnimation(Animation *animation) {
            animations.push_back(animation);
        }

        void SelectAnimation(std::string keyframe) {
            for (auto animation : animations) {
                if (animation->keyframe.rfind(keyframe, 0) == 0) {
                    currentAnimation = animation;
                }
            }
        }

        void Draw(Shader &shader, Camera &camera) {
            //get delta time
            float currentTime = glfwGetTime();
            float deltaTime = currentTime - prevTime;
            prevTime = currentTime;

            time -= deltaTime;

            if (currentAnimation != nullptr && currentAnimation->frames.size() > 0) {
                currentAnimation->frames[currentAnimation->currentFrame].Draw(shader, camera);
                if (time <= 0) {
                    currentAnimation->currentFrame++;
                    if (currentAnimation->currentFrame >= currentAnimation->frames.size() - 1) {
                        currentAnimation->currentFrame = 0;
                    }

                    time = currentAnimation->delay;
                }
            }
        }
    };

    class Graphic {
    public:
        Mesh *m_Mesh;

        Graphic(Vector3 rgb);

        template<typename T>
        void AddComponent(T component) {
            m_Mesh->AddComponent(component);
        }

        template<typename T>
        T GetComponent() {
            return m_Mesh->GetComponent<T>();
        }

        template<typename T>
        void UpdateComponent(T component) {
            m_Mesh->UpdateComponent<T>(component);
        }

        void Draw(Shader &shader, Camera &camera);
    };

    class Capsule : public Model {
    public:
        Capsule(Vector4 color = Vector4(1, 1, 1, 1));
    };

    class Cube : public Model {
    public:
        Cube(Vector4 color = Vector4(1, 1, 1, 1));
    };

    class Plane {
    public:
        Mesh *m_Mesh;
        Vector4 color;

        Plane(Vector4 color = Vector4(1, 1, 1, 1));

        void Draw(Shader &shader, Camera &camera);

        template<typename T>
        void AddComponent(T component) {
            m_Mesh->Components.push_back(std::any_cast<T>(component));
        }

        template<typename T>
        T GetComponent() {
            for (auto component : m_Mesh->Components) {
                try {
                    if (typeid(T) == typeid(std::any_cast<T>(component))) {
                        return std::any_cast<T>(component);
                    }
                }
                catch (const std::bad_any_cast &e) {}
            }
        }

        template<typename T>
        bool HasComponent() {
            for (auto component : m_Mesh->Components) {
                if (typeid(T) == typeid(std::any_cast<T>(component))) {
                    return true;
                }
            }
            return false;
        }

        template<typename T>
        void UpdateComponent(T component) {
            for (auto &comp : m_Mesh->Components) {
                if (typeid(T) == typeid(std::any_cast<T>(comp))) {
                    comp = component;
                }
            }
        }
    };

    class Cylinder : public Model {
    public:
        Cylinder(Vector4 color = Vector4(1, 1, 1, 1));
    };

    class Sphere : public Model {
    public:
        Sphere(Vector4 color = Vector4(1, 1, 1, 1));
    };

    class Cone : public Model {
    public:
        Cone(Vector4 color = Vector4(1, 1, 1, 1));
    };

    class Torus : public Model {
    public:
        Torus(Vector4 color = Vector4(1, 1, 1, 1));
    };

    class BatchLayer {
    public:
        std::vector<Vertex_Batch> vertices;
        std::vector<unsigned int> indices;
        unsigned int VBO, VAO, IBO;

        BatchLayer(
                std::vector<Vertex_Batch> &vertices,
                std::vector<unsigned int> &indices
        );

        void Draw(Shader &shader, Camera &camera);
    };

    extern std::vector<BatchLayer *> batchLayers;

    namespace Experimental {
        class ComponentEntity {
        public:
            std::string parentID = "NO_PARENT";
            std::string name = "GameObject";
            std::string ID;
            std::string tag = "Untagged";

            entt::entity entity;

            ComponentEntity() = default;

            template<typename T, typename... Args>
            T &AddComponent(Args &&... args) {
                if (!HasComponent<T>() && Scene::m_Registry.valid(entity)) {
                    T &component = Scene::m_Registry.emplace<T>(entity, std::forward<Args>(args)...);

                    auto &comp = GetComponent<T>();
                    comp.entity = entity;
                    comp.ID = ID;
                    comp.Init();

                    return comp;
                }
            }

            template<typename T>
            T &GetComponent() {
                if (HasComponent<T>()) {
                    return Scene::m_Registry.get<T>(entity);
                } else {
                     T comp;
                     return comp;
                }
            }

            template<typename T>
            bool HasComponent() {
                // check if entity is valid
                if (Scene::m_Registry.valid(entity)) {
                    return Scene::m_Registry.has<T>(entity);
                } else {
                    return false;
                }
            }

            template<typename T>
            void RemoveComponent() {
                Scene::m_Registry.remove<T>(entity);
            }
        };

        struct BaseComponent {
            entt::entity entity = entt::null;
            std::string ID = "";
            bool hasGUI = true;

            virtual void Init() {}

            virtual void GUI() {}
        };

        void DrawVec3Control(const std::string &label, Vector3 &values, float resetValue = 0.0f, float columnWidth = 100.0f);
        void DrawVec2Control(const std::string &label, Vector2 &values, float resetValue = 0.0f, float columnWidth = 100.0f);

        struct Transform : public BaseComponent {
            Transform *parentTransform = nullptr;
            glm::mat4 transform = glm::mat4(1.0f);
            glm::vec3 position = glm::vec3(0, 0, 0);
            glm::vec3 rotation = glm::vec3(0, 0, 0);
            glm::vec3 scale = glm::vec3(1, 1, 1);
            // forward
            glm::vec3 forward = glm::vec3(0, 0, 1);

            void GUI() {
                if (ImGui::TreeNode("Transform")) {
//                    ImGui::DragFloat3("Position", &position.x, 0.1f);
//                    ImGui::DragFloat3("Rotation", &rotation.x, 0.1f);
//                    ImGui::DragFloat3("Scale", &scale.x, 0.1f);
//
                    DrawVec3Control("Position", position);
                    rotation = glm::degrees(rotation);
                    DrawVec3Control("Rotation", rotation);
                    rotation = glm::radians(rotation);
                    DrawVec3Control("Scale", scale);

                    ImGui::NewLine();
                    if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<Transform>(entity);
                    }

                    ImGui::TreePop();
                }
            }

            void Update() {
                transform = glm::translate(glm::mat4(1.0f), position) *
                            glm::toMat4(glm::quat(rotation)) *
                            glm::scale(glm::mat4(1.0f), Vector3(scale.x * 0.5, scale.y * 0.5, scale.z * 0.5));
            }

            void LookAt(glm::vec3 target) {
                glm::vec3 direction = glm::normalize(target - position);
                rotation = glm::eulerAngles(glm::quatLookAt(direction, glm::vec3(0, 1, 0)));
            }

            void Translate(glm::vec3 translation) {
                position += translation;
            }

            void Rotate(glm::vec3 rotation) {
                this->rotation += rotation;
            }

            void Scale(glm::vec3 scale) {
                this->scale += scale;
            }
        };

        struct SpriteRenderer : public BaseComponent {
            Mesh *mesh;
            bool noComponent = false;

            SpriteRenderer() {
                std::vector<Vertex> vertices = {
                        Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0),
                               glm::vec2(0.0f, 0.0f)},
                        Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0),
                               glm::vec2(1.0f, 0.0f)},
                        Vertex{glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0),
                               glm::vec2(1.0f, 1.0f)},
                        Vertex{glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0),
                               glm::vec2(0.0f, 1.0f)}
                };

                std::vector<unsigned int> indices = {
                        0, 1, 2,
                        0, 2, 3
                };

                Material material(Vector4(1, 1, 1, 1));
                mesh = new Mesh(vertices, indices, material);
            }

            void GUI() {
                if (ImGui::TreeNode("Sprite Renderer")) {
                    if (ImGui::TreeNode("Texture")) {
                        if (mesh->material.diffuse != nullptr) {
                            ImGui::ImageButton((void *) mesh->material.diffuse->ID, ImVec2(128, 128), ImVec2(0, 1),
                                               ImVec2(1, 0));
                        } else {
                            ImGui::ImageButton((void *) 0, ImVec2(128, 128));
                        }
                        Scene::DropTargetMat(Scene::DRAG_SPRITE, mesh, nullptr);
                        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1) && mesh->material.diffuse != nullptr) {
                            glDeleteTextures(1, &mesh->material.diffuse->ID);
                            delete mesh->material.diffuse;
                            mesh->material.diffuse = nullptr;
                        }

                        ImGui::TreePop();
                    }
                    ImGui::ColorEdit3("Color", &mesh->material.baseColor.x);
                    ImGui::TreePop();
                }
            }

            void Update() {

            }
        };

        struct SpritesheetRenderer : public BaseComponent {
            Mesh *mesh = nullptr;
            Spritesheet *sp = nullptr;
            Vector2 spritesheetSize = Vector2(512, 512);
            Vector2 spriteSize = Vector2(32, 32);
            Vector2 spriteOffset = Vector2(0, 0);
            Material material{Vector4(1, 1, 1, 1)};

            SpritesheetRenderer() {
                Spritesheet sp("", material, spritesheetSize, spriteSize, spriteOffset);

                mesh = sp.m_Mesh;
            }

            void GUI() {
                if (ImGui::TreeNode("Spritesheet Renderer")) {
                    if (ImGui::TreeNode("Texture")) {
                        if (material.diffuse != nullptr) {
                            ImGui::ImageButton((void *) material.diffuse->ID, ImVec2(128, 128), ImVec2(0, 1),
                                               ImVec2(1, 0));
                        } else {
                            ImGui::ImageButton((void *) 0, ImVec2(128, 128));
                        }
                        Scene::DropTargetMat(Scene::DRAG_SPRITE, mesh, nullptr);
                        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1) && material.diffuse != nullptr) {
                            glDeleteTextures(1, &material.diffuse->ID);
                            delete material.diffuse;
                            material.diffuse = nullptr;
                        }

                        ImGui::TreePop();
                    }
                    ImGui::ColorEdit3("Color", &mesh->material.baseColor.x);
                    DrawVec2Control("Sheet Size", spritesheetSize);
                    DrawVec2Control("Sprite Size", spriteSize);
                    DrawVec2Control("Sprite Offset", spriteOffset);

                    if (ImGui::Button("Apply Changes")) {
                        if (sp != nullptr) {
                            delete sp;
                            sp = nullptr;
                        }

                        sp = new Spritesheet("", material, spritesheetSize, spriteSize, spriteOffset);
                        if (mesh != nullptr) {
                            delete mesh;
                            mesh = nullptr;
                        }
                        mesh = sp->m_Mesh;
                    }

                    ImGui::NewLine();
                    if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<SpriteRenderer>(entity);
                    }

                    ImGui::TreePop();
                }
            }

            void Update() {

            }
        };

        class Random {
        public:
            static void Init() {
                s_Engine.seed(std::random_device()());
            }

            static float Float() {
                return (float) s_Distribution(s_Engine) / (float) std::numeric_limits<uint32_t>::max();
            }

        private:
            static std::mt19937 s_Engine;
            static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
        };

        struct ParticleProps {
            Vector3 position;
            Vector3 velocity, velocityVariation;
            Vector4 colorBegin, colorEnd;
            float sizeBegin, sizeEnd, sizeVariation;
            float lifeTime = 1.0f;
        };

        struct ParticleSystem : public BaseComponent {
            struct Particle {
                glm::vec3 position;
                glm::vec3 velocity;
                glm::vec4 color_begin, color_end;
                float rotation = 0;
                float size_begin, size_end;
                float lifeTime = 1;
                float lifeRemaining = 1;
                bool active = false;
            };

            std::vector<Particle> particlePool;
            unsigned int poolIndex = 999;
            unsigned int quadVA = 0;

            ParticleSystem() {
                particlePool.resize(1000);
            }

            void GUI() {
                if (ImGui::TreeNode("Particle System")) {
                    ImGui::NewLine();
                    if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<SpriteRenderer>(entity);
                    }

                    ImGui::TreePop();
                }
            }

            void Update() {
                for (auto &particle : particlePool) {
                    if (particle.active) {
                        // calculate delta time, there is no namespace as Time.
                        float time = 0.016f;
                        particle.lifeRemaining -= time;

                        if (particle.lifeRemaining <= 0.0f) {
                            particle.active = false;
                            continue;
                        }

                        particle.position += particle.velocity * time;
                        particle.color_begin.r += (particle.color_end.r - particle.color_begin.r) * time;
                        particle.color_begin.g += (particle.color_end.g - particle.color_begin.g) * time;
                        particle.color_begin.b += (particle.color_end.b - particle.color_begin.b) * time;
                        particle.color_begin.a += (particle.color_end.a - particle.color_begin.a) * time;
                        particle.size_begin += (particle.size_end - particle.size_begin) * time;
                        particle.rotation += 0.01f * time;
                    }
                }
            }

            void Emit(const ParticleProps &props) {
                Particle &particle = particlePool[poolIndex];
                particle.active = true;
                particle.position = props.position;
                particle.rotation = Random::Float() * 2.0f * glm::pi<float>();

                particle.velocity = props.velocity;
                particle.velocity.x += props.velocityVariation.x * (Random::Float() - 0.5f);
                particle.velocity.y += props.velocityVariation.y * (Random::Float() - 0.5f);

                particle.color_begin = props.colorBegin;
                particle.color_end = props.colorEnd;

                particle.size_begin = props.sizeBegin;
                particle.size_end = props.sizeEnd;
                particle.size_begin += props.sizeVariation * (Random::Float() - 0.5f);
                particle.size_end += props.sizeVariation * (Random::Float() - 0.5f);

                particle.lifeRemaining = props.lifeTime;
                particle.lifeTime = props.lifeTime;

                poolIndex = --poolIndex % particlePool.size();
            }

            void Draw(Shader &shader, Camera *camera) {
                if (!quadVA) {
                    float vertices[] = {
                            -0.5, -0.5, 0,
                            0.5, -0.5, 0,
                            0.5, 0.5, 0,
                            -0.5, 0.5, 0
                    };

                    unsigned int quadVB;
                    glGenVertexArrays(1, &quadVA);
                    glGenBuffers(1, &quadVB);
                    glBindVertexArray(quadVA);
                    glBindBuffer(GL_ARRAY_BUFFER, quadVB);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void *) 0);

                    unsigned int indices[] = {
                            0, 1, 2,
                            2, 3, 0
                    };

                    unsigned int quadIB;
                    glGenBuffers(1, &quadIB);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIB);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

                    glBindVertexArray(0);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                }

                shader.Bind();
                shader.SetUniformMat4("u_ViewProjection", camera->view);

                for (auto &particle : particlePool) {
                    if (!particle.active) {
                        continue;
                    }
                    float life = particle.lifeRemaining / particle.lifeTime;
                }
            };
        };

        struct m_AnimationData {
            char name[499] = "anim_name";
            std::string id = uuid::generate_uuid_v4();
            std::vector<SpriteRenderer> frames;
            float delay = 0.1f;
            float delay_counter = 0.0f;
            bool loop = false;
        };

        struct m_SpritesheetAnimationData {
            char name[499] = "anim_name";
            std::string id = uuid::generate_uuid_v4();
            std::vector<SpritesheetRenderer> frames;
            float delay = 0.1f;
            float delay_counter = 0.0f;
            bool loop = false;
        };

        struct MeshRenderer : public BaseComponent {
            Mesh *m_Mesh = nullptr;
            bool m_Model = false;

            glm::mat4 extraMatrix = glm::mat4(1.0f);
            std::string matPath = "";
            std::string meshType = "";

            MeshRenderer() = default;

            void GUI() {
                if (ImGui::TreeNode("Material")) {
                    if (!m_Model) {
                        // mesh selection
                        if (ImGui::TreeNode("Mesh")) {
                            ImVec2 windowSize = ImGui::GetWindowSize();
                            if (ImGui::Button("Select Mesh")) {
                                ImGui::OpenPopup("Select Mesh");
                            }

                            if (ImGui::BeginPopup("Select Mesh")) {
                                ImVec2 windowSize = ImGui::GetWindowSize();

                                if (ImGui::Button("Plane", ImVec2(200, 0))) {
                                    meshType = "Plane";
                                    m_Mesh = Plane(Vector4(1, 1, 1, 1)).m_Mesh;
                                    ImGui::CloseCurrentPopup();
                                }

                                if (ImGui::Button("Cube", ImVec2(200, 0))) {
                                    meshType = "Cube";
                                    m_Mesh = Cube(Vector4(1, 1, 1, 1)).meshes[0];
                                    ImGui::CloseCurrentPopup();
                                }

                                if (ImGui::Button("Sphere", ImVec2(200, 0))) {
                                    meshType = "Sphere";
                                    m_Mesh = Sphere(Vector4(1, 1, 1, 1)).meshes[0];
                                    ImGui::CloseCurrentPopup();
                                }

                                if (ImGui::Button("Cone", ImVec2(200, 0))) {
                                    meshType = "Cone";
                                    m_Mesh = Cone(Vector4(1, 1, 1, 1)).meshes[0];
                                    ImGui::CloseCurrentPopup();
                                }

                                if (ImGui::Button("Capsule", ImVec2(200, 0))) {
                                    meshType = "Capsule";
                                    m_Mesh = Capsule(Vector4(1, 1, 1, 1)).meshes[0];
                                    ImGui::CloseCurrentPopup();
                                }

                                if (ImGui::Button("Torus", ImVec2(200, 0))) {
                                    meshType = "Torus";
                                    m_Mesh = Torus(Vector4(1, 1, 1, 1)).meshes[0];
                                    ImGui::CloseCurrentPopup();
                                }

                                if (ImGui::Button("Cylinder", ImVec2(200, 0))) {
                                    meshType = "Cylinder";
                                    m_Mesh = Cylinder(Vector4(1, 1, 1, 1)).meshes[0];
                                    ImGui::CloseCurrentPopup();
                                }

                                ImGui::EndPopup();
                            }

                            ImGui::TreePop();
                        }
                    }

                    if (m_Mesh != nullptr) {
                        if (ImGui::Button("Select Material")) {
                            ImGuiFileDialog::Instance()->OpenDialog("SelectMaterial", "Select Material", ".material",
                                                                    ".");
                        }
                    }

                    if (matPath != "") {
                        ImGui::Text("Material: %s", matPath.c_str());
                        if (ImGui::Button("Remove Material")) {
                            matPath = "";
                            if (m_Mesh->material.diffuse != nullptr) {
                                delete m_Mesh->material.diffuse;
                                m_Mesh->material.diffuse = nullptr;
                            }

                            if (m_Mesh->material.specular != nullptr) {
                                delete m_Mesh->material.specular;
                                m_Mesh->material.specular = nullptr;
                            }

                            if (m_Mesh->material.normal != nullptr) {
                                delete m_Mesh->material.normal;
                                m_Mesh->material.normal = nullptr;
                            }

                            m_Mesh->material.roughness = 0.0f;
                            m_Mesh->material.metallic = 0.0f;
                            m_Mesh->material.texUVs = Vector2(0, 0);
                        }
                    }

                    if (ImGuiFileDialog::Instance()->Display("SelectMaterial")) {
                        // action if OK
                        if (ImGuiFileDialog::Instance()->IsOk()) {
                            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                            // remove cwd from filePathName
                            filePathName.erase(0, cwd.length() + 1);
                            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                            std::ifstream file(filePathName);
                            nlohmann::json JSON = nlohmann::json::parse(file);

                            const std::string diffuseTexture = JSON["diffuse"];
                            const std::string specularTexture = JSON["specular"];
                            const std::string normalTexture = JSON["normal"];

                            if (diffuseTexture != "nullptr") {
                                if (m_Mesh->material.diffuse != nullptr) {
                                    delete m_Mesh->material.diffuse;
                                }

                                m_Mesh->material.diffuse = new Texture(diffuseTexture.c_str(), 0, "texture_diffuse");
                            }

                            if (specularTexture != "nullptr") {
                                if (m_Mesh->material.specular != nullptr) {
                                    delete m_Mesh->material.specular;
                                }

                                m_Mesh->material.specular = new Texture(specularTexture.c_str(), 1, "texture_specular");
                            }

                            if (normalTexture != "nullptr") {
                                if (m_Mesh->material.normal != nullptr) {
                                    delete m_Mesh->material.normal;
                                }

                                m_Mesh->material.normal = new Texture(normalTexture.c_str(), 2, "texture_normal");
                            }

                            m_Mesh->material.baseColor = Vector4(
                                    JSON["baseColor"]["r"],
                                    JSON["baseColor"]["g"],
                                    JSON["baseColor"]["b"],
                                    JSON["baseColor"]["a"]
                            );

                            m_Mesh->material.roughness = JSON["roughness"];
                            m_Mesh->material.metallic = JSON["metallic"];
                            m_Mesh->material.texUVs = Vector2(JSON["texUV"]["x"], JSON["texUV"]["y"]);

                            matPath = filePathName;
                            file.close();
                        }


                        // close
                        ImGuiFileDialog::Instance()->Close();
                    }

                    ImGui::NewLine();
                    if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        if (m_Mesh != nullptr) {
                            if (m_Mesh->material.diffuse != nullptr) {
                                delete m_Mesh->material.diffuse;
                            }

                            if (m_Mesh->material.specular != nullptr) {
                                delete m_Mesh->material.specular;
                            }

                            if (m_Mesh->material.normal != nullptr) {
                                delete m_Mesh->material.normal;
                            }

                            delete m_Mesh;
                            m_Mesh = nullptr;
                        }
                        Scene::m_Registry.remove<MeshRenderer>(entity);
                    }

                    ImGui::TreePop();
                }
            }
        };

        struct c_PointLight : public BaseComponent {
            glm::vec3 lightPos = glm::vec3(0, 0, 0);
            glm::vec3 color = glm::vec3(1, 1, 1);
            float intensity = 1.0f;

            PointLight *light = new PointLight(Scene::PointLights, lightPos, color, intensity);

            c_PointLight() {
                // PointLights.push_back(this);
            }

            void GUI() {

                if (ImGui::TreeNode("Point Light")) {
                    ImGui::ColorEdit3("Color", &color.x, 0);
                    ImGui::DragFloat("Intensity", &intensity, 0.01f);

                    ImGui::NewLine();
                    if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::PointLights.erase(
                                std::remove(Scene::PointLights.begin(), Scene::PointLights.end(), light),
                                Scene::PointLights.end());
                        delete light;
                        Scene::m_Registry.remove<c_PointLight>(entity);
                    }

                    ImGui::TreePop();
                }

            }

            void Update() {
                auto &transform = Scene::m_Registry.get<Transform>(entity);
                lightPos = transform.position;

                light->lightPos = lightPos;
                light->color = color;
                light->intensity = intensity;
            }
        };

        struct c_Light2D : public BaseComponent {
            glm::vec3 lightPos = glm::vec3(0, 0, 0);
            glm::vec3 color = glm::vec3(1, 1, 1);
            float range = 1.0f;

            Light2D *light = new Light2D(Scene::Lights2D, lightPos, Vector4(color, 1.0f), range);

            c_Light2D() = default;

            void GUI() {

                if (ImGui::TreeNode("2D Light")) {
                    ImGui::ColorEdit3("Color", &color.x, 0.01f);
                    ImGui::DragFloat("Range", &range, 0.01f);

                    ImGui::NewLine();
                    if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::Lights2D.erase(std::remove(Scene::Lights2D.begin(), Scene::Lights2D.end(), light),
                                              Scene::Lights2D.end());
                        delete light;
                        Scene::m_Registry.remove<c_Light2D>(entity);
                    }

                    ImGui::TreePop();
                }

            }

            void Update() {
                auto &transform = Scene::m_Registry.get<Transform>(entity);
                lightPos = transform.position;

                light->lightPos = lightPos;
                light->color = color;
                light->range = range;
            }
        };

        struct c_SpotLight : public BaseComponent {
            glm::vec3 lightPos = glm::vec3(0, 0, 0);
            glm::vec3 color = glm::vec3(1, 1, 1);
            float outerCone;
            float innerCone;
            Vector3 angle = Vector3(0.0f, -1.0f, 0.0f);
            SpotLight *light = new SpotLight(Scene::SpotLights, lightPos, color);

            void GUI() {
                if (ImGui::TreeNode("Spot Light")) {
                    ImGui::ColorEdit3("Color", &color.x, 0.01f);

                    ImGui::NewLine();
                    if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::SpotLights.erase(std::remove(Scene::SpotLights.begin(), Scene::SpotLights.end(), light),
                                                Scene::SpotLights.end());
                        delete light;
                        Scene::m_Registry.remove<c_SpotLight>(entity);
                    }
                    ImGui::TreePop();
                }
            }

            void Update() {
                auto &transform = Scene::m_Registry.get<Transform>(entity);
                lightPos = transform.position;
                angle = transform.rotation;

                light->lightPos = lightPos;
                light->color = color;
                light->angle = angle;
            }
        };

        struct c_DirectionalLight : public BaseComponent {
            glm::vec3 lightPos = glm::vec3(0, 0, 0);
            glm::vec3 color = glm::vec3(1, 1, 1);
            float intensity = 1;
            DirectionalLight *light = new DirectionalLight(Scene::DirLights, lightPos, color);

            void GUI() {
                auto &transform = Scene::m_Registry.get<Transform>(entity);
                lightPos = transform.position;

                if (ImGui::TreeNode("Directional Light")) {
                    ImGui::ColorEdit3("Color", &color.x, 0.01f);
                    ImGui::DragFloat("Intensity", &intensity, 0.01f);

                    ImGui::NewLine();
                    if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::DirLights.erase(std::remove(Scene::DirLights.begin(), Scene::DirLights.end(), light),
                                               Scene::DirLights.end());
                        delete light;
                        Scene::m_Registry.remove<c_DirectionalLight>(entity);
                    }
                    ImGui::TreePop();
                }

                light->lightPos = lightPos;
                light->color = color;
            }

            void Update() {
                auto &transform = Scene::m_Registry.get<Transform>(entity);
                lightPos = transform.position;

                light->lightPos = lightPos;
                light->color = color;
            }
        };

        struct Rigidbody2D : public BaseComponent {
            b2BodyType type = b2_staticBody;
            bool fixedRotation = false;
            float gravityScale = 1.0f;
            void *body = nullptr;

            void GUI() {
                if (ImGui::TreeNode("Rigidbody 2D")) {
                    ImGui::Text("Type");
                    ImGui::RadioButton("Static", (int *) &type, 0);
                    ImGui::RadioButton("Dynamic", (int *) &type, 1);
                    ImGui::RadioButton("Kinematic", (int *) &type, 2);
                    ImGui::Checkbox("Fixed Rotation", &fixedRotation);
                    ImGui::DragFloat("Gravity Scale", &gravityScale, 0.01f);

                    if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<Rigidbody2D>(entity);
                    }

                    ImGui::TreePop();
                }
            }

            void SetVelocity(float x, float y) {
                if (body == nullptr) return;
                b2Body *b = (b2Body *) body;
                b->SetLinearVelocity(b2Vec2(x, y));
            }

            void SetAngularVelocity(float velocity) {
                if (body == nullptr) return;
                b2Body *b = (b2Body *) body;
                b->SetAngularVelocity(velocity);
            }

            void SetPosition(float x, float y) {
                if (body == nullptr) return;
                b2Body *b = (b2Body *) body;
                b->SetTransform(b2Vec2(x, y), b->GetAngle());
            }

            void Force(float x, float y) {
                if (body == nullptr) return;
                b2Body *b = (b2Body *) body;
                b->ApplyForceToCenter(b2Vec2(x, y), true);
            }

            void Torque(float torque) {
                if (body == nullptr) return;
                b2Body *b = (b2Body *) body;
                b->ApplyTorque(torque, true);
            }
        };

        struct BoxCollider2D : public BaseComponent {
            Vector2 offset = Vector2(0, 0);
            Vector2 size = Vector2(0.5, 0.5);

            float density = 1.0f;
            float friction = 0.5f;
            float restitution = 0.0f;
            float restitutionThreshold = 0.5f;
            bool trigger = false;

            void *fixture = nullptr;

            BoxCollider2D() = default;

            void GUI() {
                if (ImGui::TreeNode("Box Collider 2D")) {
                    DrawVec2Control("Offset", offset);
                    DrawVec2Control("Size", size);

                    ImGui::Checkbox("Trigger", &trigger);
                    ImGui::DragFloat("Density", &density, 0.01f);
                    ImGui::DragFloat("Friction", &friction, 0.01f);
                    ImGui::DragFloat("Restitution", &restitution, 0.01f);
                    ImGui::DragFloat("Restitution Threshold", &restitutionThreshold, 0.01f);

                    ImGui::NewLine();
                    if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<BoxCollider2D>(entity);
                    }
                    ImGui::TreePop();
                }
            }
        };

        class GameObject : public ComponentEntity {
        public:
            bool keyDown = false;
            std::string NODE_ID = uuid::generate_uuid_v4();
            std::string layer = "Default";
            char layerData[32] = "Default";

            GameObject() {
                entity = Scene::m_Registry.create();
                ID = uuid::generate_uuid_v4();
            }

            void Update() {
                for (auto &childObject : Scene::m_GameObjects) {
                    if (childObject->parentID == ID) {
                        auto &transform = GetComponent<Transform>();
                        auto &childTransform = childObject->GetComponent<Transform>();

                        childTransform.parentTransform = &transform;
                    }
                }
            }

            void GUI() {
                bool item = ImGui::TreeNode(NODE_ID.c_str(), name.c_str());
                if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered() && !ImGui::IsMouseDragging(0)) {
                    Scene::m_Object = this;
                    strncpy(Scene::name, Scene::m_Object->name.c_str(), 499);
                    Scene::name[499] = '\0';

                    strncpy(Scene::tag, Scene::m_Object->tag.c_str(), 499);
                    Scene::tag[499] = '\0';

                    strncpy(Scene::layer, Scene::m_Object->layer.c_str(), 32);
                    Scene::layer[31] = '\0';
                }

                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                    dirPayloadData = ID;
                    ImGui::SetDragDropPayload("game_object", &dirPayloadData, dirPayloadData.size());
                    ImGui::Text(name.c_str());
                    ImGui::EndDragDropSource();
                }

                if (Input::IsKeyPressed(KEY_DELETE) && Scene::m_Object == this && !keyDown) {
                    Scene::m_Object = nullptr;
                    keyDown = true;

                    if (HasComponent<c_DirectionalLight>()) {
                        Scene::DirLights.erase(std::remove(Scene::DirLights.begin(), Scene::DirLights.end(),
                                                           GetComponent<c_DirectionalLight>().light),
                                               Scene::DirLights.end());
                        delete GetComponent<c_DirectionalLight>().light;
                    }

                    if (HasComponent<c_PointLight>()) {
                        Scene::PointLights.erase(std::remove(Scene::PointLights.begin(), Scene::PointLights.end(),
                                                             GetComponent<c_PointLight>().light),
                                                 Scene::PointLights.end());
                        delete GetComponent<c_PointLight>().light;
                    }

                    if (HasComponent<c_SpotLight>()) {
                        Scene::SpotLights.erase(std::remove(Scene::SpotLights.begin(), Scene::SpotLights.end(),
                                                            GetComponent<c_SpotLight>().light),
                                                Scene::SpotLights.end());
                        delete GetComponent<c_SpotLight>().light;
                    }

                    Scene::m_Registry.destroy(entity);
                    Scene::m_GameObjects.erase(
                            std::remove(Scene::m_GameObjects.begin(), Scene::m_GameObjects.end(), this),
                            Scene::m_GameObjects.end());
                } else if (!Input::IsKeyPressed(KEY_DELETE)) {
                    keyDown = false;
                }

                if (item) {

                    for (auto &gameObject : Scene::m_GameObjects) {
                        if (gameObject->parentID == ID) {
                            gameObject->GUI();
                        }
                    }

                    ImGui::TreePop();
                }

            }
        };

        struct Collider3D : public BaseComponent {
            btCollisionShape *shape = nullptr;
            virtual void CreateShape() {}

            // make Collider3D be shape so for example you can do collider3D->setMargin(0.05f);
            Collider3D() = default;
            Collider3D(const Collider3D&) = default;
            explicit operator const btCollisionShape*() const { return shape; }
        };

        struct BoxCollider3D : Collider3D {
            Vector3 size = Vector3(1, 1, 1);

            void CreateShape() override {
                shape = new btBoxShape(btVector3(size.x / 2, size.y / 2, size.z / 2));
                shape->setMargin(0.05f);
            }

            void GUI() override {
                if(ImGui::TreeNode("Box Collider 3D")) {
                    DrawVec3Control("Size", size);

                    ImGui::NewLine();
                    if(ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<BoxCollider3D>(entity);
                    }
                    ImGui::TreePop();
                }
            }
        };

        struct MeshCollider3D : Collider3D {
            Vector3 size = Vector3(1, 1, 1);

            void CreateShape(MeshRenderer *renderer) {
                if (renderer->m_Mesh) {
                    auto *mesh = renderer->m_Mesh;

                    shape = new btConvexHullShape();
                    for (auto &vertex : mesh->vertices) {
                        ((btConvexHullShape *) shape)->addPoint(btVector3(vertex.position.x, vertex.position.y, vertex.position.z));
                    }
                    // add transform scale to shape
                    shape->setLocalScaling(btVector3(size.x / 2, size.y / 2, size.z / 2));
                    shape->setMargin(0.05f);
                }
            }

            void GUI() override {
                if(ImGui::TreeNode("Mesh Collider 3D")) {
                    DrawVec3Control("Size", size);

                    ImGui::NewLine();
                    if(ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<MeshCollider3D>(entity);
                    }
                    ImGui::TreePop();
                }
            }
        };

        struct Rigidbody3D : public BaseComponent {
            float mass = 1;
            float restitution = 0.0f;
            float friction = 0.5f;
            bool trigger = false;
            bool fixedRotation = false;

            GameObject *gameObject = nullptr;
            Transform *transform;

            btRigidBody *body = nullptr;
            btDefaultMotionState *motionState = nullptr;
            btTransform *bt_transform = nullptr;
            btCollisionShape *ref;

            btVector3 inertia = btVector3(0, 0, 0);

            Rigidbody3D() = default;

            void Init() override {
                for(auto &m_Object : Scene::m_GameObjects) {
                    if(m_Object->ID == ID) {
                        gameObject = m_Object;
                        transform = &gameObject->GetComponent<Transform>();

                        break;
                    }
                }
            }

            void CreateBody(btCollisionShape *shape) {
                ref = shape;
                bt_transform = new btTransform();
                bt_transform->setIdentity();
                bt_transform->setOrigin(btVector3(transform->position.x, transform->position.y, transform->position.z));
                bt_transform->setRotation(btQuaternion(transform->rotation.x, transform->rotation.y, transform->rotation.z,
                                                       1));

                motionState = new btDefaultMotionState(*bt_transform);

                if(mass > 0) {
                    shape->calculateLocalInertia(mass, inertia);
                }

                btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
                rbInfo.m_restitution = restitution;
                rbInfo.m_friction = friction;
                body = new btRigidBody(rbInfo);
                // set user data
                body->setUserPointer(&gameObject->ID);
                // set trigger
                body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
                if(trigger) {
                    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
                }

                if(fixedRotation) {
                    body->setAngularFactor(btVector3(0, 0, 0));
                }

                BulletPhysicsWorld::dynamicsWorld->addRigidBody(body);
            }

            void DeleteBody() const {
                BulletPhysicsWorld::dynamicsWorld->removeRigidBody(body);
                delete body;
                delete motionState;
                delete bt_transform;
            }

            void Update() {
                btTransform btTrans = body->getWorldTransform();
                glm::mat4 mat = glm::mat4(1.0f);
                btTrans.getOpenGLMatrix(&mat[0][0]);

                // decompose
                glm::vec3 pos, rot, scal;
                DecomposeTransform(mat, pos, rot, scal);
                transform->position = pos;
                transform->rotation = rot;
            }

            void GUI() override {
                if (ImGui::TreeNode("Rigidbody 3D")) {
                    ImGui::DragFloat("Mass", &mass, 0.01f);
                    ImGui::DragFloat("Friction", &friction, 0.01f);
                    ImGui::DragFloat("Restitution", &restitution, 0.01f);
                    ImGui::Checkbox("Trigger", &trigger);
                    ImGui::Checkbox("Fixed Rotation", &fixedRotation);

                    ImGui::NewLine();
                    if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<Rigidbody3D>(entity);
                    }
                    ImGui::TreePop();
                }
            }

            // Rigidbody functions
            void AddForce(const Vector3 &force) {
                body->applyCentralForce(btVector3(force.x, force.y, force.z));
            }

            void AddTorque(const Vector3 &torque) {
                body->applyTorque(btVector3(torque.x, torque.y, torque.z));
            }

            void AddForceAtPosition(const Vector3 &force, const Vector3 &position) {
                body->applyForce(btVector3(force.x, force.y, force.z), btVector3(position.x, position.y, position.z));
            }

            // velocity functions
            void SetVelocity(const Vector3 &velocity) {
                body->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
            }

            void SetAngularVelocity(const Vector3 &velocity) {
                body->setAngularVelocity(btVector3(velocity.x, velocity.y, velocity.z));
            }

            Vector3 GetVelocity() {
                auto velocity = body->getLinearVelocity();
                return Vector3(velocity.x(), velocity.y(), velocity.z());
            }

            Vector3 GetAngularVelocity() {
                auto velocity = body->getAngularVelocity();
                return Vector3(velocity.x(), velocity.y(), velocity.z());
            }

            // make rigidbody move like a fps controller
            void Move(const Vector3 &direction, float speed) {
                auto velocity = body->getLinearVelocity();
                body->setLinearVelocity(btVector3(direction.x * speed, velocity.y(), direction.z * speed));
            }
        };

        struct FixedJoint3D : public BaseComponent {
            GameObject *gameObject = nullptr;
            GameObject *connectedGameObject = nullptr;
            Transform *transform;

            btRigidBody *body = nullptr;
            btRigidBody *body2 = nullptr;
            btTypedConstraint *joint = nullptr;

            FixedJoint3D() = default;

            void Init() override {
                for(auto &m_Object : Scene::m_GameObjects) {
                    if(m_Object->ID == ID) {
                        gameObject = m_Object;
                        transform = &gameObject->GetComponent<Transform>();

                        break;
                    }
                }
            }

            void CreateJoint() {
                if(!connectedGameObject) return;

                body = gameObject->GetComponent<Rigidbody3D>().body;
                body2 = connectedGameObject->GetComponent<Rigidbody3D>().body;

                btTransform frameInA = btTransform::getIdentity();
                btTransform frameInB = btTransform::getIdentity();

                joint = new btFixedConstraint(*body, *body2, frameInA, frameInB);
                BulletPhysicsWorld::dynamicsWorld->addConstraint(joint);
            }

            void DeleteJoint() const {
                if(joint) {
                    BulletPhysicsWorld::dynamicsWorld->removeConstraint(joint);
                    delete joint;
                }
            }

            void GUI() override {
                if (ImGui::TreeNode("Fixed Joint 3D")) {
                    if(!connectedGameObject) {
                        ImGui::Selectable("Drag and drop another object with Rigidbody3D to connect");
                    } else {
                        ImGui::Selectable(std::string("Connected with: " + connectedGameObject->name).c_str());
                    }

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("game_object")) {
                            for(auto &m_Object : Scene::m_GameObjects) {
                                if(m_Object->ID == *((std::string*)payload->Data) && m_Object->HasComponent<Rigidbody3D>()) {
                                    connectedGameObject = m_Object;
                                    break;
                                }
                            }
                        }
                    }

                    ImGui::NewLine();
                    if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<FixedJoint3D>(entity);
                    }
                    ImGui::TreePop();
                }
            }
        };

        struct SpriteAnimation : public BaseComponent {
            Mesh *currMesh;
            std::vector<m_AnimationData> anims;
            char currAnim[499] = "";

            SpriteAnimation() {
                currMesh = nullptr;
                for (auto &gameObject : Scene::m_GameObjects) {
                    if (gameObject->ID == ID) {
                        std::map<std::string, int> m_CurrFrames;
                        Scene::currFrames[gameObject->ID] = m_CurrFrames;

                        std::map<std::string, float> m_CurrDelays;
                        Scene::currDelays[gameObject->ID] = m_CurrDelays;
                    }
                }
            }

            void GUI() {
                if (ImGui::TreeNode("Sprite Animation")) {
                    ImGui::InputText("Current Animation", currAnim, 499);

                    for (auto &animation : anims) {
                        int index = &animation - &anims[0];

                        if (ImGui::TreeNode(
                                std::string(animation.name + std::string(" ") + std::to_string(index)).c_str())) {
                            ImGui::InputText("Name", animation.name, 499);
                            ImGui::InputFloat("Delay", &animation.delay);
                            ImGui::Checkbox("Loop", &animation.loop);

                            for (auto &frame : animation.frames) {
                                int index = &frame - &animation.frames[0];

                                if (ImGui::TreeNode(std::string("Frame " + std::to_string(index)).c_str())) {
                                    if (frame.mesh->material.diffuse != nullptr) {
                                        ImGui::ImageButton((void *) frame.mesh->material.diffuse->ID, ImVec2(128, 128),
                                                           ImVec2(0, 1), ImVec2(1, 0));
                                    } else {
                                        ImGui::ImageButton((void *) 0, ImVec2(128, 128));
                                    }
                                    Scene::DropTargetMat(Scene::DRAG_SPRITE, frame.mesh, nullptr);
                                    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1) &&
                                        frame.mesh->material.diffuse != nullptr) {
                                        glDeleteTextures(1, &frame.mesh->material.diffuse->ID);
                                        delete frame.mesh->material.diffuse;
                                        frame.mesh->material.diffuse = nullptr;
                                    }
                                    ImGui::ColorEdit3("Color", &frame.mesh->material.baseColor.x);

                                    if (ImGui::Button(ICON_FA_TRASH " Remove Frame")) {
                                        animation.frames.erase(animation.frames.begin() + index);
                                    }

                                    ImGui::TreePop();
                                }
                            }

                            if (ImGui::Button(ICON_FA_PLUS " Add Frame")) {
                                animation.frames.push_back(SpriteRenderer());
                            }

                            ImGui::NewLine();
                            if (ImGui::Button(ICON_FA_TRASH " Remove Animation")) {
                                anims.erase(anims.begin() + index);
                            }
                            ImGui::TreePop();
                        }
                    }

                    if (ImGui::Button(ICON_FA_PLUS " Add Animation")) {
                        m_AnimationData anim;
                        anims.push_back(anim);
                    }

                    ImGui::NewLine();
                    if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<SpriteRenderer>(entity);
                    }

                    ImGui::TreePop();
                }
            }

            void Play() {
                for (auto &animation : anims) {
                    if (std::string(animation.name) == std::string(currAnim)) {
                        std::cout << Scene::currFrames[ID][std::string(animation.name)] << std::endl;
                        // deltaTime stuff
                        Scene::currDelays[ID][std::string(animation.name)] += Timestep::deltaTime;
                        if (Scene::currDelays[ID][std::string(animation.name)] >= animation.delay) {
                            Scene::currFrames[ID][std::string(animation.name)] += 1;
                            if (Scene::currFrames[ID][std::string(animation.name)] >= animation.frames.size()) {
                                if (animation.loop) {
                                    Scene::currFrames[ID][std::string(animation.name)] = 0;
                                } else {
                                    Scene::currFrames[ID][std::string(animation.name)] = animation.frames.size() - 1;
                                }
                            }
                            Scene::currDelays[ID][std::string(animation.name)] = 0.0f;
                        }
                        currMesh = animation.frames[Scene::currFrames[ID][std::string(animation.name)]].mesh;

                        break;
                    }
                }
            }
        };

        std::vector<m_SpritesheetAnimationData>
        GetAnimationsFromXML(const char *texPath, float delay, Vector2 sheetSize, const std::string &xmlFile);

        struct c_SpritesheetAnimation : public BaseComponent {
            Mesh *currMesh;
            std::vector<m_SpritesheetAnimationData> anims;
            char currAnim[499] = "";
            Texture *texture = new Texture("assets/PONGON.png", 0, "texture_diffuse");
            Vector2 sheetSize;

            c_SpritesheetAnimation() {
                currMesh = nullptr;
                for (auto &gameObject : Scene::m_GameObjects) {
                    if (gameObject->ID == ID) {
                        std::map<std::string, int> m_CurrFrames;
                        Scene::currFrames[gameObject->ID] = m_CurrFrames;

                        std::map<std::string, float> m_CurrDelays;
                        Scene::currDelays[gameObject->ID] = m_CurrDelays;
                    }
                }
            }

            void GUI() override {
                if (ImGui::TreeNode("Spritesheet Animation")) {
                    ImGui::InputText("Current Animation", currAnim, 499);

                    if (ImGui::TreeNode("Texture")) {
                        if (texture != nullptr) {
                            ImGui::ImageButton((void *) texture->ID, ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
                        } else {
                            ImGui::ImageButton((void *) 0, ImVec2(128, 128));
                        }
                        Scene::DropTargetMat(Scene::DRAG_SPRITE_NO_MESH, nullptr, texture);
                        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1) && texture != nullptr) {
                            glDeleteTextures(1, &texture->ID);
                            delete texture;
                            texture = nullptr;
                        }

                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("XML Data")) {
                        DrawVec2Control("Sheet Size", sheetSize);

                        if (ImGui::Button(ICON_FA_FILE " Select XML")) {
                            ImGuiFileDialog::Instance()->OpenDialog("SelectXML", "Select XML", ".xml", ".");
                        }

                        ImGui::TreePop();
                    }

                    ImGui::NewLine();
                    if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<SpriteRenderer>(entity);
                    }

                    ImGui::TreePop();
                }

                if (ImGuiFileDialog::Instance()->Display("SelectXML")) {
                    // action if OK
                    if (ImGuiFileDialog::Instance()->IsOk()) {
                        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                        // remove cwd from filePathName
                        filePathName.erase(0, cwd.length() + 1);

                        anims = GetAnimationsFromXML(texture->texPath.c_str(), 0.1f, sheetSize, filePathName);

                        for (auto &animation : anims) {
                            std::string name = animation.name;
                            // delete last 4 characters
                            name.erase(name.end() - 4, name.end());

                            Scene::currFrames[ID][name] = 0;
                            Scene::currDelays[ID][name] = animation.delay;
                        }
                    }

                    ImGuiFileDialog::Instance()->Close();
                }
            }

            void Play() {
                for (auto &animation : anims) {
                    std::string name = animation.name;
                    // delete last 4 characters
                    name.erase(name.end() - 4, name.end());
                    if (name == std::string(currAnim)) {
                        std::cout << Scene::currFrames[ID][name] << std::endl;
                        // deltaTime stuff
                        Scene::currDelays[ID][name] += Timestep::deltaTime;
                        if (Scene::currDelays[ID][name] >= animation.delay) {
                            Scene::currFrames[ID][name] += 1;
                            if (Scene::currFrames[ID][name] >= animation.frames.size()) {
                                if (animation.loop) {
                                    Scene::currFrames[ID][name] = 0;
                                } else {
                                    Scene::currFrames[ID][name] = animation.frames.size() - 1;
                                }
                            }
                            Scene::currDelays[ID][name] = 0.0f;
                        }
                        currMesh = animation.frames[Scene::currFrames[ID][name]].mesh;

                        break;
                    }
                }
            }
        };

        struct NativeScriptManager : public BaseComponent {
            std::vector<StaticScript *> m_StaticScripts;
            GameObject *gameObject;

            NativeScriptManager() = default;

            void Init() {
                for (auto &gameObject : Scene::m_GameObjects) {
                    if (gameObject->ID == ID) {
                        this->gameObject = gameObject;
                        break;
                    }
                }
            }

            template<typename T>
            void AddScript() {
                m_StaticScripts.push_back(new T());
                m_StaticScripts.back()->gameObject = gameObject;
                HYPER_LOG(std::string("Added script: ") + typeid(T).name());
            }

            void Start() {
                for (auto script : m_StaticScripts) {
                    script->OnStart();
                }
            }

            void Update() {
                for (auto script : m_StaticScripts) {
                    script->OnUpdate();
                }
            }
        };

        struct CameraComponent : public BaseComponent {
            Camera *camera = nullptr;
            GameObject *m_GameObject = nullptr;

            CameraComponent() = default;

            ~CameraComponent() {
                Scene::cameras.erase(std::remove(Scene::cameras.begin(), Scene::cameras.end(), camera),
                                     Scene::cameras.end());
            }

            void Init() {
                auto &transform = Scene::m_Registry.get<Experimental::Transform>(entity);
                transform.rotation = glm::vec3(0.0f, 0.0f, -1.0f);

                camera = new Camera(false, 1280, 720, glm::vec3(0, 0, 0), entity);
                Scene::cameras.push_back(camera);

                for (auto &gameObject : Scene::m_GameObjects) {
                    if (gameObject->ID == ID) {
                        m_GameObject = gameObject;
                        break;
                    }
                }
            }

            void GUI() {
                if (ImGui::TreeNode("Camera")) {
                    if (camera != nullptr) {
                        ImGui::DragFloat("FOV", &camera->cam_fov, 0.01f);
                        ImGui::DragFloat("Near", &camera->cam_near, 0.01f);
                        ImGui::DragFloat("Far", &camera->cam_far, 0.01f);

                        ImGui::Checkbox("Main Camera", &camera->mainCamera);
                        ImGui::Checkbox("2D Mode", &camera->mode2D);

                        if (Scene::mainCamera == camera) {
                            if (ImGui::Button(ICON_FA_CAMERA " Unselect as Scene Camera")) {
                                Scene::mainCamera = nullptr;
                            }
                        } else {
                            if (ImGui::Button(ICON_FA_CAMERA " Select as Scene Camera")) {
                                Scene::mainCamera = camera;
                            }
                        }
                    }

                    ImGui::NewLine();

                    ImVec2 winSize = ImGui::GetWindowSize();
                    if (ImGui::Button(ICON_FA_TRASH " Delete", ImVec2(winSize.x, 0))) {
                        // delete camera;
                        // camera = nullptr;
                        // Scene::cameras.erase(std::remove(Scene::cameras.begin(), Scene::cameras.end(), camera), Scene::cameras.end());
                        m_GameObject->RemoveComponent<CameraComponent>();
                    }
                    ImGui::TreePop();
                }
            }
        };

        struct m_LuaScriptComponent : public BaseComponent {
            GameObject *m_GameObject;
            std::vector<ScriptEngine::m_LuaScript> scripts;

            m_LuaScriptComponent() {
                for (auto &gameObject : Scene::m_GameObjects) {
                    if (gameObject->ID == ID) {
                        m_GameObject = gameObject;
                        std::cout << "FOUND" << std::endl;
                        break;
                    }
                }
            };

            void Start() {
                for (auto &script : scripts) {
                    script.Init();
                }
            }

            void Update() {
                if (isRunning) {
                    for (auto &script : scripts) {
                        script.Update();
                    }
                }
            }

            void GUI() {
                if (ImGui::TreeNode("Lua Scripts")) {
                    if (ImGui::TreeNode("Scripts")) {
                        for (int i = 0; i < scripts.size(); i++) {
                            if (ImGui::TreeNode(std::string("Script " + std::to_string(i)).c_str())) {
                                ImGui::Text("Path: %s", scripts[i].pathToScript.c_str());
                                if (ImGui::Button(ICON_FA_TRASH " Delete")) {
                                    scripts.erase(scripts.begin() + i);
                                }
                                ImGui::TreePop();
                            }
                        }

                        ImGui::TreePop();
                    }

                    if (ImGui::Button(ICON_FA_PLUS " Add Script")) {
                        ImGuiFileDialog::Instance()->OpenDialog("ChooseScript", "Choose Script", ".lua", ".");
                    }

                    if (ImGuiFileDialog::Instance()->Display("ChooseScript")) {
                        // action if OK
                        if (ImGuiFileDialog::Instance()->IsOk()) {
                            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                            // remove cwd from filePathName
                            filePathName.erase(0, cwd.length() + 1);
                            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                            ScriptEngine::m_LuaScript script(filePathName);
                            script.m_GameObject = m_GameObject;
                            script.ID = ID;
                            // script.Init();
                            scripts.push_back(script);

                            HYPER_LOG(std::string("Added lua script: ") + filePathName);
                        }

                        ImGuiFileDialog::Instance()->Close();
                    }

                    ImGui::NewLine();
                    if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<m_LuaScriptComponent>(entity);
                    }

                    ImGui::TreePop();
                }
            }
        };

        class Model {
        private:
            int currSlot = 0;
        public:
            GameObject *mainGameObject;
            std::vector<GameObject *> m_gameObjects;
            std::string directory;

            bool texturesEnabled = true;
            Vector4 Color;

            std::string path;

            glm::mat4 transform;

            void loadModel(std::string path);

            void processNode(aiNode *node, const aiScene *scene);

            GameObject *processMesh(aiMesh *mesh, const aiScene *scene, const std::string &name);

            std::vector<Texture> textures_loaded;

            std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                      std::string typeName);

            Model(char *path, bool AddTexture = false, Vector4 color = Vector4(1, 1, 1, 1)) {
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

            void Draw(Shader &shader, Camera &camera);
        };
    }

    class QuadBatch {
    public:
        BatchLayer *layer;
        int vert1, vert2, vert3, vert4;
        Experimental::Transform transform;

        [[maybe_unused]] QuadBatch(BatchLayer *layer, std::vector<Vertex_Batch> &vertices,
                                   std::vector<unsigned int> &indices);

        void Update();
    };

    namespace f_GameObject {
        Experimental::GameObject *FindGameObjectByName(const std::string &name);
        Experimental::GameObject *FindGameObjectByTag(const std::string &tag);
        Experimental::GameObject *FindGameObjectByID(const std::string &id);
        Experimental::GameObject *InstantiatePrefab(const std::string &path);

        Experimental::GameObject *
        InstantiateTransformPrefab(const std::string &path, Vector3 position = Vector3(0, 0, 0),
                                   Vector3 rotation = Vector3(0, 0, 0));
    };
}

extern float rectangleVert[];

void NewFrame(unsigned int FBO, int width, int height);

void EndFrame(HyperAPI::Shader &framebufferShader, HyperAPI::Renderer &renderer, unsigned int FBO, unsigned int rectVAO,
              unsigned int postProcessingTexture, unsigned int postProcessingFBO, const int width, const int height);

void EndEndFrame(
        HyperAPI::Shader &framebufferShader,
        HyperAPI::Renderer &renderer,
        unsigned int FBO,
        unsigned int rectVAO,
        unsigned int postProcessingTexture,
        unsigned int postProcessingFBO,
        unsigned int S_FBO,
        unsigned int S_postProcessingTexture,
        unsigned int S_postProcessingFBO,
        const int width,
        const int height
);

void
SC_EndFrame(HyperAPI::Renderer &renderer, unsigned int FBO, unsigned int rectVAO, unsigned int postProcessingTexture,
            unsigned int postProcessingFBO, const int width, const int height);

namespace Hyper {
    class Application {
    public:
        int sceneMouseX, sceneMouseY;

        bool renderOnScreen = false;
        int winWidth, winHeight;
        int width;
        int height;

        std::string vendor, srenderer, version;

        const std::string title;
        std::function<void()> m_update;

        HyperAPI::Renderer *renderer;

        Application(const int width, const int height, const char *gameTitle, bool fullscreen = false,
                    bool resizable = true, bool wireframe = false)
                : width(width), height(height), title(std::string(gameTitle)) {
            HYPER_LOG("Initializing Static Engine");
            renderer = new HyperAPI::Renderer(width, height, title.c_str(), {0, -1}, 8, fullscreen, resizable,
                                              wireframe);
            HYPER_LOG("Initialized Static Engine");

            // HYPER_LOG("Initializing Audio Engine");
            // HyperAPI::AudioEngine::Init();
            // HYPER_LOG("Initialized Audio Engine");

            //get vendor 
            vendor = (char *) glGetString(GL_VENDOR);
            srenderer = (char *) glGetString(GL_RENDERER);
            version = (char *) glGetString(GL_VERSION);

            ImGui::CreateContext();
            ImGuiIO &io = ImGui::GetIO();

            io.ConfigWindowsMoveFromTitleBarOnly = true;

            // io.Fonts->AddFontDefault();
            ImGui::StyleColorsDark();

            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

            //enable so that windows can be their own windows
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 2.0f);

            ImGui_ImplGlfw_InitForOpenGL(renderer->window, true);
            ImGui_ImplOpenGL3_Init("#version 330");
            io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-Bold.ttf", 18.f);
            static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
            ImFontConfig icons_config;
            icons_config.MergeMode = true;
            icons_config.PixelSnapH = true;
            io.Fonts->AddFontFromFileTTF("assets/fonts/fa-solid-900.ttf", 16.0f, &icons_config, icons_ranges);
            HYPER_LOG("Initialized ImGui");
        }

        void Run(std::function<void()> update,
                 std::function<void(unsigned int &PPT, unsigned int &PPFBO)> gui = [](unsigned int &PPT,
                                                                                      unsigned int &PPFBO) {});
    };

    class MousePicker {
    public:
        Vector3 currentRay;
        glm::mat4 projectionMatrix;
        HyperAPI::Camera *camera;
        Application *appRef;

        int winX, winY;
        int mouseX, mouseY;

        MousePicker(Application *app, HyperAPI::Camera *camera, glm::mat4 projection);

        Vector3 getCurrentRay();

        void update();

        Vector3 calculateMouseRay();

        Vector2 getNormalizedDeviceCoords(float mouseX, float mouseY);

        Vector4 toEyeCoords(Vector4 clipCoords);

        Vector3 toWorldCoords(Vector4 eyeCoords);
    };

    float LerpFloat(float a, float b, float t);
}

extern std::vector<HyperAPI::PointLight *> PointLights;
extern std::vector<HyperAPI::Light2D *> Lights2D;
extern std::vector<HyperAPI::SpotLight *> SpotLights;
extern std::vector<HyperAPI::DirectionalLight *> DirLights;
extern std::vector<HyperAPI::Mesh *> hyperEntities;