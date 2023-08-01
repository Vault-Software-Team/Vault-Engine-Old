#pragma once
#include <dllapi.hpp>
#include "Widget.hpp"
#include <cstdint>
#include <libs.hpp>

namespace VulkanUI {
    class DLL_API Button : public Widget {
    private:
        std::string label;
        int32_t width;
        int32_t height;

    public:
        Button(const std::string &label, const int32_t &width, const int32_t &height);
        void ChangeLabel(const std::string &label);
        void ChangeSize(const int32_t &width, const int32_t &height);
        int32_t GetWidth() const;
        int32_t GetHeight() const;
    };
} // namespace VulkanUI