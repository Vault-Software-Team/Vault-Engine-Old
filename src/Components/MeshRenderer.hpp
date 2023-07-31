#pragma once
#include <dllapi.hpp>
#include <fstream>
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"
#include "Bloom.hpp"
#include "../Renderer/Shapes.hpp"

namespace HyperAPI::Experimental {
    struct DLL_API MeshRenderer : public BaseComponent {
        Mesh *m_Mesh;
        bool m_Model = false;

        glm::mat4 extraMatrix = glm::mat4(1.0f);
        std::string matPath = "";
        std::string meshType = "";
        int mesh_index = -1;

        struct DLL_API CustomShader {
            bool usingCustomShader = false;
            Shader *shader = nullptr;
        } customShader;

        MeshRenderer() = default;
        void DeleteComp() override {
            if (m_Mesh != nullptr) {
                delete m_Mesh;
            }
        }

        void GUI() override;
        void Update();
    };
} // namespace HyperAPI::Experimental