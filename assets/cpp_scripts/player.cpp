#include <iostream>
#include "player.hpp"

extern "C" {
    Player *create_object() {
        return new Player();
    }
}

void Player::Start() {
}

void Player::Update() {
    auto *gameObject = f_GameObject::FindGameObjectByID(objId);
    auto &rigidbody3D = gameObject->GetComponent<Rigidbody3D>();
    auto &transform = gameObject->GetComponent<Transform>();

    float X = 0;
    float Z = 0;
    static bool pressedA;

    if(Input::IsKeyPressed(KEY_D)) {
        X = speed * Timestep::deltaTime;
        pressedA = false;
    }

    if(Input::IsKeyPressed(KEY_A)) {
        X = -(speed * Timestep::deltaTime);
        pressedA = true;
    }

    if(Input::IsKeyPressed(KEY_W)) {
        Z = -(speed * Timestep::deltaTime);
    }

    if(Input::IsKeyPressed(KEY_S)) {
        Z = speed * Timestep::deltaTime;
    }

    if(pressedA) {
        transform.scale.x = Hyper::LerpFloat(transform.scale.x, -2, 15 * Timestep::deltaTime);
    } else {
        transform.scale.x = Hyper::LerpFloat(transform.scale.x, 2, 15 * Timestep::deltaTime);
    }

    rigidbody3D.SetVelocity(Vector3(X, rigidbody3D.GetVelocity().y, Z));
}