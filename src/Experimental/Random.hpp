#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "../Components/Exp_Base.hpp"
#include "../Renderer/Structures.hpp"
#include "../Renderer/OldStuff.hpp"

namespace HyperAPI::Experimental {
    class DLL_API Random {
    public:
        static void Init() { s_Engine.seed(std::random_device()()); }

        static float Float() {
            return (float)s_Distribution(s_Engine) /
                   (float)std::numeric_limits<uint32_t>::max();
        }

    private:
        static std::mt19937 s_Engine;
        static std::uniform_int_distribution<std::mt19937::result_type>
            s_Distribution;
    };
} // namespace HyperAPI::Experimental