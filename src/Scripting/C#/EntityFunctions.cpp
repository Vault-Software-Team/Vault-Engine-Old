#include "EntityFunctions.hpp"

namespace HyperAPI::CsharpScriptEngine::Functions {
    void Entity_GetID(MonoString **result) {
        *result = mono_string_new(CsharpVariables::appDomain, nextId.c_str());
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions