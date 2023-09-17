#!/bin/sh

cwd=$(pwd)
export LD_LIBRARY_PATH="$cwd/lib"
clear
# valgrind --leak-check=yes --log-file=valgrind.rpt ./build.out
# MANGOHUD_CONFIG="cpu_temp,gpu_temp,core_load,engine_version,ram,vram" mangohud --dlsym ./bin/game.out
# ./bin/build.out /home/koki1019/Desktop/Projects/Polyjam_Game
# gdb --args ./bin/build.out "/home/koki1019/.vault_projects/NPC"
# gdb ./bin/game.out
# ./bin/build_proj.out
gdb --args ./bin/build.out /home/koki1019/Desktop/VaultProjects/flappy_bird
# ./bin/build.out /home/koki1019/Desktop/Projects/A_A_JotnarEngine 