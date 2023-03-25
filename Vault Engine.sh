#!/bin/sh

cwd=$(pwd)
export LD_LIBRARY_PATH="$cwd/lib"
clear
# valgrind --leak-check=yes --log-file=valgrind.rpt ./build.out
gdb ./bin/build.out