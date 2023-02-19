#pragma once
#ifdef RUSTY_BUILD
extern "C" {
int cpp_play_audio(char *audio_file);
}

extern "C" int rusty_play_audio(char *s);
#endif
