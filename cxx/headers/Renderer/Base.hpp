#pragma once
#include <libs.hpp>

#include "Renderer.hpp"

extern float rectangleVert[];

void NewFrame(uint32_t FBO, int width, int height);

void EndFrame(HyperAPI::Shader &framebufferShader, HyperAPI::Renderer &renderer,
              uint32_t rectVAO, uint32_t postProcessingTexture,
              uint32_t postProcessingFBO, const int width, const int height);

void EndEndFrame(HyperAPI::Shader &framebufferShader,
                 HyperAPI::Renderer &renderer, uint32_t rectVAO,
                 uint32_t postProcessingTexture, uint32_t postProcessingFBO,
                 uint32_t S_postProcessingTexture, uint32_t S_postProcessingFBO,
                 const int width, const int height, const int mouseX,
                 const int mouseY);

void SC_EndFrame(HyperAPI::Renderer &renderer, uint32_t FBO, uint32_t rectVAO,
                 uint32_t postProcessingTexture, uint32_t postProcessingFBO,
                 const int width, const int height);

bool EndsWith(std::string const &value, std::string const &ending);

extern std::vector<HyperAPI::PointLight *> PointLights;
extern std::vector<HyperAPI::Light2D *> Lights2D;
extern std::vector<HyperAPI::SpotLight *> SpotLights;
extern std::vector<HyperAPI::DirectionalLight *> DirLights;
extern std::vector<HyperAPI::Mesh *> hyperEntities;