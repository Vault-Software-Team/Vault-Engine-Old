#pragma once
#include "Base.hpp"
#include "Material.hpp"
#include <cstdint>
#include <libs.hpp>

namespace HyperAPI {
    class Terrain {
    public:
        Mesh *mesh;
        Material material;

        Terrain();
    };
} // namespace HyperAPI