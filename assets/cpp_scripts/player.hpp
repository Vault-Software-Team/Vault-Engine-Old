//
// Created by koki1019 on 23/10/2022.
//

#ifndef COLOR_CHANGE_SCRIPT_HPP
#define COLOR_CHANGE_SCRIPT_HPP
#include "../../src/lib/api.hpp"

using namespace HyperAPI;
using namespace CppScripting;
using namespace Experimental;

class Player : public Script {
public:
    float speed = 500;

    void Start() override;
    void Update() override;
};

extern "C" {
    Player *create_object();
}

#endif //COLOR_CHANGE_SCRIPT_HPP
