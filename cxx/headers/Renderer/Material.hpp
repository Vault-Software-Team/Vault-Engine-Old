#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "Texture.hpp"

namespace HyperAPI {
    class DLL_API Material {
    public:
        std::vector<Texture> textures;

        Texture *diffuse = nullptr;
        Texture *specular = nullptr;
        Texture *normal = nullptr;
        Texture *height = nullptr;
        Texture *emission = nullptr;

        Vector4 baseColor;
        float shininess;
        float metallic;
        float bloom_threshold;
        bool dynamic_bloom;
        float roughness;
        Vector3 bloomColor = Vector3(0, 0, 0);
        Vector2 texUVs = Vector2(0, 0);

        Material(Vector4 baseColor = Vector4(1, 1, 1, 1),
                 std::vector<Texture> textures = {}, float shininess = 16,
                 float metallic = 0, float roughness = 0);

        ~Material() {
            textures.clear();
        }

        void Bind(Shader &shader);

        void Unbind(Shader &shader);
    };
} // namespace HyperAPI