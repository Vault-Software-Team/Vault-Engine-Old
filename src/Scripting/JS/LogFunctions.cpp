#include "LogFunctions.hpp"
#include <api.hpp>
#include <libs.hpp>

namespace HyperAPI {
    void LogInfo(const FunctionCallbackInfo<Value> &args) {
        HandleScope scope(args.GetIsolate());
        String::Utf8Value str(args.GetIsolate(), args[0]);
        std::string cstr = StaticHelpers::ToCString(str);
        HyperAPI::Log log(cstr, LOG_INFO);
    }

    void LogError(const FunctionCallbackInfo<Value> &args) {
        HandleScope scope(args.GetIsolate());
        String::Utf8Value str(args.GetIsolate(), args[0]);
        std::string cstr = StaticHelpers::ToCString(str);
        HyperAPI::Log log(cstr, LOG_ERROR);
    }

    void LogWarning(const FunctionCallbackInfo<Value> &args) {
        HandleScope scope(args.GetIsolate());
        String::Utf8Value str(args.GetIsolate(), args[0]);
        std::string cstr = StaticHelpers::ToCString(str);
        HyperAPI::Log log(cstr, LOG_WARNING);
    }
} // namespace HyperAPI