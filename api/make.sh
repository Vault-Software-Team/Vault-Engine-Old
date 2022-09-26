TYPE=$1
cwd=$(pwd)

if [ "$TYPE" = "windows" ]; then
    FLAGS="-fno-stack-protector -std=c++20 -L\"../lib\" -lbox2d -lGL -lGLU -lglfw -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor -lGLEW `pkg-config --cflags --libs mono-2`"
    clear
    echo "Compiling..."
    x86_64-w64-mingw32-g++ -c -g $FLAGS api.cpp ScriptEngine.cpp InputEvents.cpp scene.cpp vendor/glad/src/glad.c vendor/glm/detail/glm.cpp vendor/imgui/imgui.cpp vendor/stb_image/stb_image.cpp -fpic
    echo "Making libhyper.so..."
    x86_64-w64-mingw32-g++ -shared -o libhyper.dll *.o $FLAGS
    cp ../bin/libhyper.so ../lib/
    # sudo cp ../bin/libhyper.so /usr/lib/
    echo "Compiled Successfully"
else 
    FLAGS="-fno-stack-protector -std=c++20 -L\"../lib\" -lfreetype -lbox2d -lGL -lGLU -lglfw -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor -lGLEW `pkg-config --cflags --libs mono-2`"
    clear
    echo "Compiling..."
    g++ -c -g $FLAGS api.cpp scripts.cpp ScriptEngine.cpp InputEvents.cpp scene.cpp vendor/glad/src/glad.c vendor/glm/detail/glm.cpp vendor/imgui/imgui.cpp vendor/stb_image/stb_image.cpp -fpic
    echo "Making libhyper.so..."
    g++ *.o -shared -o ../bin/libhyper.so
    cp ../bin/libhyper.so ../lib/
    # sudo cp ../bin/libhyper.so /usr/lib/
    echo "Compiled Successfully"
fi