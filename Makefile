exec = build.out
win_exec = win_build.exe
cwd=$(shell pwd)
sources = $(wildcard src/*.cpp)
sources += $(wildcard src/vendor/*/*.cpp)
sources += $(wildcard src/vendor/*/*/*.cpp)
sources += $(wildcard src/vendor/*/*/*/*.cpp)
sources += $(wildcard src/vendor/*/*/*.c)
sources += $(wildcard src/vendor/*/*/*/*.c)

sources += $(wildcard mono/*/*.cpp)
sources += $(wildcard mono/*/*/*.cpp)
sources += $(wildcard mono/*/*/*/*.cpp)
sources += $(wildcard mono/*/*/*.c)
sources += $(wildcard mono/*/*/*/*.c)
objects = $(sources:.cpp=.o)
api = $(wildcard src/api.cpp)
api += $(wildcard src/InputEvents.cpp)
api += $(wildcard src/scene.cpp)
api += $(wildcard src/ScriptEngine.cpp)

api_obj = $(api:.cpp=.o)

# relatively link shared libraries
# -L$(cwd)/lib
MONO_LIB=-I"$(cwd)/mono/include/mono-2.0" -D_REENTRANT  -L"$(cwd)/mono/lib" -lmono-2.0
flags = -fno-stack-protector -std=c++20 -lstdc++fs -g -L"./lib" -lluajit-5.1 -I"./src/vendor" -I"./src/vendor/bullet/bullet" -lfreetype -lGL -lbox2d -lGLU -lglfw -lm -lSDL2_mixer -lassimp -lXrandr -lXi -lbox2d -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor -lGLEW
win_flags = -L"./win_libs" -I"./src/vendor" -I"./src/vendor/bullet/bullet" -lglfw3dll -lstdc++fs -lluajit-5.1 -lbox2d -lassimp.dll

api:
	g++ -c $(api) $(flags)
	mv *.o src

scripts:
	g++ -c src/scripts.cpp $(flags)
	mv *.o src

app:
	g++ -c src/main.cpp $(flags)
	mv *.o src
	g++ $(objects) -o $(exec) $(flags)

bundle:
	mv src/*.o bin
linux: 
# compile to object files
	g++ -c $(sources) $(flags)
	mv *.o src
	g++ $(objects) -o $(exec) $(flags)
	mv src/*.o bin

$(exec): $(objects)
	g++ $(objects) $(flags) -o $(exec)
	make clean
	clear
	./Static\ Engine.sh

%.o: %.cpp lib/%.h
	g++ -c -I"./src/vendor" $(flags) $< -o $@

win:
# staticly link libassimp.a
	x86_64-w64-mingw32-g++ -static -g -std=c++20 $(sources) $(win_flags) -o $(win_exec)

clean:	
	-rm src/*.o
	-rm *.o

# mono:
# 	g++ -c $(flags) src/ScriptEngine/ScriptEngine.cpp -o src/ScriptEngine/ScriptEngine.o