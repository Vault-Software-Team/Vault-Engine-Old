#include "LogFunctions.hpp"

namespace HyperAPI::CsharpScriptEngine::Functions {
    void NativeLog(MonoString *text) {
        std::string str(mono_string_to_utf8(text));
        std::cout << str << std::endl;
    }

    void EditorLog(MonoString *text) {
        std::string str(mono_string_to_utf8(text));
        Log log(str, LOG_INFO);
    }

    void EditorWarning(MonoString *text) {
        std::string str(mono_string_to_utf8(text));
        Log log(str, LOG_WARNING);
    }

    void EditorError(MonoString *text) {
        std::string str(mono_string_to_utf8(text));
        Log log(str, LOG_ERROR);
    }

    void TestVector(glm::vec3 *parameter) {
        std::cout << parameter->x << " " << parameter->y << " " << parameter->z << std::endl;
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions