#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#define MAX_BONE_INFLUENCE 4
#define MAX_BONE_WEIGHTS MAX_BONE_INFLUENCE

namespace HyperAPI {
    struct DLL_API BoneInfo {
        int id;
        glm::mat4 offset;
    };

    struct DLL_API Vertex {
        glm::vec3 position;
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec2 texUV = glm::vec2(0.0f, 0.0f);
        int m_BoneIDs[MAX_BONE_INFLUENCE] = {-1};
        float m_Weights[MAX_BONE_INFLUENCE] = {0.0f};
        glm::vec3 tangent = glm::vec3(-435.0f, -435.0f, -435.0f);
        glm::vec3 bitangent = glm::vec3(-435.0f, -435.0f, -435.0f);
        float transformIndex = 0;
    };

    struct DLL_API Vertex_Batch {
        glm::vec3 position;
        glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec2 texUV = glm::vec2(0.0f, 0.0f);

        int diffuse = -1;
        int specular = -1;
        int normalMap = -1;

        float metallic = 0;
        float roughness = 0;

        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        Vector2 texUVs = Vector2(0, 0);

        glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

        // float texID = -1;
        // glm::mat4 model = glm::mat4(1.0f);
    };

    struct DLL_API PointLight : public ComponentSystem {
        glm::vec3 lightPos;
        glm::vec3 color;
        float intensity;

        PointLight(std::vector<HyperAPI::PointLight *> &lights,
                   glm::vec3 lightPos, glm::vec3 color, float intensity) {
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

    struct DLL_API SpotLight : public ComponentSystem {
        glm::vec3 lightPos;
        glm::vec3 color;
        float outerCone;
        float innerCone;
        Vector3 angle = Vector3(0.0f, -1.0f, 0.0f);

        SpotLight(std::vector<HyperAPI::SpotLight *> &lights,
                  glm::vec3 lightPos, glm::vec3 color, float outerCone = 0.9,
                  float innerCone = 0.95) {
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

    struct DLL_API DirectionalLight : public ComponentSystem {
        glm::vec3 lightPos;
        glm::vec3 color;
        float intensity = 1;

        DirectionalLight(std::vector<HyperAPI::DirectionalLight *> &lights,
                         glm::vec3 lightPos, glm::vec3 color) {
            this->lightPos = lightPos;
            this->color = color;

            TransformComponent transform;
            transform.position = lightPos;
            AddComponent(transform);

            lights.push_back(this);
            // scriptComponent.componentSystem = this;
        }
    };

    struct DLL_API Light2D : public ComponentSystem {
        glm::vec2 lightPos;
        glm::vec3 color;
        float range;

        Light2D(std::vector<Light2D *> &lights, Vector2 lightPos, Vector4 color,
                float range) {
            this->lightPos = lightPos;
            this->color = color;
            this->range = range;

            TransformComponent transform;
            AddComponent(transform);

            lights.push_back(this);
            // scriptComponent.componentSystem = this;
        }
    };

    struct DLL_API Instanced {
        bool isInstanced = false;
        std::vector<TransformComponent> transforms = {};
        int count = 1;

        Instanced(bool instanced, int count = 1,
                  std::vector<TransformComponent> transforms = {}) {
            isInstanced = instanced;
            this->count = count;
            this->transforms = transforms;
        }
    };
} // namespace HyperAPI