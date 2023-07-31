#pragma once
#include <dllapi.hpp>

DLL_API extern "C" {
    // To rusty vault
    void hyper_log(char *str);
    void play_audio(char *file);

    // From rusty vault
    int rusty_play_audio(char *audio_file);
}