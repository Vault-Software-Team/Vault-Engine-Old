exec = ../g_build.out
win_exec = g_win_build.exe
cwd=$(shell pwd)
sources = $(wildcard *.cpp)
sources += $(wildcard vendor/*/*.cpp)
sources += $(wildcard vendor/*/*/*.cpp)
sources += $(wildcard vendor/*/*/*/*.cpp)
sources += $(wildcard vendor/*/*/*.c)
sources += $(wildcard vendor/*/*/*/*.c)

sources += $(wildcard mono/*/*.cpp)
sources += $(wildcard mono/*/*/*.cpp)
sources += $(wildcard mono/*/*/*/*.cpp)
sources += $(wildcard mono/*/*/*.c)
sources += $(wildcard mono/*/*/*/*.c)
sources += $(wildcard ../src/*.cpp)
# remove main.cpp from sources
sources := $(filter-out ../src/main.cpp,$(sources))
objects = $(sources:.cpp=.o)
# remove main.o from objects
objects := $(filter-out ../src/main.o,$(objects))

# relatively link shared libraries
# -L$(cwd)/lib
MONO_LIB=-I"$(cwd)/mono/include/mono-2.0" -D_REENTRANT  -L"$(cwd)/mono/lib" -lmono-2.0
flags = -fno-stack-protector -std=c++20 -lstdc++fs -g -L"../lib" -lluajit-5.1 -lfreetype -lGL -lbox2d -lGLU -lglfw -lm -lSDL2_mixer -lassimp -lXrandr -lXi -lbox2d -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor -lGLEW
win_flags = -L"../win_libs" -lglfw3dll -lstdc++fs -lluajit-5.1 -lbox2d -lassimp.dll


$(exec): $(objects)
	g++ $(objects) $(flags) -o $(exec)
	# clear

%.o: %.cpp ../src/lib/%.h
	g++ -c $(flags) $< -o $@

install:
	make
	cp ./hello.out /usr/local/bin/hello


win:
# staticly link libassimp.a
	x86_64-w64-mingw32-g++ -static -g -std=c++20 $(sources) $(win_flags) -o $(win_exec)

clean:	
	-rm *.o
	-rm *.o

# mono:
# 	g++ -c $(flags) ScriptEngine/ScriptEngine.cpp -o ScriptEngine/ScriptEngine.o