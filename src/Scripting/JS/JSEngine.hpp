#pragma once
#include <libs.hpp>
#include <Renderer/Log.hpp>

using v8::HandleScope;
namespace HyperAPI {
    class JSEngine : Lemon {
    public:
        JSEngine(char *argv[]);

        void Start();

        // private:
        // void RegisterFunction(void (*callback)(const FunctionCallbackInfo<Value> &));
    };
} // namespace HyperAPI