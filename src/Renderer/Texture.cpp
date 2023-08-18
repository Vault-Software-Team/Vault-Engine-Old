#include "Texture.hpp"
#include "libs.hpp"
#include <exception>

namespace HyperAPI {
    DLL_API std::vector<m_Texture *> textures;
    DLL_API std::vector<m_Texture *> image_textures;

    m_Texture::~m_Texture() {
        HYPER_LOG("Texture " + texPath + " unloaded");
        glDeleteTextures(1, &ID);
        textures.erase(std::remove(textures.begin(), textures.end(), this), textures.end());
    }

    Texture::Texture(const char *texturePath, uint32_t slot, const char *textureType) {
        std::cout << texturePath << std::endl;
        texPath = std::string(texturePath);
        if (std::string(textureType) != "texture_image") {
            for (auto *m_tex : textures) {
                if (m_tex->texPath == std::string(texturePath)) {
                    tex = m_tex;
                    tex->sharing++;
                    break;
                }
            }
        } else {
            for (auto *m_tex : image_textures) {
                if (m_tex->texPath == std::string(texturePath)) {
                    tex = m_tex;
                    tex->sharing++;
                    break;
                }
            }
        }

        if (tex == nullptr) {
            tex = new m_Texture();
            tex->sharing++;

            stbi_set_flip_vertically_on_load(true);
            tex->texType = textureType;
            tex->texStarterPath = texturePath;
            tex->slot = slot;
            tex->texPath = std::string(texturePath);
            tex->data = stbi_load(texturePath, &tex->width, &tex->height, &tex->nrChannels, 0);

            HYPER_LOG("Texture " + std::to_string(slot) + " loaded from " +
                      texturePath)

            glGenTextures(1, &tex->ID);
            glBindTexture(GL_TEXTURE_2D, tex->ID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            if (std::string(textureType) == "texture_height") {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->data);
            } else if (std::string(textureType) == "texture_normal") {
                if (tex->nrChannels >= 4)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->data);
                else if (tex->nrChannels == 3)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->width, tex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex->data);
                else if (tex->nrChannels == 1)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->width, tex->height, 0, GL_RED, GL_UNSIGNED_BYTE, tex->data);
            } else if (std::string(textureType) == "texture_image") {
                if (tex->nrChannels >= 4)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->data);
                else if (tex->nrChannels == 3)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->width, tex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex->data);
                else if (tex->nrChannels == 1)
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->width, tex->height, 0, GL_RED, GL_UNSIGNED_BYTE, tex->data);
            } else if (tex->nrChannels >= 4)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->data);
            else if (tex->nrChannels == 3)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, tex->width, tex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex->data);
            else if (tex->nrChannels == 1)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, tex->width, tex->height, 0, GL_RED, GL_UNSIGNED_BYTE, tex->data);
            else
                return;

            // throw std::invalid_argument("Texture format not supported");

            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(tex->data);
            glBindTexture(GL_TEXTURE_2D, 0);

            std::string(textureType) == "texture_normal" ? image_textures.push_back(tex) : textures.push_back(tex);
        }
    }
    Texture::~Texture() {
        if (!tex)
            return;
        try {
            std::cout << tex << std::endl;
            if (--tex->sharing <= 0) {
                delete tex;
            }
        } catch (std::exception &e) {
            HYPER_LOG(e.what())
        }
    }
    Texture::Texture(unsigned char *m_Data, uint32_t slot,
                     const char *textureType, const char *texturePath) {
        stbi_set_flip_vertically_on_load(true);
        tex->texType = textureType;
        tex->texStarterPath = texturePath;
        tex->slot = slot;
        tex->texPath = std::string(texturePath);
        texPath = std::string(texturePath);
        tex->data = stbi_load_from_memory(tex->data, 0, &tex->width, &tex->height, &tex->nrChannels, 4);

        HYPER_LOG("Texture " + std::to_string(slot) + " loaded from " +
                  texturePath)

        glGenTextures(1, &tex->ID);
        glBindTexture(GL_TEXTURE_2D, tex->ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if (std::string(textureType) == "texture_normal") {
            if (tex->nrChannels >= 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->data);
            } else {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->width, tex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex->data);
            }
        } else if (std::string(textureType) == "texture_height") {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->data);
        } else if (tex->nrChannels >= 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->data);
        else if (tex->nrChannels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, tex->width, tex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex->data);
        else if (tex->nrChannels == 1)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, tex->width, tex->height, 0, GL_RED, GL_UNSIGNED_BYTE, tex->data);
        else
            return;

        // throw std::invalid_argument("Texture format not supported");

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(tex->data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Bind(uint32_t slot) {
        // #ifdef _WIN32
        //         if (IsBadReadPtr(text, sizeof(m_Texture)))
        //             return;
        // #else
        //         int nullfd = open("/dev/random", O_WRONLY);
        //         if (write(nullfd, tex, sizeof(m_Texture)) < 0) {
        //             return;
        //         }
        //         close(nullfd);
        // #endif
        if (tex) {
            if (slot == -1) {
                glActiveTexture(GL_TEXTURE0 + tex->slot);
                glBindTexture(GL_TEXTURE_2D, tex->ID);
            } else {
                glActiveTexture(GL_TEXTURE0 + slot);
                glBindTexture(GL_TEXTURE_2D, tex->ID);
            }
        }
    }

    void Texture::Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
} // namespace HyperAPI