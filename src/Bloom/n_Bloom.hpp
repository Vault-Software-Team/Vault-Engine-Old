#pragma once
#include <dllapi.hpp>
#include <libs.hpp>

namespace HyperAPI {
    namespace n_Bloom {
        struct DLL_API BloomMip {
            glm::vec2 size;
            glm::ivec2 intSize;
            uint32_t texture;
        };

        class DLL_API BloomBuffer {
        public:
            BloomBuffer();
            ~BloomBuffer();

            bool Init(uint32_t windowWidth, uint32_t windowHeight,
                      uint32_t mipChainLength);
            void Destroy();
            void BindWriting();

            const std::vector<BloomMip> &GetMipChain() const {
                return m_MipChain;
            }

        private:
            bool m_Init;
            uint32_t m_Framebuffer;
            std::vector<BloomMip> m_MipChain;
        };

        class DLL_API BloomRenderer {
        public:
            BloomRenderer();
            ~BloomRenderer();
            bool Init(uint32_t windowWidth, uint32_t windowHeight);
            void Destroy();
            void RenderBloomTexture(uint32_t srcTexture, float filterRadius,
                                    uint32_t &quadVAO);
            uint32_t BloomTexture();

            glm::ivec2 m_SrcViewportSize;
            glm::vec2 m_SrcViewportSizeFloat;

        private:
            void RenderDownsamples(uint32_t srcTexture, uint32_t &quadVAO);

            void RenderUpsamples(float filterRadius, uint32_t &quadVAO);
            bool mInit;
            BloomBuffer m_Framebuffer;
            Shader *m_DownsampleShader;
            Shader *m_UpsampleShader;
        };
    } // namespace n_Bloom
} // namespace HyperAPI