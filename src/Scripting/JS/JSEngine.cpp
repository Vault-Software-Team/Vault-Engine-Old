#include "JSEngine.hpp"

#include "LogFunctions.hpp"

namespace HyperAPI {
    JSEngine::JSEngine(char *argv[]) : Lemon() {
        this->CreatePlatform(argv);
        this->CreateVM();

        {

            Isolate::Scope isolatescope(this->GetIsolate());
            HandleScope handlescope(this->GetIsolate());
            this->CreateGlobalEnvironment();
            this->SetupEngineEnvironment();
            this->SetupEnvironment();

            this->Start();
        }

        // Shutdown
        this->ShutdownVM();
    }

    void JSEngine::Start() {
        Local<Context> context = this->CreateLocalContext();
        Context::Scope contextscope(context);

        // Debug Scope
        CreateGlobalObject("Debug").SetPropertyMethod("Log", LogInfo).Register();
        CreateGlobalObject("Debug").SetPropertyMethod("Error", LogError).Register();
        CreateGlobalObject("Debug").SetPropertyMethod("Warning", LogWarning).Register();

        this->RunJsFromFile("test.js");
    }
} // namespace HyperAPI