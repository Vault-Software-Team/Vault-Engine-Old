#pragma once
#include <dllapi.hpp>
#include <libs.hpp>

namespace Debugging {
    GLenum glCheckError_(const char *file, int line);
} // namespace Debugging
#define glCheckError() Debugging::glCheckError_(__FILE__, __LINE__)