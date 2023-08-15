#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"
#include "../Renderer/Timestep.hpp"

#include "Bloom.hpp"

namespace HyperAPI::Experimental {
    struct DLL_API m_SpritesheetAnimationData {
        struct DLL_API Frame {
            Vector2 size = Vector2(0.0f, 0.0f);
            Vector2 offset = Vector2(0.0f, 0.0f);
        };

        char name[499] = "anim_name";
        std::string id = uuid::generate_uuid_v4();
        float delay = 0.1f;
        float delay_counter = 0.0f;

        std::vector<Frame> frames;

        bool loop = false;
    };

    std::vector<m_SpritesheetAnimationData>
    GetAnimationsFromXML(const char *texPath, float delay, Vector2 sheetSize,
                         const std::string &xmlFile);

    struct DLL_API c_SpritesheetAnimation : public BaseComponent {
        Material mat{Vector4(1, 1, 1, 1)};
        Spritesheet sheet{"", mat, Vector2(0, 0), Vector2(0, 0), Vector2(0, 0)};
        Mesh *mesh;
        char currAnim[499] = "";
        std::vector<m_SpritesheetAnimationData> anims;
        Vector2 spritesheetSize;
        m_SpritesheetAnimationData::Frame currFrame;

        struct DLL_API CustomShader {
            bool usingCustomShader = false;
            Shader *shader = nullptr;
        } customShader;

        c_SpritesheetAnimation() { mesh = sheet.m_Mesh; }

        void DeleteComp() override {
            for (auto &anim : anims) {
                anim.frames.clear();
            }
            anims.clear();

            if (mesh) {
                delete mesh;
            }
        }

        void GUI() override;

        void Play() {
            for (auto &animation : anims) {
                if (std::string(animation.name) == std::string(currAnim)) {
                    Scene::currDelays[ID][std::string(animation.name)] +=
                        Timestep::deltaTime;
                    if (Scene::currDelays[ID][std::string(animation.name)] >=
                        animation.delay) {
                        Scene::currFrames[ID][std::string(animation.name)] += 1;
                        if (Scene::currFrames[ID][std::string(
                                animation.name)] >= animation.frames.size()) {
                            if (animation.loop) {
                                Scene::currFrames[ID]
                                                 [std::string(animation.name)] =
                                                     0;
                            } else {
                                Scene::currFrames[ID]
                                                 [std::string(animation.name)] =
                                                     animation.frames.size() -
                                                     1;
                            }
                        }
                        currFrame =
                            animation.frames[Scene::currFrames[ID][std::string(
                                animation.name)]];
                        Scene::currDelays[ID][std::string(animation.name)] =
                            0.0f;
                    }
                } else if (!animation.loop) {
                    Scene::currFrames[ID][std::string(animation.name)] = 0;
                }
            }
        }

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

            bool trulyChanged;
            for (auto &vertex : mesh->vertices) {
                m_SpritesheetAnimationData::Frame currFrame;
                for (auto &animation : anims) {
                    if (std::string(animation.name) == std::string(currAnim)) {
                        currFrame =
                            animation.frames[Scene::currFrames[ID][std::string(
                                animation.name)]];
                        break;
                    }
                }
                int index = &vertex - &mesh->vertices[0];
                float xCoord = currFrame.offset.x + currFrame.size.x;
                float yCoord = (spritesheetSize.y -
                                (currFrame.offset.y + currFrame.size.y)) +
                               currFrame.size.y;

                std::vector<Vector2> texCoords = {
                    Vector2(currFrame.offset.x / spritesheetSize.x,
                            (spritesheetSize.y -
                             (currFrame.offset.y + currFrame.size.y)) /
                                spritesheetSize.y),
                    Vector2(xCoord / spritesheetSize.x,
                            (spritesheetSize.y -
                             (currFrame.offset.y + currFrame.size.y)) /
                                spritesheetSize.y),
                    Vector2(xCoord / spritesheetSize.x,
                            yCoord / spritesheetSize.y),
                    Vector2(currFrame.offset.x / spritesheetSize.x,
                            yCoord / spritesheetSize.y)};

                trulyChanged = !(vertex.texUV.x == texCoords[index].x && vertex.texUV.y == texCoords[index].y);

                vertex.texUV = texCoords[index];
            }

            if (trulyChanged) {
                glBindVertexArray(mesh->VAO);
                glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mesh->vertices.size(),
                                mesh->vertices.data());
            }
        }
    };
} // namespace HyperAPI::Experimental