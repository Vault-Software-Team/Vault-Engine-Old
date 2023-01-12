#pragma once
#include <libs.hpp>

namespace HyperAPI {
    class Texture {
    public:
        uint32_t ID;
        int width, height, nrChannels;
        unsigned char *data;
        const char *texType;
        uint32_t slot;
        std::string texPath;
        const char *texStarterPath;

        Texture(const char *texturePath, uint32_t slot,
                const char *textureType);
        Texture(unsigned char *m_Data, uint32_t slot, const char *textureType,
                const char *texturePath = "");

        ~Texture() {
            HYPER_LOG("Texture " + texPath + " deleted");
            glDeleteTextures(1, &ID);
        }

        void Bind(uint32_t slot = -1);

        void Unbind();
    };
} // namespace HyperAPI