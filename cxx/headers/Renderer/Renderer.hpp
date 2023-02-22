#pragma once
#include <libs.hpp>
#include "Structures.hpp"

namespace HyperAPI {
    class Renderer {
    public:
        bool wireframe;
        uint32_t postProcessingTexture;
        uint32_t postProcessingFBO;
        Shader *framebufferShader;
        uint32_t FBO;
        uint32_t bufferTexture;
        uint32_t rbo;
        uint32_t rectVAO, rectVBO;
        int width, height;

        const char *title;

        uint32_t samples = 8;

        std::vector<PointLight> PointLights;

        GLFWwindow *window;

        Renderer(int width, int height, const char *title, Vector2 g_gravity,
                 uint32_t samples = 8, bool fullscreen = false,
                 bool resizable = true, bool wireframe = false);

        void Render(Camera &camera);

        void Swap(HyperAPI::Shader &framebufferShader, uint32_t FBO,
                  uint32_t rectVAO, uint32_t postProcessingTexture,
                  uint32_t postProcessingFBO);

        void NewFrame();
        std::pair<GLint, GLint> GetVRamUsage();
    };
} // namespace HyperAPI