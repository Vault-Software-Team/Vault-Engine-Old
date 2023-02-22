#pragma once
#include <libs.hpp>

namespace HyperAPI {
    class Shader {
    public:
        uint32_t ID;
        std::string path;

        Shader(const char *shaderPath,
               const std::string &shaderContent = "NO_CONTENT");

        void Bind();

        void Unbind();

        // set uniforms
        void SetUniform1i(const char *name, int value);

        void SetUniform1f(const char *name, float value);

        void SetUniform2f(const char *name, float v0, float v1);

        void SetUniform3f(const char *name, float v0, float v1, float v2);

        void SetUniform4f(const char *name, float v0, float v1, float v2,
                          float v3);

        void SetUniformMat4(const char *name, glm::mat4 value);
        // uint
        void SetUniform1ui(const char *name, uint32_t value);
    };
} // namespace HyperAPI