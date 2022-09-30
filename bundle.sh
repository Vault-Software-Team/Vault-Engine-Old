PLATFORM=$1

if [ "$PLATFORM" = "windows" ]; then
    # check if dist/windows exists
    # if not, create it
    
    if [ -d dist/windows ]; then
        echo "dist/windows exists"
    else
        echo "dist/windows does not exist"
        mkdir dist/windows
    fi
    
    cp win_build.exe dist/windows/Static\ Engine.exe
    cp *.dll dist/windows/
    cp assets dist/windows/assets
    cp imgui.ini dist/windows/imgui.ini
    cp shaders dist/windows/shaders
    cp build dist/windows/build
else 
    if [ -d dist/linux ]; then
        echo "dist/linux exists"
    else
        echo "dist/linux does not exist"
        mkdir dist/linux
    fi

    cp -r lib dist/linux/lib
    cp -r build.out dist/linux/build.out
    cp -r Static\ Engine.sh dist/linux/Static\ Engine

    cp -r assets dist/linux/assets
    cp -r imgui.ini dist/linux/imgui.ini
    cp -r shaders dist/linux/shaders
    cp -r build dist/linux/build
fi