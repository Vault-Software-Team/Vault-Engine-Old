#pragma once
#include <cstdint>
#include <libs.hpp>

namespace VulkanUI {
    class Widget {
    protected:
        std::string name;
        std::string ID;
    };
} // namespace VulkanUI