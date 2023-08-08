#include "Splash.hpp"
#include <libs.hpp>
#include <api.hpp>
// #include <Components/GameObject.hpp>

using namespace HyperAPI;
using namespace HyperAPI::Experimental;

SplashScreen::SplashScreen(const char *image, glm::vec2 scale, float to_last, float fade_in, float fade_out) {
    this->fade_in = fade_in;
    this->fade_out = fade_out;
    this->to_last = to_last;

    instance = this;
    img = new Texture(image, 0, "texture_diffuse");

    splashObject = new GameObject();
    cameraObject = new GameObject();
    splashObject->AddComponent<Transform>();
    cameraObject->AddComponent<Transform>();
    Scene::m_GameObjects->push_back(splashObject);
    Scene::m_GameObjects->push_back(cameraObject);

    // Splash Object
    auto &transform = splashObject->GetComponent<Transform>();
    splashObject->AddComponent<SpriteRenderer>();

    spriteRenderer = &splashObject->GetComponent<SpriteRenderer>();

    transform.position.y = -500;
    transform.position.z = -5;
    transform.scale = glm::vec3(scale.x, scale.y, 1);
    spriteRenderer->mesh->material.diffuse = img;
    spriteRenderer->mesh->material.baseColor.a = 0;

    // Camera Object
    cameraObject->AddComponent<CameraComponent>();

    auto &camera = cameraObject->GetComponent<CameraComponent>();
    camera.camera->layers.push_back("Default");
    auto &cam_transform = cameraObject->GetComponent<Transform>();

    cam_transform.position.y = -500;
    cam_transform.scale = glm::vec3(1, 1, 1);
}

bool SplashScreen::Play() {
    static bool switchFade = false;
    static bool finished = false;
    if (!switchFade) {
        spriteRenderer->mesh->material.baseColor.a = Hyper::LerpFloat(spriteRenderer->mesh->material.baseColor.a, 1, fade_in * Timestep::deltaTime);
    }

    if (spriteRenderer->mesh->material.baseColor.a >= 0.9)
        switchFade = true;

    if (switchFade) {
        timer += Timestep::deltaTime;

        if (timer >= to_last) {
            spriteRenderer->mesh->material.baseColor.a = Hyper::LerpFloat(spriteRenderer->mesh->material.baseColor.a, 0, fade_out * Timestep::deltaTime);
            if (spriteRenderer->mesh->material.baseColor.a <= 0.1 && !finished) {
                finished = true;
                HYPER_LOG("Finished Splashscreen Animation")
                Scene::mainCamera = Scene::scene_camera;
                splashObject->DeleteGameObject();
                cameraObject->DeleteGameObject();
                delete splashObject;
                delete cameraObject;
                return true;
            }
        }
    }
    return false;
}