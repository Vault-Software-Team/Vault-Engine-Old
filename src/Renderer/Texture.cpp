#include "Texture.hpp"

namespace HyperAPI {
    Texture::Texture(const char *texturePath, uint32_t slot,
                     const char *textureType) {
        stbi_set_flip_vertically_on_load(true);
        texType = textureType;
        texStarterPath = texturePath;
        this->slot = slot;
        texPath = std::string(texturePath);
        data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

        HYPER_LOG("Texture " + std::to_string(slot) + " loaded from " +
                  texturePath)

        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if (std::string(textureType) == "texture_normal") {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, data);
        } else if (std::string(textureType) == "texture_height") {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, data);
        } else if (nrChannels >= 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, data);
        else if (nrChannels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, data);
        else if (nrChannels == 1)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RED,
                         GL_UNSIGNED_BYTE, data);
        else
            return;

        // throw std::invalid_argument("Texture format not supported");

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    Texture::Texture(unsigned char *m_Data, uint32_t slot,
                     const char *textureType, const char *texturePath) {
        stbi_set_flip_vertically_on_load(true);
        texType = textureType;
        texStarterPath = texturePath;
        this->slot = slot;
        texPath = std::string(texturePath);
        data = stbi_load_from_memory(data, 0, &width, &height, &nrChannels, 0);

        HYPER_LOG("Texture " + std::to_string(slot) + " loaded from " +
                  texturePath)

        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if (std::string(textureType) == "texture_normal") {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, data);
        } else if (std::string(textureType) == "texture_height") {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, data);
        } else if (nrChannels >= 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, data);
        else if (nrChannels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, data);
        else if (nrChannels == 1)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RED,
                         GL_UNSIGNED_BYTE, data);
        else
            return;

        // throw std::invalid_argument("Texture format not supported");

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Bind(uint32_t slot) {
        if (slot == -1) {
            glActiveTexture(GL_TEXTURE0 + this->slot);
            glBindTexture(GL_TEXTURE_2D, ID);
        } else {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, ID);
        }
    }

    void Texture::Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
} // namespace HyperAPI