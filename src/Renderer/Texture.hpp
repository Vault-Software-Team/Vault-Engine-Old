#pragma once
#include <libs.hpp>

namespace HyperAPI {
    struct m_Texture {
        uint32_t ID;
        int width, height, nrChannels;
        unsigned char *data;
        const char *texType;
        uint32_t slot;
        std::string texPath;
        const char *texStarterPath;
        int sharing;

        ~m_Texture();
    };

    extern std::vector<m_Texture *> textures;
    class Texture {
    public:
        std::string texPath;
        m_Texture *tex = nullptr;

        Texture(const char *texturePath, uint32_t slot,
                const char *textureType);
        Texture(unsigned char *m_Data, uint32_t slot, const char *textureType,
                const char *texturePath = "");

        ~Texture();

        void Bind(uint32_t slot = -1);
        void Unbind();
    };
} // namespace HyperAPI