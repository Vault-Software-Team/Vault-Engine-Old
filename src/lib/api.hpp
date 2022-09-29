#pragma once
#include "../vendor/glad/include/glad/glad.h"
#include "../vendor/GLFW/glfw3.h"
#include <fstream>
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../vendor/stb_image/stb_image.h"
// #include "../vendor/stb_image/stb_image.h"
#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../vendor/glm/gtc/type_ptr.hpp"
#include "../vendor/glm/ext.hpp"
#include "../vendor/glm/gtx/rotate_vector.hpp"
#include "../vendor/glm/gtx/vector_angle.hpp"
#include "../vendor/json/json.hpp"
#include "../vendor/imgui/imgui.h"
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
#include <random>

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
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"
#define RESET "\033[0m"
#define HYPER_LOG(x) std::cout << RED "[STATIC] - " RESET << x << std::endl;

using json = nlohmann::json;

extern   glm::mat4 projection;
extern   glm::mat4 view;

namespace uuid {
    extern   std::random_device              rd;
    extern   std::mt19937                    gen;
    extern   std::uniform_int_distribution<> dis;
    extern   std::uniform_int_distribution<> dis2;

    std::string generate_uuid_v4();
}

namespace HyperAPI {
    extern std::string cwd;
    extern std::string dirPayloadData;
    extern bool isRunning;
    extern bool isStopped;

    class AssimpGLMHelpers
    {
    public:

        static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
        {
            glm::mat4 to;
            //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
            to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
            to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
            to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
            to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
            return to;
        }

        static inline glm::vec3 GetGLMVec(const aiVector3D& vec) 
        { 
            return glm::vec3(vec.x, vec.y, vec.z); 
        }

        static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
        {
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

        ComponentSystem() {}

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
                catch (const std::bad_any_cast& e) {}
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
            for (auto& comp : Components) {
                if (typeid(T) == typeid(std::any_cast<T>(comp))) {
                    comp = component;
                }
            }
        }
    };

    struct KeyPosition
    {
        glm::vec3 position;
        float timeStamp;
    };

    struct KeyRotation
    {
        glm::quat orientation;
        float timeStamp;
    };

    struct KeyScale
    {
        glm::vec3 scale;
        float timeStamp;
    };

    struct TransformComponent : public Component {
        glm::mat4 transform = glm::mat4(1.0f);
        glm::vec3 position = glm::vec3(0,0,0);
        glm::vec3 rotation = glm::vec3(0,0,0);
        glm::vec3 scale = glm::vec3(1,1,1);

        void GUI() {
            ImGui::DragFloat3("Position", &position.x, 0.1f);
            ImGui::DragFloat3("Rotation", &rotation.x, 0.1f);
            ImGui::DragFloat3("Scale", &scale.x, 0.1f);
        }

        void LookAt(glm::vec3 target) {
            glm::vec3 direction = glm::normalize(target - position);
            rotation = glm::degrees(glm::eulerAngles(glm::quatLookAt(direction, glm::vec3(0,1,0))));
        }
    };

    struct Character {
        unsigned int TextureID;  // ID handle of the glyph texture
        glm::ivec2   Size;       // Size of glyph
        glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
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
            switch (type)
            {
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
        // glm::vec3 Position;
        // glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        // glm::vec3 RotationValue = glm::vec3(0.0f, 0.0f, 0.0f);
        bool EnttComp = false;
        entt::entity entity = entt::null;

        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

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

        void updateMatrix(float FOVdeg, float nearPlane, float farPlane, Vector2 winSize);
        void Matrix(Shader& shader, const char* uniform);
        void Inputs(GLFWwindow* window, Vector2 winPos);
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
        int m_BoneIDs[MAX_BONE_INFLUENCE] = { -1 };
        float m_Weights[MAX_BONE_INFLUENCE] = { 0.0f };
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

        PointLight(std::vector<HyperAPI::PointLight*> &lights, glm::vec3 lightPos, glm::vec3 color, float intensity) {
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

        SpotLight(std::vector<HyperAPI::SpotLight*> &lights, glm::vec3 lightPos, glm::vec3 color, float outerCone = 0.9, float innerCone = 0.95) {
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

        DirectionalLight(std::vector<HyperAPI::DirectionalLight*> &lights, glm::vec3 lightPos, glm::vec3 color) {
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
        Light2D(std::vector<Light2D*> &lights, Vector2 lightPos, Vector4 color, float range) {
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

        Material(Vector4 baseColor = Vector4(1,1,1,1), std::vector<Texture> textures = {}, float shininess = 0, float metallic = 0, float roughness = 0);
        ~Material() {
            if(diffuse != nullptr) {
                delete diffuse;
            }

            if(specular != nullptr) {
                delete specular;
            }

            if(normal != nullptr) {
                delete normal;
            }
        }

        void Bind(Shader &shader);
        void Unbind(Shader &shader);
    };

    class Mesh : public ComponentSystem {
    public:
        std::string parentType = "None";
        Material material{Vector4(1,1,1,1)};

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

        Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, Material &material, bool empty = false, bool batched = false);

        void Draw(
            Shader &shader,
            Camera &camera,
            glm::mat4 matrix = glm::mat4(1.0f),
            glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
            glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
        );  
    };

    class Model : public ComponentSystem 
    {
    private:
        int currSlot = 0;
    public:
        std::map<std::string, BoneInfo> m_BoneInfoMap;
        int m_BoneCounter = 0;

        auto& GetBoneInfoMap() { return m_BoneInfoMap; }
        int& GetBoneCounter() { return m_BoneCounter; }

        void SetVertexBoneDataToDefault(Vertex &vertex) {
            for(int i = 0; i < MAX_BONE_INFLUENCE; i++) {
                vertex.m_BoneIDs[i] = -1;
                vertex.m_Weights[i] = 0.0f;
            }
        }

        void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
        {
            for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
            {
                if (vertex.m_BoneIDs[i] < 0)
                {
                    vertex.m_Weights[i] = weight;
                    vertex.m_BoneIDs[i] = boneID;
                    break;
                }
            }
        }

        void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
        {
            for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
            {
                int boneID = -1;
                std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
                if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
                {
                    BoneInfo newBoneInfo;
                    newBoneInfo.id = m_BoneCounter;
                    newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(
                        mesh->mBones[boneIndex]->mOffsetMatrix);
                    m_BoneInfoMap[boneName] = newBoneInfo;
                    boneID = m_BoneCounter;
                    m_BoneCounter++;
                }
                else
                {
                    boneID = m_BoneInfoMap[boneName].id;
                }
                assert(boneID != -1);
                auto weights = mesh->mBones[boneIndex]->mWeights;
                int numWeights = mesh->mBones[boneIndex]->mNumWeights;

                for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
                {
                    int vertexId = weights[weightIndex].mVertexId;
                    float weight = weights[weightIndex].mWeight;
                    assert(vertexId <= vertices.size());
                    SetVertexBoneData(vertices[vertexId], boneID, weight);
                }
            }
        }

        std::vector<Mesh*> meshes;
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

        Model(char *path, bool AddTexture = false, Vector4 color = Vector4(1,1,1, 1))
        {
            this->path = std::string(path);
            Color = color;
            this->name = "Model";
            texturesEnabled = AddTexture;
            TransformComponent transform;
            AddComponent(transform);
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
        Skybox(const std::string &right, const std::string &left,  const std::string &top, const std::string &bottom, const std::string &front, const std::string &back);
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

        Renderer(int width, int height, const char *title, Vector2 g_gravity, unsigned int samples = 8, bool wireframe = false);

        void Render(Camera &camera);
        void Swap(HyperAPI::Shader &framebufferShader, unsigned int FBO, unsigned int rectVAO, unsigned int postProcessingTexture, unsigned int postProcessingFBO);
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
        std::vector<Animation*> animations;
        Animation *currentAnimation = nullptr;

        float time = 0.1;
        float prevTime = 0;

        SpritesheetAnimation() {};

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

            if(currentAnimation != nullptr && currentAnimation->frames.size() > 0) {
                currentAnimation->frames[currentAnimation->currentFrame].Draw(shader, camera);
                if(time <= 0) {
                    currentAnimation->currentFrame++;
                    if (currentAnimation->currentFrame >= currentAnimation->frames.size() - 1) {
                        currentAnimation->currentFrame = 0;
                    }
                    
                    time = currentAnimation->delay;
                }
            }
        }
    };

    std::vector<HyperAPI::Animation> GetAnimationsFromXML(const char *texPath, float delay, Vector2 sheetSize, const std::string &xmlFile);

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
        Capsule(Vector4 color = Vector4(1,1,1, 1));
    };

    class Cube : public Model {
    public:
        Cube(Vector4 color = Vector4(1,1,1, 1));
    };

    class Plane {
    public:
        Mesh *m_Mesh;
        Vector4 color;

        Plane(Vector4 color = Vector4(1,1,1, 1));
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
                catch (const std::bad_any_cast& e) {}
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
            for (auto& comp : m_Mesh->Components) {
                if (typeid(T) == typeid(std::any_cast<T>(comp))) {
                    comp = component;
                }
            }
        }	
    };

    class Cylinder : public Model {
    public:
        Cylinder(Vector4 color = Vector4(1,1,1, 1));
    };

    class Sphere : public Model {
    public:
        Sphere(Vector4 color = Vector4(1,1,1, 1));
    };

    class Cone : public Model {
    public:
        Cone(Vector4 color = Vector4(1,1,1, 1));
    };

    class Torus : public Model {
    public:
        Torus(Vector4 color = Vector4(1,1,1, 1));
    };

    class SpriteShader : public Shader {
    public:
        unsigned int ID;

        SpriteShader();
        virtual void Bind();
        virtual void Unbind();

        // set uniforms
        virtual void SetUniform1i(const char *name, int value);
        virtual void SetUniform1f(const char *name, float value);
        virtual void SetUniform2f(const char *name, float v0, float v1);
        virtual void SetUniform3f(const char *name, float v0, float v1, float v2);
        virtual void SetUniform4f(const char *name, float v0, float v1, float v2, float v3);
        virtual void SetUniformMat4(const char *name, glm::mat4 value);
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

    extern std::vector<BatchLayer*> batchLayers;

    namespace Experimental {
        class ComponentEntity {
        public:
            std::string parentID = "NO_PARENT";
            std::string name = "GameObject";
            std::string ID;
            std::string tag = "Untagged";

            entt::entity entity;
            ComponentEntity() {};

            template<typename T, typename... Args>
            T& AddComponent(Args&&... args) {
                if(!HasComponent<T>()) {
                    T& component = Scene::m_Registry.emplace<T>(entity, std::forward<Args>(args)...);

                    auto &comp = GetComponent<T>();
                    comp.entity = entity;
                    comp.ID = ID;
                    comp.Init();
                    
                    return comp;
                }
            }

            template<typename T>
            T& GetComponent() {
                if(HasComponent<T>()) {
                    return Scene::m_Registry.get<T>(entity);
                } else {
                    T& component = AddComponent<T>();
                    component.hasGUI = false;
                    return component;
                }
            }

            template<typename T>
            bool HasComponent() {
                return Scene::m_Registry.has<T>(entity);
            }

            template<typename T>
            void RemoveComponent() {
                Scene::m_Registry.remove<T>(entity);
            }
        };

        struct BaseComponent {
            entt::entity entity;
            std::string ID;
            bool hasGUI = true;

            virtual void Init() {}
            virtual void GUI() {}
        };

        struct Transform : public BaseComponent {
            Transform *parentTransform = nullptr;
            glm::mat4 transform = glm::mat4(1.0f);
            glm::vec3 position = glm::vec3(0,0,0);
            glm::vec3 rotation = glm::vec3(0,0,0);
            glm::vec3 scale = glm::vec3(1,1,1);

            void GUI() {
                if(ImGui::TreeNode("Transform")) {
                    ImGui::DragFloat3("Position", &position.x, 0.1f);
                    ImGui::DragFloat3("Rotation", &rotation.x, 0.1f);
                    ImGui::DragFloat3("Scale", &scale.x, 0.1f);
                    
                    ImGui::NewLine();
                    if(ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<Transform>(entity);
                    }
                    
                    ImGui::TreePop();
                }
            }

            void Update() {
                transform = glm::translate(glm::mat4(1.0f), position) *
                glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
                glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
                glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
                glm::scale(glm::mat4(1.0f), Vector3(scale.x * 0.5, scale.y * 0.5, scale.z * 0.5));
            }

            void LookAt(glm::vec3 target) {
                glm::vec3 direction = glm::normalize(target - position);
                rotation = glm::degrees(glm::eulerAngles(glm::quatLookAt(direction, glm::vec3(0,1,0))));
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
                    Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1,1,1), glm::vec3(0, 1, 0), glm::vec2(0.0f, 0.0f)},
                    Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1,1,1), glm::vec3(0, 1, 0), glm::vec2(1.0f, 0.0f)},
                    Vertex{glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1,1,1), glm::vec3(0, 1, 0), glm::vec2(1.0f, 1.0f)},
                    Vertex{glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1,1,1), glm::vec3(0, 1, 0), glm::vec2(0.0f, 1.0f)}
                };

                std::vector<unsigned int> indices = {
                    0, 1, 2,
                    0, 2, 3
                };

                Material material(Vector4(1,1,1,1));
                mesh = new Mesh(vertices, indices, material);
            }

            void GUI() {
                if(ImGui::TreeNode("Sprite Renderer")) {
                    if(ImGui::TreeNode("Texture")) {
                        if(mesh->material.diffuse != nullptr) {
                            HYPER_LOG("Changed diffuse texture");
                            ImGui::ImageButton((void*)mesh->material.diffuse->ID, ImVec2(128,128), ImVec2(0,1), ImVec2(1,0));
                        } else {
                            ImGui::ImageButton((void*)0, ImVec2(128,128));
                        }
                        Scene::DropTargetMat(Scene::DRAG_SPRITE, mesh);
                        if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(1) && mesh->material.diffuse != nullptr) {
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
            Vector2 spritesheetSize = Vector2(512,512);
            Vector2 spriteSize = Vector2(32,32);
            Vector2 spriteOffset = Vector2(0,0);
            Material material{Vector4(1,1,1,1)};

            SpritesheetRenderer() {
                Spritesheet sp("", material, spritesheetSize, spriteSize, spriteOffset);

                mesh = sp.m_Mesh;
            }

            void GUI() {
                if(ImGui::TreeNode("Spritesheet Renderer")) {
                    if(ImGui::TreeNode("Texture")) {
                        if(material.diffuse != nullptr) {
                            HYPER_LOG("Changed diffuse texture");
                            ImGui::ImageButton((void*)material.diffuse->ID, ImVec2(128,128), ImVec2(0,1), ImVec2(1,0));
                        } else {
                            ImGui::ImageButton((void*)0, ImVec2(128,128));
                        }
                        Scene::DropTargetMat(Scene::DRAG_SPRITE, mesh);
                        if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(1) && material.diffuse != nullptr) {
                            glDeleteTextures(1, &material.diffuse->ID);
                            delete material.diffuse;
                            material.diffuse = nullptr;
                        }

                        ImGui::TreePop();
                    }
                    ImGui::ColorEdit3("Color", &mesh->material.baseColor.x);
                    ImGui::DragFloat2("Spritesheet Size", &spritesheetSize.x, 1.0f, 1.0f);
                    ImGui::DragFloat2("Sprite Size", &spriteSize.x, 1.0f, 1.0f);
                    ImGui::DragFloat2("Sprite Offset", &spriteOffset.x, 1.0f, 0.0f);

                    if(ImGui::Button("Apply Changes")) {
                        if(sp != nullptr) {
                            delete sp;
                            sp = nullptr;
                        }

                        sp = new Spritesheet("", material, spritesheetSize, spriteSize, spriteOffset);
                        if(mesh != nullptr) {
                            delete mesh;
                            mesh = nullptr;
                        }
                        mesh = sp->m_Mesh;
                    }

                    ImGui::NewLine();
                    if(ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<SpriteRenderer>(entity);
                    }
                    
                    ImGui::TreePop();
                }
            }

            void Update() {

            }
        };

        struct m_AnimationData {
            char name[499] = "anim_name";
            std::string id = uuid::generate_uuid_v4();
            std::vector<SpriteRenderer> frames;
            float delay = 0.1f;
            bool loop = false;
        };

        struct SpriteAnimation : public BaseComponent {
            Mesh *currMesh;
            std::vector<m_AnimationData> anims;
            char currAnim[499] = "anim_name";
            float prevTime, currTime, currDelay = 0;
            int currFrame;

            SpriteAnimation() {
                currMesh = nullptr;
            }

            void GUI() {
                if(ImGui::TreeNode("Sprite Animation")) {
                    ImGui::InputText("Current Animation", currAnim, 499);
                    if(ImGui::TreeNode("Animations")) {
                        for(int i = 0; i < anims.size(); i++) {
                            if(ImGui::TreeNode(anims[i].id.c_str(), anims[i].name)) {  
                                ImGui::InputText("Name", anims[i].name, 499);
                                ImGui::DragFloat("Delay", &anims[i].delay, 0.01f, 0.01f);
                                ImGui::Checkbox("Loop", &anims[i].loop);
                                for(int m = 0; m < anims[i].frames.size(); m++) {
                                    SpriteRenderer &sr = anims[i].frames[m];
                                    if(ImGui::TreeNode(std::string("Frame " + std::to_string(m)).c_str())) {
                                        if(ImGui::TreeNode("Texture")) {
                                            if(sr.mesh->material.diffuse != nullptr) {
                                                ImGui::ImageButton((void*)sr.mesh->material.diffuse->ID, ImVec2(128,128), ImVec2(0,1), ImVec2(1,0));
                                            } else {
                                                ImGui::ImageButton((void*)0, ImVec2(128,128));
                                            }
                                            Scene::DropTargetMat(Scene::DRAG_SPRITE, sr.mesh);
                                            if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(1) && sr.mesh->material.diffuse != nullptr) {
                                                glDeleteTextures(1, &sr.mesh->material.diffuse->ID);
                                                delete sr.mesh->material.diffuse;
                                                sr.mesh->material.diffuse = nullptr;
                                            }

                                            ImGui::TreePop();
                                        }
                                        ImGui::ColorEdit3("Color", &sr.mesh->material.baseColor.x);                    
                                        ImGui::TreePop();
                                    }
                                }

                                if(ImGui::Button("Add Frame")) {
                                    anims[i].frames.push_back(SpriteRenderer());
                                }

                                ImGui::TreePop();
                            }
                        }

                        if(ImGui::Button("Add Animation")) {
                            anims.push_back(m_AnimationData());
                        }
                        ImGui::TreePop();
                    }

                    ImGui::NewLine();
                    if(ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<SpriteAnimation>(entity);
                    }
                    
                    ImGui::TreePop();
                }
            }

            void Play() {
                currTime = glfwGetTime();
                float deltaTime = currTime - prevTime;
                prevTime = currTime;

                for(int i = 0; i < anims.size(); i++) {
                    if(std::string(anims[i].name) == std::string(currAnim) && anims[i].frames.size() > 0) {
                        m_AnimationData &anim = anims[i];
                        //delay   
                        currDelay += deltaTime;
                        if(currDelay >= anim.delay) {
                            currDelay = 0;

                            //frame
                            currFrame++;
                            if(anim.frames.size() <= 0) return;
                            if(currFrame >= anim.frames.size()) {
                                if(anim.loop) {
                                    currFrame = 0;
                                } else {
                                    currFrame = anim.frames.size() - 1;
                                }
                            }

                            currMesh = anim.frames[currFrame].mesh;
                            std::cout << currFrame << std::endl;
                        }
                    }
                }
            }
        };

        struct MeshRenderer : public BaseComponent {
            Mesh *m_Mesh = nullptr;
            bool m_Model = false;

            glm::mat4 extraMatrix = glm::mat4(1.0f);
            std::string matPath = "";
            std::string meshType = "";

            MeshRenderer() {}
            void GUI() {
                if(ImGui::TreeNode("Material")) {
                    if(!m_Model) {
                        // mesh selection
                        if(ImGui::TreeNode("Mesh")) {
                            ImVec2 windowSize = ImGui::GetWindowSize();
                            if(ImGui::Button("Select Mesh")) {
                                ImGui::OpenPopup("Select Mesh");
                            }

                            if(ImGui::BeginPopup("Select Mesh")) {
                                ImVec2 windowSize = ImGui::GetWindowSize();

                                if(ImGui::Button("Plane", ImVec2(200, 0))) {
                                    meshType = "Plane";
                                    m_Mesh = Plane(Vector4(1,1,1,1)).m_Mesh;
                                    ImGui::CloseCurrentPopup();
                                }

                                if(ImGui::Button("Cube", ImVec2(200, 0))) {
                                    meshType = "Cube";
                                    m_Mesh = Cube(Vector4(1,1,1,1)).meshes[0];
                                    ImGui::CloseCurrentPopup();
                                }

                                if(ImGui::Button("Sphere", ImVec2(200, 0))) {
                                    meshType = "Sphere";
                                    m_Mesh = Sphere(Vector4(1,1,1,1)).meshes[0];
                                    ImGui::CloseCurrentPopup();
                                }

                                if(ImGui::Button("Cone", ImVec2(200, 0))) {
                                    meshType = "Cone";
                                    m_Mesh = Cone(Vector4(1,1,1,1)).meshes[0];
                                    ImGui::CloseCurrentPopup();
                                }

                                if(ImGui::Button("Capsule", ImVec2(200, 0))) {
                                    meshType = "Capsule";
                                    m_Mesh = Capsule(Vector4(1,1,1,1)).meshes[0];
                                    ImGui::CloseCurrentPopup();
                                }

                                if(ImGui::Button("Torus", ImVec2(200, 0))) {
                                    meshType = "Torus";
                                    m_Mesh = Torus(Vector4(1,1,1,1)).meshes[0];
                                    ImGui::CloseCurrentPopup();
                                }

                                if(ImGui::Button("Cylinder", ImVec2(200, 0))) {
                                    meshType = "Cylinder";
                                    m_Mesh = Cylinder(Vector4(1,1,1,1)).meshes[0];
                                    ImGui::CloseCurrentPopup();
                                }

                                ImGui::EndPopup();
                            }

                            ImGui::TreePop();
                        }
                    }

                    if(m_Mesh != nullptr) {
                        if(ImGui::Button("Select Material")) {
                            ImGuiFileDialog::Instance()->OpenDialog("SelectMaterial", "Select Material", ".material", ".");
                        }
                    }

                    if(matPath != "") {
                        ImGui::Text("Material: %s", matPath.c_str());
                        if(ImGui::Button("Remove Material")) {
                            matPath = "";
                            if(m_Mesh->material.diffuse != nullptr) {
                                delete m_Mesh->material.diffuse;
                                m_Mesh->material.diffuse = nullptr;
                            }

                            if(m_Mesh->material.specular != nullptr) {
                                delete m_Mesh->material.specular;
                                m_Mesh->material.specular = nullptr;
                            }

                            if(m_Mesh->material.normal != nullptr) {
                                delete m_Mesh->material.normal;
                                m_Mesh->material.normal = nullptr;
                            }

                            m_Mesh->material.roughness = 0.0f;
                            m_Mesh->material.metallic = 0.0f;
                            m_Mesh->material.texUVs = Vector2(0, 0);
                        }
                    }

                    if (ImGuiFileDialog::Instance()->Display("SelectMaterial")) 
                    {
                        // action if OK
                        if (ImGuiFileDialog::Instance()->IsOk())
                        {
                            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                            // remove cwd from filePathName
                            filePathName.erase(0, cwd.length() + 1);
                            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                            
                            std::ifstream file(filePathName);
                            nlohmann::json JSON = nlohmann::json::parse(file);

                            const std::string diffuseTexture = JSON["diffuse"];
                            const std::string specularTexture = JSON["specular"];
                            const std::string normalTexture = JSON["normal"];
                        
                            if(diffuseTexture != "nullptr") {
                                if(m_Mesh->material.diffuse != nullptr) {
                                    delete m_Mesh->material.diffuse;
                                }

                                m_Mesh->material.diffuse = new Texture(diffuseTexture.c_str(), 0, "texture_diffuse");
                            }

                            if(specularTexture != "nullptr") {
                                if(m_Mesh->material.specular != nullptr) {
                                    delete m_Mesh->material.specular;
                                }

                                m_Mesh->material.specular = new Texture(specularTexture.c_str(), 1, "texture_specular");
                            }

                            if(normalTexture != "nullptr") {
                                if(m_Mesh->material.normal != nullptr) {
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
                    if(ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        if(m_Mesh != nullptr) {
                            if(m_Mesh->material.diffuse != nullptr) {
                                delete m_Mesh->material.diffuse;
                            }

                            if(m_Mesh->material.specular != nullptr) {
                                delete m_Mesh->material.specular;
                            }

                            if(m_Mesh->material.normal != nullptr) {
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

                if(ImGui::TreeNode("Point Light")) {
                    ImGui::ColorEdit3("Color", &color.x, 0.01f);
                    ImGui::DragFloat("Intensity", &intensity, 0.01f);

                    ImGui::NewLine();
                    if(ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::PointLights.erase(std::remove(Scene::PointLights.begin(), Scene::PointLights.end(), light), Scene::PointLights.end());
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

        struct c_SpotLight : public BaseComponent {
            glm::vec3 lightPos = glm::vec3(0, 0, 0);
            glm::vec3 color = glm::vec3(1, 1, 1);
            float outerCone;
            float innerCone;
            Vector3 angle = Vector3(0.0f, -1.0f, 0.0f);
            SpotLight *light = new SpotLight(Scene::SpotLights, lightPos, color);

            void GUI() {
                if(ImGui::TreeNode("Spot Light")) {
                    ImGui::ColorEdit3("Color", &color.x, 0.01f);

                    ImGui::NewLine();
                    if(ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::SpotLights.erase(std::remove(Scene::SpotLights.begin(), Scene::SpotLights.end(), light), Scene::SpotLights.end());
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

                if(ImGui::TreeNode("Directional Light")) {
                    ImGui::ColorEdit3("Color", &color.x, 0.01f);
                    ImGui::DragFloat("Intensity", &intensity, 0.01f);

                    ImGui::NewLine();
                    if(ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::DirLights.erase(std::remove(Scene::DirLights.begin(), Scene::DirLights.end(), light), Scene::DirLights.end());
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
                if(ImGui::TreeNode("Rigidbody2D")) {
                    ImGui::Text("Type");
                    ImGui::RadioButton("Static", (int *)&type, 0);
                    ImGui::RadioButton("Dynamic", (int *)&type, 1);
                    ImGui::RadioButton("Kinematic", (int *)&type, 2);
                    ImGui::Checkbox("Fixed Rotation", &fixedRotation);
                    ImGui::DragFloat("Gravity Scale", &gravityScale, 0.01f);

                    ImGui::TreePop();
                }
            }

            void SetVelocity(float x, float y) {
                if(body == nullptr) return;
                b2Body *b = (b2Body*)body;
                b->SetLinearVelocity(b2Vec2(x, y));
            }

            void SetAngularVelocity(float velocity) {
                if(body == nullptr) return;
                b2Body *b = (b2Body*)body;
                b->SetAngularVelocity(velocity);
            }

            void SetPosition(float x, float y) {
                if(body == nullptr) return;
                b2Body *b = (b2Body*)body;
                b->SetTransform(b2Vec2(x, y), b->GetAngle());
            }

            void Force(float x, float y) {
                if(body == nullptr) return;
                b2Body *b = (b2Body*)body;
                b->ApplyForceToCenter(b2Vec2(x, y), true);
            }

            void Torque(float torque) {
                if(body == nullptr) return;
                b2Body *b = (b2Body*)body;
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

            void *fixture = nullptr;

            void GUI() {
                if(ImGui::TreeNode("Box Collider 2D")) {
                    ImGui::DragFloat2("Offset", &offset.x, 0.01f);
                    ImGui::DragFloat2("Size", &size.x, 0.01f);

                    ImGui::DragFloat("Density", &density, 0.01f);
                    ImGui::DragFloat("Friction", &friction, 0.01f);
                    ImGui::DragFloat("Restitution", &restitution, 0.01f);
                    ImGui::DragFloat("Restitution Threshold", &restitutionThreshold, 0.01f);

                    ImGui::NewLine();
                    if(ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<BoxCollider2D>(entity);
                    }
                    ImGui::TreePop();
                }
            }
        };

        class GameObject : public ComponentEntity {
        public:
            std::string NODE_ID = uuid::generate_uuid_v4();
            std::string layer = "Default";
            char layerData[32] = "Default";
            GameObject() {
                entity = Scene::m_Registry.create();
                ID = uuid::generate_uuid_v4();

                Scene::m_GameObjects.push_back(this);
            }

            void Update() {
                for(auto &childObject : Scene::m_GameObjects) {
                    if(childObject->parentID == ID) {
                        Transform &transform = GetComponent<Transform>();
                        Transform &childTransform = childObject->GetComponent<Transform>();

                        childTransform.parentTransform = &transform;
                    }
                }
            }

            void GUI() {
                bool item = ImGui::TreeNode(NODE_ID.c_str(), name.c_str());
                if(ImGui::IsItemClicked(0)) {
                    Scene::m_Object = this;
                    strncpy(Scene::name, Scene::m_Object->name.c_str(), 499);
                    Scene::name[499] = '\0';

                    strncpy(Scene::tag, Scene::m_Object->tag.c_str(), 499);
                    Scene::tag[499] = '\0';

                    strncpy(Scene::layer, Scene::m_Object->layer.c_str(), 32);
                    Scene::layer[31] = '\0';
                }

                if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                    dirPayloadData = ID;
                    ImGui::SetDragDropPayload("file", dirPayloadData.c_str(), strlen(dirPayloadData.c_str()));
                    ImGui::Text(name.c_str());
                    ImGui::EndDragDropSource();
                }

                if(ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Delete)) && Scene::m_Object == this) {
                    Scene::m_Object = nullptr;

                    if(HasComponent<c_DirectionalLight>()) {
                        Scene::DirLights.erase(std::remove(Scene::DirLights.begin(), Scene::DirLights.end(), GetComponent<c_DirectionalLight>().light), Scene::DirLights.end());
                        delete GetComponent<c_DirectionalLight>().light;
                    }

                    if(HasComponent<c_PointLight>()) {
                        Scene::PointLights.erase(std::remove(Scene::PointLights.begin(), Scene::PointLights.end(), GetComponent<c_PointLight>().light), Scene::PointLights.end());
                        delete GetComponent<c_PointLight>().light;
                    }

                    if(HasComponent<c_SpotLight>()) {
                        Scene::SpotLights.erase(std::remove(Scene::SpotLights.begin(), Scene::SpotLights.end(), GetComponent<c_SpotLight>().light), Scene::SpotLights.end());
                        delete GetComponent<c_SpotLight>().light;
                    }

                    Scene::m_Registry.destroy(entity);
                    Scene::m_GameObjects.erase(std::remove(Scene::m_GameObjects.begin(), Scene::m_GameObjects.end(), this), Scene::m_GameObjects.end());
                }

                if(item) {

                    for(auto &gameObject : Scene::m_GameObjects) {
                        if(gameObject->parentID == ID) {
                            gameObject->GUI();
                        }
                    }

                    ImGui::TreePop();
                }

            }
        };

        struct NativeScriptManager : public BaseComponent {
            std::vector<StaticScript*> m_StaticScripts;
            GameObject *gameObject;

            NativeScriptManager() {}

            void Init() {
                for(auto &gameObject : Scene::m_GameObjects) {
                    if(gameObject->ID == ID) {
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
                for(auto script : m_StaticScripts) {
                    script->OnStart();
                }
            }

            void Update() {
                for(auto script : m_StaticScripts) {
                    script->OnUpdate();
                }
            }
        };

        struct CameraComponent : public BaseComponent {
            Camera *camera = nullptr;
            GameObject *m_GameObject = nullptr;

            CameraComponent() {}

            void Init() {
                auto &transform = Scene::m_Registry.get<Experimental::Transform>(entity);
                transform.rotation = glm::vec3(0.0f, 0.0f, -1.0f);

                camera = new Camera(false, 1280, 720, glm::vec3(0,0,0), entity);
                Scene::cameras.push_back(camera);
                
                for(auto &gameObject : Scene::m_GameObjects) {
                    if(gameObject->ID == ID) {
                        m_GameObject = gameObject;
                        break;
                    }
                }
            }

            void GUI() {
                if(ImGui::TreeNode("Camera")) {
                    if(camera != nullptr) {
                        ImGui::DragFloat("FOV", &camera->cam_fov, 0.01f);
                        ImGui::DragFloat("Near", &camera->cam_near, 0.01f);
                        ImGui::DragFloat("Far", &camera->cam_far, 0.01f);

                        ImGui::Checkbox("Main Camera", &camera->mainCamera);
                        ImGui::Checkbox("2D Mode", &camera->mode2D);

                        if(Scene::mainCamera == camera) {
                            if(ImGui::Button(ICON_FA_CAMERA " Unselect as Scene Camera")) {
                                Scene::mainCamera = nullptr;
                            }
                        } else {
                            if(ImGui::Button(ICON_FA_CAMERA " Select as Scene Camera")) {
                                Scene::mainCamera = camera;
                            }
                        }
                    }

                    ImGui::NewLine();

                    ImVec2 winSize = ImGui::GetWindowSize();
                    if(ImGui::Button(ICON_FA_TRASH " Delete", ImVec2(winSize.x, 0))) {
                        delete camera;
                        camera = nullptr;
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
                for(auto &gameObject : Scene::m_GameObjects) {
                    if(gameObject->ID == ID) {
                        m_GameObject = gameObject;
                        std::cout << "FOUND" << std::endl;
                        break;
                    }
                }
            };

            void Start() {
                for(auto &script : scripts) {
                    script.Init();
                }
            }

            void Update() {
                if(isRunning) {
                    for(auto &script : scripts) {
                        script.Update();
                    }
                }
            }

            void GUI() {
                if(ImGui::TreeNode("Lua Scripts")) {
                    if(ImGui::TreeNode("Scripts")) {
                        for(int i = 0; i < scripts.size(); i++) {
                            if(ImGui::TreeNode(std::string("Script " + std::to_string(i)).c_str())) {
                                ImGui::Text("Path: %s", scripts[i].pathToScript.c_str());
                                if(ImGui::Button(ICON_FA_TRASH " Delete")) {
                                    scripts.erase(scripts.begin() + i);
                                }
                                ImGui::TreePop();
                            }
                        }

                        ImGui::TreePop();
                    }

                    if(ImGui::Button(ICON_FA_PLUS " Add Script")) {
                        ImGuiFileDialog::Instance()->OpenDialog("ChooseScript", "Choose Script", ".lua", ".");
                    }

                    if (ImGuiFileDialog::Instance()->Display("ChooseScript")) 
                    {
                        // action if OK
                        if (ImGuiFileDialog::Instance()->IsOk())
                        {
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
                    if(ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                        Scene::m_Registry.remove<m_LuaScriptComponent>(entity);
                    }
                            
                    ImGui::TreePop();
                }
            }
        };

        class Model
        {
        private:
            int currSlot = 0;
        public:
            GameObject *mainGameObject;
            std::vector<GameObject*> m_gameObjects;
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

            Model(char *path, bool AddTexture = false, Vector4 color = Vector4(1,1,1, 1))
            {
                this->path = std::string(path);
                Color = color;

                mainGameObject = new GameObject();
                mainGameObject->name = "Model";
                mainGameObject->ID = uuid::generate_uuid_v4();
                mainGameObject->AddComponent<Transform>();
                // Scene::m_GameObjects.push_back(mainGameObject);

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

        QuadBatch(BatchLayer *layer, std::vector<Vertex_Batch> &vertices, std::vector<unsigned int> &indices);
        void Update();
    };
    
    namespace f_GameObject {
        Experimental::GameObject *FindGameObjectByName(const std::string &name);
        Experimental::GameObject *FindGameObjectByTag(const std::string &tag);
    };

    class FontFace {
    public:
        std::string path;
        int size;
        FT_Face face;
        std::map<char, Character> Characters;

        unsigned int texture;
        unsigned int VAO, VBO;

        FontFace(const char *path, int size);
        void DrawText(Shader &shader, const char *text, float x, float y, float scale, Vector4 color);
    };
}

extern   const int width ;
extern   const int height;
extern   float rectangleVert[];

void   NewFrame(unsigned int FBO, int width, int height);
void   EndFrame(HyperAPI::Shader &framebufferShader, HyperAPI::Renderer &renderer, unsigned int FBO, unsigned int rectVAO, unsigned int postProcessingTexture, unsigned int postProcessingFBO, const int width, const int height);

void   EndEndFrame(
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

void   SC_EndFrame(HyperAPI::Renderer &renderer, unsigned int FBO, unsigned int rectVAO, unsigned int postProcessingTexture, unsigned int postProcessingFBO, const int width, const int height);

namespace Hyper {
    class Application {
    public:
        int sceneMouseX, sceneMouseY;

        bool renderOnScreen = false;
        int width;
        int height;

        std::string vendor, srenderer, version;

        const std::string title;
        std::function<void()> m_update;

        HyperAPI::Renderer *renderer;

        Application(const int width, const int height, const char *gameTitle, bool wireframe = false) 
        : width(width), height(height), title(std::string(gameTitle))  {
            HYPER_LOG("Initializing Static Engine");
            renderer = new HyperAPI::Renderer(width, height, title.c_str(), {0, -1}, 8, wireframe);
            HYPER_LOG("Initialized Static Engine");

            //get vendor 
            vendor = (char*)glGetString(GL_VENDOR);
            srenderer = (char*)glGetString(GL_RENDERER);
            version = (char*)glGetString(GL_VERSION);

            ImGui::CreateContext();
            ImGui_ImplGlfw_InitForOpenGL(renderer->window, true);
            ImGui_ImplOpenGL3_Init("#version 330");
            ImGuiIO& io = ImGui::GetIO();

            io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-Bold.ttf", 18.f); 
            static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
            ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
            io.Fonts->AddFontFromFileTTF("assets/fonts/fa-solid-900.ttf", 16.0f, &icons_config, icons_ranges );

            io.ConfigWindowsMoveFromTitleBarOnly=true;

            io.Fonts->AddFontDefault();
            ImGui::StyleColorsDark();

            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 2.0f);  
            HYPER_LOG("Initialized ImGui");
        }
        void Run(std::function<void()> update, std::function<void(unsigned int &PPT, unsigned int &PPFBO)> gui = [](unsigned int &PPT, unsigned int &PPFBO){});
    };

    float LerpFloat(float a, float b, float t);
}

extern   std::vector<HyperAPI::PointLight*> PointLights;
extern   std::vector<HyperAPI::Light2D*> Lights2D;
extern   std::vector<HyperAPI::SpotLight*> SpotLights;
extern   std::vector<HyperAPI::DirectionalLight*> DirLights;
extern   std::vector<HyperAPI::Mesh*> hyperEntities;