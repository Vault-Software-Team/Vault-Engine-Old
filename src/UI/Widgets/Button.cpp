#include "Button.hpp"
#include <cstdint>

namespace VulkanUI {
    Button::Button(const std::string &label, const int32_t &width, const int32_t &height) {
        this->label = label;
        this->width = width;
        this->height = height;
    }

    void Button::ChangeLabel(const std::string &label) {
        this->label = label;
    }

    void Button::ChangeSize(const int32_t &width, const int32_t &height) {
        this->width = width;
        this->height = height;
    }

    int32_t Button::GetWidth() const {
        return width;
    }

    int32_t Button::GetHeight() const {
        return height;
    }
} // namespace VulkanUI