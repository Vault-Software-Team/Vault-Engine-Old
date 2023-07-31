#pragma once
#include <api.hpp>

using namespace HyperAPI;
using namespace HyperAPI::Experimental;

#ifdef _WIN32
#define VAULT_API __declspec(dllexport)
#else
#define VAULT_API
#endif

class NewScript : CppScripting::Script {
public:
    NewScript() = default;
    ~NewScript() = default;

    void Start() override;
    void Update() override;
};
extern "C" {
VAULT_API void set_api(std::vector<Experimental::GameObject *> *g);
VAULT_API NewScript *create_object();
}
