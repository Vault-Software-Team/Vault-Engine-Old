#pragma once
#include <libs.hpp>

namespace HyperAPI {
    bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &translation,
                            glm::vec3 &rotation, glm::vec3 &scale);
}