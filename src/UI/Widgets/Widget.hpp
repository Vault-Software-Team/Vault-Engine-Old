#pragma once
#include <dllapi.hpp>
#include <cstdint>
#include <libs.hpp>

namespace VulkanUI {
    class DLL_API Widget {
    protected:
        std::string name;
        std::string ID;
    };
} // namespace VulkanUI