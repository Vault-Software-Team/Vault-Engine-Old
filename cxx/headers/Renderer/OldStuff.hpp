#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Mesh.hpp"
#include "Model.hpp"

namespace HyperAPI {
    class DLL_API Sprite {
    public:
        Mesh *m_Mesh;

        Sprite(const char *texPath);

        template <typename T>
        void AddComponent(T component) {
            m_Mesh->AddComponent(component);
        }

        template <typename T>
        T GetComponent() {
            return m_Mesh->GetComponent<T>();
        }

        template <typename T>
        void UpdateComponent(T component) {
            m_Mesh->UpdateComponent<T>(component);
        }

        void Draw(Shader &shader, Camera &camera,
                  glm::mat4 trans = glm::mat4(1));
    };

    class DLL_API Spritesheet {
    public:
        Mesh *m_Mesh;

        Spritesheet(const char *texPath, Material &mat, Vector2 sheetSize,
                    Vector2 spriteSize, Vector2 spriteCoords);

        template <typename T>
        void AddComponent(T component) {
            m_Mesh->AddComponent(component);
        }

        template <typename T>
        T GetComponent() {
            return m_Mesh->GetComponent<T>();
        }

        template <typename T>
        void UpdateComponent(T component) {
            m_Mesh->UpdateComponent<T>(component);
        }

        void Draw(Shader &shader, Camera &camera);
    };

    struct DLL_API mg_Animation {
        std::vector<Spritesheet> frames;
        int currentFrame = 0;
        float delay = 0.1;
        std::string keyframe;
    };

    class DLL_API SpritesheetAnimation {
    public:
        std::vector<mg_Animation *> animations;
        mg_Animation *currentAnimation = nullptr;

        float time = 0.1;
        float prevTime = 0;

        SpritesheetAnimation() = default;

        void AddAnimation(mg_Animation *animation) {
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
            // get delta time
            float currentTime = glfwGetTime();
            float deltaTime = currentTime - prevTime;
            prevTime = currentTime;

            time -= deltaTime;

            if (currentAnimation != nullptr &&
                currentAnimation->frames.size() > 0) {
                currentAnimation->frames[currentAnimation->currentFrame].Draw(
                    shader, camera);
                if (time <= 0) {
                    currentAnimation->currentFrame++;
                    if (currentAnimation->currentFrame >=
                        currentAnimation->frames.size() - 1) {
                        currentAnimation->currentFrame = 0;
                    }

                    time = currentAnimation->delay;
                }
            }
        }
    };

    class DLL_API Graphic {
    public:
        Mesh *m_Mesh;

        Graphic(Vector3 rgb);

        template <typename T>
        void AddComponent(T component) {
            m_Mesh->AddComponent(component);
        }

        template <typename T>
        T GetComponent() {
            return m_Mesh->GetComponent<T>();
        }

        template <typename T>
        void UpdateComponent(T component) {
            m_Mesh->UpdateComponent<T>(component);
        }

        void Draw(Shader &shader, Camera &camera);
    };
} // namespace HyperAPI