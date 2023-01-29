#include "Canvas.hpp"
#include <cstdint>

namespace VulkanUI {
    Canvas::Canvas(const int32_t &width, const int32_t &height) {
        this->width = width;
        this->heigth = height;
    }

    void Canvas::SetMousePosition(const int32_t &mouse_x, const int32_t &mouse_y) {
        this->mouse_x = mouse_x;
        this->mouse_y = mouse_y;
    }

    int32_t Canvas::GetMouseX() const {
        return mouse_x;
    }

    int32_t Canvas::GetMouseY() const {
        return mouse_y;
    }
} // namespace VulkanUI