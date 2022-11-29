#include <iostream>
#include "test.hpp"

extern "C" {
    Test *create_object() {
        return new Test();
    }
}

void Test::Start() {
    std::cout << "Loaded test.so" << std::endl;
}

void Test::Update() {
    GameObject *gameObject = f_GameObject::FindGameObjectByName("Player");
    std::cout << gameObject->ID << std::endl;
}