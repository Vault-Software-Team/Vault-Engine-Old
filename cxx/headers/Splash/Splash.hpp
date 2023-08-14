#pragma once
#include <dllapi.hpp>
#include <Renderer/Texture.hpp>
#include <Components/SpriteRenderer.hpp>

DLL_API class SplashScreen {
public:
    static inline SplashScreen *instance;

public:
    HyperAPI::Texture *img;
    HyperAPI::Experimental::GameObject *splashObject;
    HyperAPI::Experimental::GameObject *cameraObject;
    HyperAPI::Experimental::SpriteRenderer *spriteRenderer;
    float fade_in;
    float fade_out;
    float to_last;

    float timer;

    SplashScreen(const char *image, Vector2 scale = Vector2(6.0f, 3.40f), float to_last = 3, float fade_in = 1, float fade_out = 1);
    bool Play();
};