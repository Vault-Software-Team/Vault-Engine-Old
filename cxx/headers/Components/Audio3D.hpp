#pragma once
#include <cstdint>
#include <libs.hpp>
#include <memory>
#include "AL/al.h"
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"
#include "../Audio/SoundSource.hpp"
#include "../Audio/SoundBuffer.hpp"
#include "imgui/imgui.h"
#include "scene.hpp"
#include "Transform.hpp"

namespace HyperAPI::Experimental {
    struct Audio3D : BaseComponent {
        uint32_t audio;
        std::unique_ptr<SoundSource> m_src = nullptr;

        struct AudioFields {
            float pitch = 1;
            float volume = 1;
            float max_distance = 15;
            bool loop;
            glm::vec3 positions{0, 0, 0};
            glm::vec3 velocity{0, 0, 0};
        } src;

        bool on_start;
        std::string file;
        Audio3D() = default;

        void GUI() override {
            if (ImGui::TreeNode("3D Audio")) {
                ImGui::Text("NOTE: The audio file must be mono for it to be 3D!");
                ImGui::Selectable(("File: " + file).c_str(), false);
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload *payload =
                            ImGui::AcceptDragDropPayload("file")) {
                        dirPayloadData.erase(0, cwd.length() + 1);

                        file = dirPayloadData;
                    }

                    ImGui::EndDragDropTarget();
                }
                ImGui::NewLine();

                DrawVec3Control("Velocity", src.velocity);
                ImGui::Checkbox("Loop", &src.loop);
                ImGui::DragFloat("Volume", &src.volume, 0.1, 0, 1);
                ImGui::DragFloat("Pitch", &src.pitch, 0.1);
                ImGui::DragFloat("Max Distance", &src.max_distance, 0.1);
                ImGui::Checkbox("Play On Start", &on_start);

                ImGui::NewLine();
                if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                    Scene::m_Registry.remove<Audio3D>(entity);
                }

                ImGui::TreePop();
            }
        }

        void Start() {
            m_src = std::make_unique<SoundSource>();
            m_src->pitch = src.pitch;
            m_src->volume = src.volume;
            m_src->max_distance = src.max_distance;
            m_src->loop = src.loop;
            m_src->positions = src.positions;
            m_src->velocity = src.velocity;

            audio = SoundBuffer::get()->AddSoundEffect(file.c_str());
            if (on_start)
                Play();
        }

        void Update() {
            m_src->pitch = src.pitch;
            m_src->volume = src.volume;
            m_src->max_distance = src.max_distance;
            m_src->loop = src.loop;
            m_src->positions = src.positions;
            m_src->velocity = src.velocity;

            auto &transform = Scene::m_Registry.get<Transform>(entity);
            m_src->SetPosition(transform.position);
            m_src->Update();
        }

        void Play() {
            m_src->Play(audio);
        }

        void Stop() {
            alSourceStop(m_src->GetBuffer());
        }

        void SetVelocity(glm::vec3 vel) {
            m_src->SetVelocity(vel);
        }

        void SetVolume(float m_volume) {
            m_src->SetVolume(m_volume);
        }

        void SetPitch(float m_pitch) {
            m_src->SetPitch(m_pitch);
        }

        void Loop(bool m_loop) {
            m_src->Loop(m_loop);
        }

        void SetMaxDistance(float max) {
            m_src->SetMaxDistance(max);
        }
    };
} // namespace HyperAPI::Experimental