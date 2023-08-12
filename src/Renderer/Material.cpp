#include "Material.hpp"
#include "Shader.hpp"

namespace HyperAPI {
    Material::Material(Vector4 baseColor, std::vector<Texture> textures,
                       float shininess, float metallic, float roughness) {
        this->baseColor = baseColor;
        this->shininess = shininess;
        this->metallic = metallic;
        this->textures = textures;
        this->roughness = roughness;

        for (auto &tex : textures) {
            if (std::string(tex.tex->texType) == "texture_diffuse") {
                this->diffuse = &tex;
            } else if (std::string(tex.tex->texType) == "texture_specular") {
                this->specular = &tex;
            } else if (std::string(tex.tex->texType) == "texture_normal") {
                this->normal = &tex;
            }
        }

        if (diffuse != nullptr) {
            std::cout << "Diffuse: " << diffuse->tex->texType << std::endl;
        }
    }

    void Material::Bind(Shader &shader) {
        // uint32_t diffuse = 0;
        // uint32_t specular = 0;
        // uint32_t normal = 0;

        shader.Bind();

        shader.SetUniform4f("baseColor", baseColor.x, baseColor.y, baseColor.z,
                            baseColor.w);
        shader.SetUniform1f("shininess", shininess);
        shader.SetUniform1f("metallic", metallic);
        shader.SetUniform1f("roughness", roughness);
        shader.SetUniform1f("bloom_threshold", bloom_threshold);
        shader.SetUniform1i("dynamic_bloom", dynamic_bloom);
        shader.SetUniform2f("texUvOffset", texUVs.x, texUVs.y);
        shader.SetUniform3f("u_BloomColor", bloomColor.x, bloomColor.y,
                            bloomColor.z);
        const uint32_t noEquippedSlot = 5;

        if (diffuse != nullptr) {
            diffuse->Bind(0);
            std::cout << diffuse->texPath << "\n";

            shader.SetUniform1i("isTex", 1);
            shader.SetUniform1i("texture_diffuse0", 0);
        } else {
            shader.SetUniform1i("isTex", 0);
        }

        if (specular != nullptr) {
            specular->Bind(1);
            shader.SetUniform1i("texture_specular0", 1);
        } else {
            shader.SetUniform1i("texture_specular0", noEquippedSlot);
        }

        if (normal != nullptr) {
            normal->Bind(2);
            shader.SetUniform1i("texture_normal0", 2);
            shader.SetUniform1i("hasNormalMap", 1);
        } else {
            shader.SetUniform1i("texture_normal0", noEquippedSlot);
            shader.SetUniform1i("hasNormalMap", 0);
        }

        if (height != nullptr) {
            height->Bind(3);
            shader.SetUniform1i("texture_height0", 3);
            shader.SetUniform1i("hasHeightMap", 1);
        } else {
            shader.SetUniform1i("texture_height0", noEquippedSlot);
            shader.SetUniform1i("hasHeightMap", 0);
        }

        if (emission != nullptr) {
            emission->Bind(4);
            shader.SetUniform1i("texture_emission0", 4);
        } else {
            shader.SetUniform1i("texture_emission0", noEquippedSlot);
        }
    }

    void Material::Unbind(Shader &shader) {
        if (diffuse != nullptr)
            diffuse->Unbind();

        if (specular != nullptr)
            specular->Unbind();

        if (normal != nullptr)
            normal->Unbind();

        shader.Unbind();
    }
} // namespace HyperAPI