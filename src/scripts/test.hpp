//
// Created by koki1019 on 23/10/2022.
//

#ifndef VAULT_ENGINE_TEST_HPP
#define VAULT_ENGINE_TEST_HPP
#include "../lib/api.hpp"

using namespace HyperAPI;
using namespace CppScripting;
using namespace Experimental;

class Test : public Script {
public:
    void Start() override;
    void Update() override;
};

extern "C" {
    Test *create_object();
}

#endif //VAULT_ENGINE_TEST_HPP
