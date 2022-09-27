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

# relatively link shared libraries
# -L$(cwd)/lib
MONO_LIB=-I"$(cwd)/mono/include/mono-2.0" -D_REENTRANT  -L"$(cwd)/mono/lib" -lmono-2.0
flags = -fno-stack-protector -std=c++20 -lstdc++fs -g -L"./lib" -lluajit-5.1 -lfreetype -lGL -lbox2d -lGLU -lglfw -lm -lSDL2_mixer -lassimp -lXrandr -lXi -lbox2d -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor -lGLEW
win_flags = -L"./win_libs" -lglfw3dll -lstdc++fs -lluajit-5.1 -lbox2d -lassimp.dll


$(exec): $(objects)
	g++ $(objects) $(flags) -o $(exec)
	make clean
	clear
	./Static\ Engine.sh

%.o: %.cpp lib/%.h
	g++ -c $(flags) $< -o $@

install:
	make
	cp ./hello.out /usr/local/bin/hello


win:
# staticly link libassimp.a
	x86_64-w64-mingw32-g++ -static -g -std=c++20 $(sources) $(win_flags) -o $(win_exec)

clean:	
	-rm src/*.o
	-rm *.o

# mono:
# 	g++ -c $(flags) src/ScriptEngine/ScriptEngine.cpp -o src/ScriptEngine/ScriptEngine.o