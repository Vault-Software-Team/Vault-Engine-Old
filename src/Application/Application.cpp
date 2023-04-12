#include "Application.hpp"
#include "../Bloom/n_Bloom.hpp"
#include "../Renderer/Base.hpp"
#include "../Components/LuaScriptComponent.hpp"
#include "../Components/Model.hpp"
#include "../Components/NativeScriptManager.hpp"
#include "../Components/CppScriptManager.hpp"
#include "../Components/CsharpScriptManager.hpp"
#include "../Components/Rigidbody2D.hpp"
#include "../Components/Rigidbody3D.hpp"
#include "../Components/BoxCollider2D.hpp"
#include "../Components/Colliders3D.hpp"
#include "../Components/PathfindingAI.hpp"
#include "../Components/SpriteAnimation.hpp"
#include "../Components/SpritesheetAnimation.hpp"
#include "../Renderer/Camera.hpp"
#include "../Debugging/GLError.hpp"

namespace Hyper {
    void Application::Run(
        std::function<void(uint32_t &)> update,
        std::function<void(uint32_t &PPT, uint32_t &PPFBO, uint32_t &gui_gui)>
            gui,
        std::function<void(HyperAPI::Shader &)> shadowMapRender) {
        HYPER_LOG("Application started")
        width = 1280;
        height = 720;
        float gamma = 2.2f;
        // if (renderOnScreen) {
        // glEnable(GL_FRAMEBUFFER_SRGB);
        // }

        HyperAPI::Shader shadowMapProgram("shaders/shadowMap.glsl");
        HyperAPI::Shader framebufferShader("shaders/framebuffer.glsl");
        HyperAPI::Shader blurShader("shaders/blur.glsl");

        framebufferShader.Bind();
        framebufferShader.SetUniform1f("gamma", gamma);
        // HyperAPI::Shader shader("shaders/sprite.glsl");
        // shader.Bind();
        // shader.SetUniform1f("ambient", 0.5);

        glfwSwapInterval(1);

        uint32_t rectVAO, rectVBO;
        glGenVertexArrays(1, &rectVAO);
        glGenBuffers(1, &rectVBO);
        glBindVertexArray(rectVAO);
        glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
        // dynamic
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr,
                     GL_DYNAMIC_DRAW);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVert), &rectangleVert,
        // GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                              (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                              (void *)(2 * sizeof(float)));

        uint32_t postProcessingFBO;
        glGenFramebuffers(1, &postProcessingFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);
        glCheckError();

        uint32_t postProcessingTexture;
        glGenTextures(1, &postProcessingTexture);
        glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, postProcessingTexture, 0);
        glCheckError();

        uint32_t bloomTexture;
        glGenTextures(1, &bloomTexture);
        glBindTexture(GL_TEXTURE_2D, bloomTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                     GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                               GL_TEXTURE_2D, bloomTexture, 0);
        glCheckError();

        // r32i entityTexture
        uint32_t entityTexture;
        glGenTextures(1, &entityTexture);
        glBindTexture(GL_TEXTURE_2D, entityTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0,
                     GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
                               GL_TEXTURE_2D, entityTexture, 0);
        glCheckError();

        // rbo
        uint32_t rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width,
                              height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, rbo);
        glCheckError();

        uint32_t attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                   GL_COLOR_ATTACHMENT2};
        glDrawBuffers(3, attachments);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        uint32_t pingpongFBO[2];
        uint32_t pingpongBuffer[2];
        glGenFramebuffers(2, pingpongFBO);
        glGenTextures(2, pingpongBuffer);

        for (uint32_t i = 0; i < 2; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D, pingpongBuffer[i], 0);
        }

        uint32_t S_PPFBO;
        glGenFramebuffers(1, &S_PPFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, S_PPFBO);
        glCheckError();

        uint32_t S_PPT;
        glGenTextures(1, &S_PPT);
        glBindTexture(GL_TEXTURE_2D, S_PPT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, S_PPT, 0);
        glCheckError();

        uint32_t SRBO;
        glGenRenderbuffers(1, &SRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, SRBO);
        glRenderbufferStorage(GL_RENDERBUFFER,
                              GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, SRBO);
        glCheckError();

        float timeStep = 1.0f / 60.0f;

        HYPER_LOG("Renderer initialized")
        float timeSinceAppStart = 0.0f;

        uint32_t depthMapFBO;
        glGenFramebuffers(1, &depthMapFBO);

        const uint32_t SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

        uint32_t depthMap;
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
                     SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        std::vector<HyperAPI::Experimental::GameObject *> mouseOverObjects;

        glEnable(GL_DEPTH_TEST);

        HyperAPI::n_Bloom::BloomRenderer bloomRenderer;
        bloomRenderer.Init(width, height);

        // create a framebuffer test
        uint32_t testFBO;
        glGenFramebuffers(1, &testFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, testFBO);

        uint32_t testTexture;
        glGenTextures(1, &testTexture);
        glBindTexture(GL_TEXTURE_2D, testTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, testTexture, 0);

        uint32_t testRBO;
        glGenRenderbuffers(1, &testRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, testRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width,
                              height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER, testRBO);
        glCheckError();

        float prevWidth, prevHeight;
        while (!glfwWindowShouldClose(renderer->window)) {
            bloomRenderer.m_SrcViewportSize = glm::vec2(width, height);
            bloomRenderer.m_SrcViewportSizeFloat = glm::vec2(width, height);
            framebufferShader.Bind();
            framebufferShader.SetUniform1f("exposure", exposure);

            framebufferShader.Bind();
            framebufferShader.SetUniform1f(
                "chromaticAmount",
                HyperAPI::config.postProcessing.chromaticAberration.intensity);
            framebufferShader.SetUniform1f(
                "vignetteAmount",
                HyperAPI::config.postProcessing.vignette.intensity);

            HyperAPI::Timestep::currentFrame = glfwGetTime();
            HyperAPI::Timestep::deltaTime = HyperAPI::Timestep::currentFrame -
                                            HyperAPI::Timestep::lastFrame;
            HyperAPI::Timestep::lastFrame = HyperAPI::Timestep::currentFrame;

            timeSinceAppStart += HyperAPI::Timestep::deltaTime;

            glfwPollEvents();
            glfwGetWindowSize(renderer->window, &winWidth, &winHeight);
            glClear(GL_DEPTH_BUFFER_BIT);

            if (width != prevWidth || prevHeight != height) {
                glDeleteFramebuffers(1, &postProcessingFBO);
                glDeleteTextures(1, &postProcessingTexture);

                glDeleteFramebuffers(1, &S_PPFBO);
                glDeleteTextures(1, &S_PPT);
                glDeleteTextures(1, &bloomTexture);
                glDeleteFramebuffers(2, pingpongFBO);
                glDeleteTextures(2, pingpongBuffer);
                glDeleteRenderbuffers(1, &SRBO);
                glDeleteRenderbuffers(1, &rbo);
                glDeleteTextures(1, &entityTexture);
                glDeleteTextures(1, &testTexture);
                glDeleteFramebuffers(1, &testFBO);
                glDeleteRenderbuffers(1, &testRBO);

                glGenFramebuffers(1, &testFBO);
                glBindFramebuffer(GL_FRAMEBUFFER, testFBO);

                glGenTextures(1, &testTexture);
                glBindTexture(GL_TEXTURE_2D, testTexture);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0,
                             GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                                GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                                GL_CLAMP_TO_EDGE);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                       GL_TEXTURE_2D, testTexture, 0);

                glGenRenderbuffers(1, &testRBO);
                glBindRenderbuffer(GL_RENDERBUFFER, testRBO);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                                      width, height);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                          GL_DEPTH_STENCIL_ATTACHMENT,
                                          GL_RENDERBUFFER, testRBO);

                glGenFramebuffers(1, &postProcessingFBO);
                glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);

                glGenTextures(1, &postProcessingTexture);
                glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0,
                             GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                                GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                                GL_CLAMP_TO_EDGE);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                       GL_TEXTURE_2D, postProcessingTexture, 0);

                glGenTextures(1, &bloomTexture);
                glBindTexture(GL_TEXTURE_2D, bloomTexture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0,
                             GL_RGB, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                                GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                                GL_CLAMP_TO_EDGE);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                                       GL_TEXTURE_2D, bloomTexture, 0);

                glGenTextures(1, &entityTexture);
                glBindTexture(GL_TEXTURE_2D, entityTexture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0,
                             GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                                GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                                GL_CLAMP_TO_EDGE);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
                                       GL_TEXTURE_2D, entityTexture, 0);

                glDrawBuffers(3, attachments);

                glGenRenderbuffers(1, &rbo);
                glBindRenderbuffer(GL_RENDERBUFFER, rbo);
                glRenderbufferStorage(
                    GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                    width, height);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                          GL_DEPTH_STENCIL_ATTACHMENT,
                                          GL_RENDERBUFFER, rbo);

                glBindFramebuffer(GL_FRAMEBUFFER, S_PPFBO);

                // glGenFramebuffers(2, pingpongFBO);
                // glGenTextures(2, pingpongBuffer);

                // for(uint32_t i = 0; i < 2; i++)
                // {
                //     glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
                //     glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
                //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height,
                //     0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
                //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                //     GL_NEAREST); glTexParameteri(GL_TEXTURE_2D,
                //     GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                //     GL_CLAMP_TO_EDGE); glTexParameteri(GL_TEXTURE_2D,
                //     GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                //     glFramebufferTexture2D(GL_FRAMEBUFFER,
                //     GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i],
                //     0);
                // }
                // glBindFramebuffer(GL_FRAMEBUFFER, 0);

                glGenFramebuffers(1, &S_PPFBO);
                glBindFramebuffer(GL_FRAMEBUFFER, S_PPFBO);

                glGenTextures(1, &S_PPT);
                glBindTexture(GL_TEXTURE_2D, S_PPT);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0,
                             GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                                GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                                GL_CLAMP_TO_EDGE);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                       GL_TEXTURE_2D, S_PPT, 0);
                glBindBuffer(GL_ARRAY_BUFFER, rectVBO);

                glGenRenderbuffers(1, &SRBO);
                glBindRenderbuffer(GL_RENDERBUFFER, SRBO);
                glRenderbufferStorage(
                    GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                    width, height);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                          GL_DEPTH_STENCIL_ATTACHMENT,
                                          GL_RENDERBUFFER, SRBO);
            }

            prevWidth = width;
            prevHeight = height;

            float rectangleVert[] = {
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

            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(rectangleVert),
                            rectangleVert);
            NewFrame(postProcessingFBO, width, height);

            if (renderOnScreen) {
                glfwGetWindowSize(renderer->window, &width, &height);
                glViewport(0, 0, width, height);
            }

            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            update(S_PPT);

            glReadBuffer(GL_COLOR_ATTACHMENT2);
            uint32_t entityId;
            glReadPixels(sceneMouseX, sceneMouseY, 1, 1, GL_RED_INTEGER,
                         GL_UNSIGNED_INT, &entityId);

            for (auto &gameObject : HyperAPI::Scene::m_GameObjects) {
                if (sceneMouseY < 0 || sceneMouseX < 0) {
                    break;
                }

                using namespace HyperAPI::Experimental;

                if (HyperAPI::isRunning) {
                    for (auto &overObject : mouseOverObjects) {
                        if (overObject->entity != (entt::entity)entityId) {
                            if (overObject
                                    ->HasComponent<NativeScriptManager>()) {
                                auto &nativeManager =
                                    overObject
                                        ->GetComponent<NativeScriptManager>();
                                for (auto &script :
                                     nativeManager.m_StaticScripts) {
                                    script->OnMouseExit();
                                    mouseOverObjects.push_back(overObject);
                                }
                            }

                            if (overObject
                                    ->HasComponent<m_LuaScriptComponent>()) {
                                auto &nativeManager =
                                    overObject
                                        ->GetComponent<m_LuaScriptComponent>();
                                for (auto &script : nativeManager.scripts) {
                                    script.OnMouseExit();
                                    mouseOverObjects.push_back(overObject);
                                }
                            }

                            if (overObject->HasComponent<CsharpScriptManager>()) {
                                auto &scriptManager = overObject->GetComponent<CsharpScriptManager>();
                                for (auto klass : scriptManager.selectedScripts) {
                                    MonoObject *exception = nullptr;
                                    MonoScriptClass *behaviour =
                                        HyperAPI::CsharpScriptEngine::instances[klass.first];
                                    MonoMethod *method = behaviour->GetMethod("OnMouseExit", 0);
                                    if (!method)
                                        continue;

                                    void *params[0] = {};
                                    mono_runtime_invoke(method, behaviour->f_GetObject(), params, &exception);
                                }
                            }

                            mouseOverObjects.erase(
                                std::remove(mouseOverObjects.begin(),
                                            mouseOverObjects.end(), overObject),
                                mouseOverObjects.end());
                            break;
                        }
                    }
                }

                if (gameObject->entity == (entt::entity)entityId) {
                    currently_hovering_over = gameObject;
                    if (HyperAPI::isRunning) {
                        if (gameObject->HasComponent<NativeScriptManager>()) {
                            auto &nativeManager =
                                gameObject->GetComponent<NativeScriptManager>();
                            for (auto &script : nativeManager.m_StaticScripts) {
                                script->OnMouseEnter();
                            }
                        }
                        // if exists
                        if (std::find(mouseOverObjects.begin(),
                                      mouseOverObjects.end(),
                                      gameObject) == mouseOverObjects.end()) {
                            mouseOverObjects.push_back(gameObject);
                        }

                        if (gameObject->HasComponent<m_LuaScriptComponent>()) {
                            auto &nativeManager =
                                gameObject
                                    ->GetComponent<m_LuaScriptComponent>();
                            for (auto &script : nativeManager.scripts) {
                                script.OnMouseEnter();
                            }
                        }

                        if (gameObject->HasComponent<CsharpScriptManager>()) {
                            auto &scriptManager = gameObject->GetComponent<CsharpScriptManager>();
                            for (auto klass : scriptManager.selectedScripts) {
                                MonoObject *exception = nullptr;
                                MonoScriptClass *behaviour =
                                    HyperAPI::CsharpScriptEngine::instances[klass.first];
                                MonoMethod *method = behaviour->GetMethod("OnMouseEnter", 0);
                                if (!method)
                                    continue;

                                void *params[0] = {};
                                mono_runtime_invoke(method, behaviour->f_GetObject(), params, &exception);
                            }
                        }
                    }

                    double mouseX, mouseY;
                    glfwGetCursorPos(renderer->window, &mouseX, &mouseY);

                    if (ImGui::IsMouseDoubleClicked(0) && !mouseClicked &&
                        !isGuzimoInUse) {
                        HyperAPI::Scene::m_Object = gameObject;
                        strncpy(HyperAPI::Scene::name,
                                HyperAPI::Scene::m_Object->name.c_str(), 499);
                        HyperAPI::Scene::name[499] = '\0';

                        strncpy(HyperAPI::Scene::tag,
                                HyperAPI::Scene::m_Object->tag.c_str(), 499);
                        HyperAPI::Scene::tag[499] = '\0';

                        strncpy(HyperAPI::Scene::layer,
                                HyperAPI::Scene::m_Object->layer.c_str(), 32);
                        HyperAPI::Scene::layer[31] = '\0';
                        glfwSetCursorPos(renderer->window, mouseX, mouseY);

                        mouseClicked = true;
                    } else if (!ImGui::IsMouseDoubleClicked(0)) {
                        mouseClicked = false;
                    }

                    break;
                }
            }

            glClear(GL_DEPTH_BUFFER_BIT);

            bool horizontal = true, first_iteration = true;
            int amount = 10;
            blurShader.Bind();

            glDisable(GL_BLEND);
            bloomRenderer.RenderBloomTexture(bloomTexture, 0.005f, rectVAO);

            glActiveTexture(GL_TEXTURE16);
            glBindTexture(GL_TEXTURE_2D, bloomRenderer.BloomTexture());

            glClear(GL_DEPTH_BUFFER_BIT);
            framebufferShader.Bind();
            glActiveTexture(GL_TEXTURE16);
            glBindTexture(GL_TEXTURE_2D, bloomRenderer.BloomTexture());
            framebufferShader.SetUniform1i("bloomTexture", 16);

            if (renderOnScreen) {
                EndFrame(framebufferShader, *renderer, rectVAO,
                         postProcessingTexture, postProcessingFBO, width,
                         height);
            } else {
                EndEndFrame(framebufferShader, *renderer, rectVAO,
                            postProcessingTexture, postProcessingFBO, S_PPT,
                            S_PPFBO, width, height, sceneMouseX, sceneMouseY);
            }

            if (!renderOnScreen) {
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                ImGuizmo::BeginFrame();

                gui(S_PPT, S_PPFBO, testTexture);

                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                GLFWwindow *backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }
            glfwSwapBuffers(renderer->window);
        }

        HYPER_LOG("Closing Vault Engine")
    }

    MousePicker::MousePicker(Application *app, HyperAPI::Camera *camera,
                             glm::mat4 projection) {
        this->camera = camera;
        this->projectionMatrix = projection;
        this->appRef = app;
    }

    Vector3 MousePicker::getCurrentRay() { return currentRay; }

    void MousePicker::update() { currentRay = calculateMouseRay(); }

    Vector3 MousePicker::calculateMouseRay() {
        Vector2 normalizedCoords = getNormalizedDeviceCoords(mouseX, mouseY);
        Vector4 clipCoords =
            Vector4(normalizedCoords.x, normalizedCoords.y, -1.0f, 1.0f);
        Vector4 eyeCoords = toEyeCoords(clipCoords);
        Vector3 worldRay = toWorldCoords(eyeCoords);

        return worldRay;
    }

    Vector2 MousePicker::getNormalizedDeviceCoords(float mouseX, float mouseY) {
        float x = (2.0f * mouseX) / winX - 1;
        float y = (2.0f * mouseY) / winY - 1.0f;
        return Vector2(x, y);
    }

    Vector4 MousePicker::toEyeCoords(Vector4 clipCoords) {
        glm::mat4 invertedProjection = glm::inverse(camera->projection);
        Vector4 eyeCoords = invertedProjection * clipCoords;
        return Vector4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
    }

    Vector3 MousePicker::toWorldCoords(Vector4 eyeCoords) {
        glm::mat4 invertedView = glm::inverse(camera->view);
        Vector4 rayWorld = invertedView * eyeCoords;
        Vector3 mouseRay = Vector3(rayWorld.x, rayWorld.y, rayWorld.z);
        mouseRay = glm::normalize(mouseRay);
        return mouseRay;
    }

    float LerpFloat(float a, float b, float t) { return a + t * (b - a); }
} // namespace Hyper