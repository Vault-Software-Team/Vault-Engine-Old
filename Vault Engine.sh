#!/bin/sh

cwd=$(pwd)
export LD_LIBRARY_PATH="$cwd/lib"
clear
# valgrind --leak-check=yes --log-file=valgrind.rpt ./build.out
./bin/build.out /home/koki1019/Desktop/Projects/Polyjam_Game
