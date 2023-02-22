#pragma once
#include <cstdint>
#include <libs.hpp>

namespace VulkanUI {
    class Canvas {
    private:
        int32_t width;
        int32_t heigth;
        int32_t mouse_x;
        int32_t mouse_y;

    public:
        Canvas(const int32_t &width, const int32_t &height);

        void SetMousePosition(const int32_t &mouse_x, const int32_t &mouse_y);
        int32_t GetMouseX() const;
        int32_t GetMouseY() const;
    };
} // namespace VulkanUI