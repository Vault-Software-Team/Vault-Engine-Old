TYPE=$1
cwd=$(pwd)

if [ "$TYPE" = "windows" ]; then
    make win
    ./win_build.exe
elif [ "$TYPE" = "linux" ]; then
    make linux
elif [ "$TYPE" = "linux_game" ]; then
    make linux_game
elif [ "$TYPE" = "win_game" ]; then
    make win_game
    ./game.exe
elif [ "$TYPE" = "clean" ]; then
    make clean
fi