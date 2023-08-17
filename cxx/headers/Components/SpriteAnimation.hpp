#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"
#include "../Renderer/Timestep.hpp"

#include "SpriteRenderer.hpp"
#include "GameObject.hpp"

namespace HyperAPI::Experimental {
    struct DLL_API SpriteAnimation : public BaseComponent {
        Mesh *currMesh;
        std::vector<m_AnimationData> anims;
        char currAnim[499] = "";

        struct DLL_API CustomShader {
            bool usingCustomShader = false;
            Shader *shader = nullptr;
        } customShader;

        SpriteAnimation() {
            currMesh = nullptr;
            for (auto &gameObject : (*Scene::m_GameObjects)) {
                if (gameObject->ID == ID) {
                    std::map<std::string, int> m_CurrFrames;
                    Scene::currFrames[gameObject->ID] = m_CurrFrames;

                    std::map<std::string, float> m_CurrDelays;
                    Scene::currDelays[gameObject->ID] = m_CurrDelays;
                }
            }
        }

        void DeleteComp() override {
            for (auto &gameObject : (*Scene::m_GameObjects)) {
                if (gameObject->ID == ID) {
                    Scene::currFrames.erase(gameObject->ID);
                    Scene::currDelays.erase(gameObject->ID);
                }
            }

            for (auto &anim : anims) {
                for (auto &frame : anim.frames) {
                    if (frame.mesh)
                        delete frame.mesh;
                }
                anim.frames.clear();
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
                        Scene::currDelays[ID][std::string(animation.name)] =
                            0.0f;
                    }
                    currMesh = animation
                                   .frames[Scene::currFrames[ID][std::string(
                                       animation.name)]]
                                   .mesh;

                    break;
                }
            }
        }
    };
} // namespace HyperAPI::Experimental