#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Mesh.hpp"
#include "../Renderer/Structures.hpp"
#include "Bloom.hpp"

namespace HyperAPI::Experimental {
    struct DLL_API SpriteRenderer : public BaseComponent {
        Mesh *mesh;
        bool noComponent = false;

        struct DLL_API CustomShader {
            bool usingCustomShader = false;
            Shader *shader = nullptr;
        } customShader;

        SpriteRenderer() {
            std::vector<Vertex> vertices = {
                Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1, 1, 1),
                       glm::vec3(0, 1, 0), glm::vec2(0.0f, 0.0f)},
                Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1, 1, 1),
                       glm::vec3(0, 1, 0), glm::vec2(1.0f, 0.0f)},
                Vertex{glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1, 1, 1),
                       glm::vec3(0, 1, 0), glm::vec2(1.0f, 1.0f)},
                Vertex{glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1, 1, 1),
                       glm::vec3(0, 1, 0), glm::vec2(0.0f, 1.0f)}};

            std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

            Material material(Vector4(1, 1, 1, 1));
            // new Mesh to shared_ptr
            mesh = new Mesh(vertices, indices, material);
        }
        void DeleteComp() override {
            if (mesh) {
                delete mesh;
            }
        }

        void GUI() override;

        void Update() {
            if (Scene::m_Registry.has<Bloom>(entity)) {
                auto &bloom = Scene::m_Registry.get<Bloom>(entity);
                mesh->material.bloomColor = bloom.bloomColor;
                mesh->material.bloom_threshold = bloom.bloom_threshold;
                mesh->material.dynamic_bloom = bloom.dynamic_bloom;
            } else {
                mesh->material.bloomColor = Vector3(0, 0, 0);
                mesh->material.bloom_threshold = 0;
                mesh->material.dynamic_bloom = false;
            }
        }
    };

    struct DLL_API m_AnimationData {
        char name[499] = "anim_name";
        std::string id = uuid::generate_uuid_v4();
        std::vector<SpriteRenderer> frames;
        float delay = 0.1f;
        float delay_counter = 0.0f;
        bool loop = false;

        m_AnimationData() = default;
    };
} // namespace HyperAPI::Experimental