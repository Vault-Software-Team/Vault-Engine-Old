#include "Font.hpp"
#include "Camera.hpp"
#include "freetype/freetype.h"

namespace HyperAPI {
    DLL_API FT_Library ft;
    DLL_API Shader *font_shader;

    int Font::InitFT() {
        font_shader = new Shader("shaders/text_shader.glsl");
        if (FT_Init_FreeType(&ft)) {
            HYPER_LOG("ERROR: Couldn't init FreeType Library")
            return -1;
        }
        return 0;
    }

    Font::Font(const char *font, uint32_t scale) : font_path(font) {
        FT_Face face;
        if (FT_New_Face(ft, font, 0, &face)) {
            HYPER_LOG("ERROR: Failed to load font " + std::string(font))
        }

        FT_Set_Pixel_Sizes(face, 0, scale);

        if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
            HYPER_LOG("ERROR: Failed to load Glyph")
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (unsigned char c = 0; c < 128; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                HYPER_LOG("ERROR: Failed to load Glyph")
                continue;
            }
            // generate texture
            uint32_t texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                (uint32_t)face->glyph->advance.x};
            Characters.insert(std::pair<char, Character>(c, character));
        }

        FT_Done_Face(face);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Font::Draw(Camera camera, const glm::mat4 &model, const std::string &text, Vector3 color, Vector3 bloomColor, float x, float y, float scale, float y_offset) {
        float saved_x = x, saved_y = y;
        glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

        font_shader->Bind();
        camera.Matrix(*font_shader, "camera");
        font_shader->SetUniformMat4("model", model);
        font_shader->SetUniform3f("bloomColor", bloomColor.x, bloomColor.y, bloomColor.z);
        font_shader->SetUniform3f("textColor", color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        // iterate through all characters
        std::string::const_iterator c;
        float newLine;
        for (c = text.begin(); c != text.end(); c++) {
            Character ch = Characters[*c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
            ypos += newLine;
            if (*c == '\n') {
                x = saved_x;
                newLine -= (ch.Bearing.y * scale) + y_offset;
                continue;
            }

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            float vertices[6][4] = {
                {xpos, ypos + h, 0.0f, 0.0f},
                {xpos, ypos, 0.0f, 1.0f},
                {xpos + w, ypos, 1.0f, 1.0f},

                {xpos, ypos + h, 0.0f, 0.0f},
                {xpos + w, ypos, 1.0f, 1.0f},
                {xpos + w, ypos + h, 1.0f, 0.0f}};

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDepthFunc(GL_LESS);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (ch.Advance >> 6) * scale;
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glDepthFunc(GL_LEQUAL);
    }
} // namespace HyperAPI