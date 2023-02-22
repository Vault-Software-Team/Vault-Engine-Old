
#pragma once
#include <api.hpp>

using namespace HyperAPI;
using namespace HyperAPI::Experimental;

#ifdef _WIN32
#ifdef BUILD_DLL
#define VAULT_API __declspec(dllexport)
#else
#define VAULT_API __declspec(dllimport)
#endif
#else
#define VAULT_API
#endif

extern "C" {
class NewScript : CppScripting::Script {
public:
    NewScript() = default;
    ~NewScript() = default;

    void Start() override;
    void Update() override;
};

VAULT_API NewScript *create_object();
}
                            