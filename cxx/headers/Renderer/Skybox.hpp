#pragma once
#include <libs.hpp>

namespace HyperAPI {
    class Skybox {
    public:
        uint32_t skyboxVAO, skyboxVBO, skyboxEBO;
        Shader *shader;
        uint32_t cubemapTexture;
        std::vector<std::string> facesCubemap;

        Skybox(const std::string &right, const std::string &left,
               const std::string &top, const std::string &bottom,
               const std::string &front, const std::string &back);

        void Draw(Camera &camera, int width, int height);
    };
} // namespace HyperAPI