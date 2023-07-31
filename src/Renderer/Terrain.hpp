#pragma once
#include <dllapi.hpp>
#include "Base.hpp"
#include "Material.hpp"
#include <cstdint>
#include <libs.hpp>

namespace HyperAPI {
    class DLL_API Terrain {
    public:
        Mesh *mesh;
        Material material;

        Terrain(int div, float width);
    };
} // namespace HyperAPI