#pragma once
#include <libs.hpp>
#include "Exp_Base.hpp"
#include "imgui/imgui.h"

namespace HyperAPI::Experimental {
    struct Bloom : public BaseComponent {
        Vector3 bloomColor;
        bool dynamic_bloom;
        float bloom_threshold;

        Bloom() = default;

        void GUI() override;
    };
} // namespace HyperAPI::Experimental