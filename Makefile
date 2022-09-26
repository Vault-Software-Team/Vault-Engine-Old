exec = build.out
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
flags = -fno-stack-protector -std=c++20 -lstdc++fs -g -L"./lib" -lluajit-5.1 -lfreetype -lGL -lbox2d -lGLU -lglfw -lm -lSDL2_mixer -lassimp -lXrandr -lXi -lbox2d -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor -lGLEW -lhyper


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
	i686-w64-mingw32-g++ $(sources) $(flags) -o $(exec).exe

clean:	
	-rm src/*.o
	-rm *.o

# mono:
# 	g++ -c $(flags) src/ScriptEngine/ScriptEngine.cpp -o src/ScriptEngine/ScriptEngine.o