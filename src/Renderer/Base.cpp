#include "Base.hpp"
#include "Shader.hpp"

void NewFrame(uint32_t FBO, int width, int height) {

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    // glClearColor(pow(0.3f, 2.2f), pow(0.3f, 2.2f), pow(0.3f, 2.2f), 1.0f);
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
}

void EndFrame(HyperAPI::Shader &framebufferShader, HyperAPI::Renderer &renderer,
              uint32_t rectVAO, uint32_t postProcessingTexture,
              uint32_t postProcessingFBO, const int width, const int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    framebufferShader.Bind();
    framebufferShader.SetUniform1i("screenTexture", 15);
    glBindVertexArray(rectVAO);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDepthFunc(GL_LEQUAL);
    if (renderer.wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void EndEndFrame(HyperAPI::Shader &framebufferShader,
                 HyperAPI::Renderer &renderer, uint32_t rectVAO,
                 uint32_t postProcessingTexture, uint32_t postProcessingFBO,
                 uint32_t S_postProcessingTexture, uint32_t S_postProcessingFBO,
                 const int width, const int height, const int mouseX,
                 const int mouseY) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    NewFrame(S_postProcessingFBO, width, height);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    framebufferShader.Bind();
    framebufferShader.SetUniform1i("screenTexture", 15);
    glBindVertexArray(rectVAO);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    EndFrame(framebufferShader, renderer, rectVAO, S_postProcessingTexture,
             S_postProcessingFBO, width, height);

    glDepthFunc(GL_LEQUAL);
    if (renderer.wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void SC_EndFrame(HyperAPI::Renderer &renderer, uint32_t FBO, uint32_t rectVAO,
                 uint32_t postProcessingTexture, uint32_t postProcessingFBO,
                 const int width, const int height) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool EndsWith(std::string const &value, std::string const &ending) {
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

DLL_API std::vector<HyperAPI::PointLight *> PointLights;
DLL_API std::vector<HyperAPI::SpotLight *> SpotLights;
DLL_API std::vector<HyperAPI::Light2D *> Lights2D;
DLL_API std::vector<HyperAPI::DirectionalLight *> DirLights;
DLL_API std::vector<HyperAPI::Mesh *> hyperEntities;

DLL_API float rectangleVert[] = {
    1,
    -1,
    1,
    0,
    -1,
    -1,
    0,
    0,
    -1,
    1,
    0,
    1,

    1,
    1,
    1,
    1,
    1,
    -1,
    1,
    0,
    -1,
    1,
    0,
    1,
};

DLL_API float rotation = 0.0f;
DLL_API double previousTime = glfwGetTime();