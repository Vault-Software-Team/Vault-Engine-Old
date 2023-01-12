#pragma once
#include <libs.hpp>
#include "Texture.hpp"

namespace HyperAPI {
    class Material {
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
        float roughness;
        Vector3 bloomColor = Vector3(0, 0, 0);
        Vector2 texUVs = Vector2(0, 0);

        Material(Vector4 baseColor = Vector4(1, 1, 1, 1),
                 std::vector<Texture> textures = {}, float shininess = 0,
                 float metallic = 0, float roughness = 0);

        ~Material() {
            textures.clear();

            if (diffuse != nullptr) {
                delete diffuse;
            }

            if (specular != nullptr) {
                delete specular;
            }

            if (normal != nullptr) {
                delete normal;
            }

            if (height != nullptr) {
                delete height;
            }

            if (emission != nullptr) {
                delete emission;
            }
        }

        void Bind(Shader &shader);

        void Unbind(Shader &shader);
    };
} // namespace HyperAPI