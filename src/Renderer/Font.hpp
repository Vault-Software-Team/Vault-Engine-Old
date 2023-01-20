#pragma once
#include "Shader.hpp"
#include <libs.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace HyperAPI {
    extern FT_Library ft;
    extern Shader *font_shader;

    class Font {
    public:
        static int InitFT();

        uint32_t VAO, VBO;
        std::string font_path;

        struct Character {
            uint32_t TextureID; // ID handle of the glyph texture
            glm::ivec2 Size;    // Size of glyph
            glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
            uint32_t Advance;   // Offset to advance to next glyph
        };
        std::map<char, Character> Characters;

        Font(const char *font, uint32_t scale);
        void Draw(Camera camera, const glm::mat4 &model, const std::string &text, Vector3 color, Vector3 bloomColor, float x, float y, float scale, float y_offset);
    };
} // namespace HyperAPI
