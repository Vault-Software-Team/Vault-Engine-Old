FLAGS="-fno-stack-protector -std=c++20 -lGL -lGLU -lglfw -lm -lXrandr -lXi -ltinyxml2 -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor -lGLEW `pkg-config --cflags --libs mono-2`"
clear
echo "Compiling..."
g++ -c -g $FLAGS api.cpp ScriptEngine.cpp scene.cpp vendor/glad/src/glad.c vendor/glm/detail/glm.cpp vendor/imgui/imgui.cpp vendor/stb_image/stb_image.cpp -fpic
echo "Making libhyper.so..."
g++ *.o -shared -o ../bin/libhyper.so
cp ../bin/libhyper.so ../lib/
# sudo cp ../bin/libhyper.so /usr/lib/
echo "Compiled Successfully"