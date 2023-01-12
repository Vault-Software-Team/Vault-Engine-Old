#include "Renderer.hpp"
#include "Shader.hpp"

namespace HyperAPI {
    Renderer::Renderer(int width, int height, const char *title,
                       Vector2 g_gravity, uint32_t samples, bool fullscreen,
                       bool resizable, bool wireframe) {
        this->samples = samples;
        this->wireframe = wireframe;

        // mix
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

        if (!glfwInit()) {
            HYPER_LOG("Failed to initialize GLFW")
        }
        // //set verisons
        glfwWindowHint(GLFW_RESIZABLE, resizable ? GL_TRUE : GL_FALSE);
        // 3.1 version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        // core profile
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // error callback
        glfwSetErrorCallback([](int error, const char *description) {
            HYPER_LOG("GLFW Error (" << error << "): " << description)
        });

        if (fullscreen) {
            // get monitor width and height
            const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            window = glfwCreateWindow(mode->width, mode->height, title,
                                      glfwGetPrimaryMonitor(), NULL);
        } else {
            window = glfwCreateWindow(width, height, title, NULL, NULL);
        }

        if (!window) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }

        glfwMakeContextCurrent(window);
        gladLoadGL();

        // set minimum size of a window;
        glfwSetWindowSizeLimits(window, 800, 600, GLFW_DONT_CARE,
                                GLFW_DONT_CARE);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        // blur cubemap
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        // add icon to window
        GLFWimage images[1];
        images[0].pixels = stbi_load("build/logo2.png", &images[0].width,
                                     &images[0].height, 0, 4);
        glfwSetWindowIcon(window, 1, images);

        // glEnable(GL_FRAMEBUFFER_SRGB);

        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_BACK);
        // glFrontFace(GL_CW);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glDepthFunc(GL_LESS);
    }

    std::pair<GLint, GLint> Renderer::GetVRamUsage() {
        GLint totalMemoryInKB = 0;
        glGetIntegerv(0x9048, &totalMemoryInKB);

        GLint curAvailMemoryInKB = 0;
        glGetIntegerv(0x9049, &curAvailMemoryInKB);

        // return in megabytes
        return {(totalMemoryInKB - curAvailMemoryInKB) / 1024,
                totalMemoryInKB / 1024};
    }

    void Renderer::Render(Camera &camera) {
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        glfwSwapBuffers(window);
    }

    void Renderer::NewFrame() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);
    }

    void Renderer::Swap(Shader &framebufferShader, uint32_t FBO,
                        uint32_t rectVAO, uint32_t postProcessingTexture,
                        uint32_t postProcessingFBO) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        framebufferShader.Bind();
        glBindVertexArray(rectVAO);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDepthFunc(GL_LEQUAL);
    }
} // namespace HyperAPI