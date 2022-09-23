exec = ../g_build.out
cwd=$(shell pwd)
sources = $(wildcard *.cpp)
sources += $(wildcard vendor/*/*.cpp)
sources += $(wildcard vendor/*/*/*.cpp)
sources += $(wildcard vendor/*/*/*/*.cpp)
sources += $(wildcard vendor/*/*/*.c)
sources += $(wildcard vendor/*/*/*/*.c)
objects = $(sources:.cpp=.o)

flags = -fno-stack-protector -std=c++20 -lstdc++fs -g -L"../lib" -lluajit-5.1 -lGL -lGLU -lglfw -ltinyxml2 -lm -lSDL2_mixer -lassimp -lXrandr -lXi -lbox2d -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor -lGLEW -lhyper


$(exec): $(objects)
	g++ $(objects) $(flags) -o $(exec)
	make clean
	clear

%.o: %.cpp lib/%.h
	g++ -c $(flags) $< -o $@

install:
	make
	cp ./hello.out /usr/local/bin/hello

clean:	
	-rm *.o
	-rm *.o

# mono:
# 	g++ -c $(flags) ScriptEngine/ScriptEngine.cpp -o ScriptEngine/ScriptEngine.o