exec = build.out
exec_game = game.out
exec_win_game = game.exe
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
api += $(wildcard src/networking.cpp)
scripts = $(wildcard src/scripts/*.cpp)

api_obj = $(api:.cpp=.o)

# relatively link shared libraries
# -L$(cwd)/lib
MONO_LIB=-I"$(cwd)/mono/include/mono-2.0" -D_REENTRANT  -L"$(cwd)/mono/lib" -lmono-2.0
bullet_physics_linker_flags = -lBulletDynamics -lBulletCollision -lLinearMath
bullet_physics_linker_flags_windows = -lBulletDynamics.dll -lBulletCollision.dll -lLinearMath.dll
flags = -fno-stack-protector -std=c++20 -lstdc++fs -g -L"./lib" -lluajit-5.1 -I"./src/vendor" -I"./src/vendor/bullet/bullet" -lfreetype -lGL -lbox2d -lGLU -lglfw -lm -lSDL2_mixer -lassimp -ltinyxml2 -lXrandr -lXi -lbox2d -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor -lGLEW -ldiscord-rpc $(bullet_physics_linker_flags)
win_flags = -L"./win_libs" -I"./src/lib" -I"./src/vendor" -I"./src/vendor/bullet/bullet" -lglfw3dll -lstdc++fs -lluajit-5.1 -lbox2d -lassimp.dll -lfreetype -lSDL2.dll -lSDL2_mixer.dll -ldiscord-rpc -ltinyxml2 $(bullet_physics_linker_flags_windows)

all:
	g++ $(flags) $(sources) -o $(exec)

eng:
# set the .o files with objects variable
	g++ -c $(api) src/scripts.cpp  $(flags)
	mv *.o src

libs:
	g++ -c $(sources) $(flags)

scripts:
# DO NOT COMPILE SCRIPTS THIS WAS JUST A TEST
	for i in $(scripts); do \
		j=$${i/.cpp/.o}; \
		g++ -c -fPIC $$i $(flags) -o $$j; \
		g=$${j/.o/.so}; \
		g++ -shared $$j src/api.o -o $$g; \
	done
app:
	g++ -c src/main.cpp $(flags)
	mv *.o src
	g++ $(objects) -o $(exec) $(flags)

bundle:
	mv src/*.o bin
linux:
	make eng
	make app

linux_game:
	g++ -c $(api) $(flags)
	mv *.o src

	g++ -c src/main.cpp -DGAME_BUILD $(flags)
	mv *.o src

	g++ $(objects) -o $(exec_game) -DGAME_BUILD $(flags)
	# ./LaunchGame.sh
# compile to object files
# g++ -c $(sources) $(flags)
# mv *.o src
# g++ $(objects) -o $(exec) $(flags)
# mv src/*.o bin

$(exec): $(objects)
	g++ $(objects) $(flags) -o $(exec)
	make clean
	clear

%.o: %.cpp lib/%.h
	g++ -c -I"./src/vendor" $(flags) $< -o $@

win:
	x86_64-w64-mingw32-g++ -static -g -std=c++20 $(sources) $(win_flags) -o $(win_exec)

win_game:
	x86_64-w64-mingw32-g++ -static -g -std=c++20 $(sources) -DGAME_BUILD $(win_flags) -o $(exec_win_game)

clean:	
	-rm src/*.o
	-rm *.o

# mono:
# 	g++ -c $(flags) src/ScriptEngine/ScriptEngine.cpp -o src/ScriptEngine/ScriptEngine.o