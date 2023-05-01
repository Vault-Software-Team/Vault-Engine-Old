#!/bin/sh

cwd=$(pwd)
export LD_LIBRARY_PATH="$cwd/lib"
clear
# valgrind --leak-check=yes --log-file=valgrind.rpt ./build.out
# MANGOHUD_CONFIG="cpu_temp,gpu_temp,core_load,engine_version,ram,vram" mangohud --dlsym ./bin/build.out /home/koki1019/Desktop/Projects/Polyjam_Game
# ./bin/build.out /home/koki1019/Desktop/Projects/Polyjam_Game
./bin/build.out
# ./bin/build.out