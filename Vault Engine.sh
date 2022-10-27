#!/bin/sh

cwd=$(pwd)
export LD_LIBRARY_PATH="$cwd/lib"
clear
./build_proj.out
