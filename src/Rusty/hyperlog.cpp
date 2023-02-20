#include "hyperlog.hpp"
#include "libs.hpp"
#include "api.hpp"
#include <iostream>

void hyper_log(char *str) {
    HYPER_LOG(str)
}

void play_audio(char *file) {
    HyperAPI::AudioEngine::PlaySound(file);
}