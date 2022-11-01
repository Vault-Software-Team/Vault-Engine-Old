# SET THESE VARIABLES TO MATCH YOUR ENVIRONMENT
MINGW_COMPILER = x86_64-w64-mingw32-g++
GNU_LINUX_COMPILER = g++
# SET THESE VARIABLES TO MATCH YOUR ENVIRONMENT

exec = build.out
exec_game = game.out
exec_win_game = windows/game.exe
win_exec = windows/win_build.exe

exec_proj = build_proj.out
exec_win_proj = windows/win_proj.exe

cwd=$(shell pwd)
sources = $(wildcard src/*.cpp)
sources += $(wildcard src/vendor/*/*.cpp)
sources += $(wildcard src/vendor/*/*/*.cpp)
sources += $(wildcard src/vendor/*/*/*/*.cpp)
sources += $(wildcard src/vendor/*/*/*.c)
sources += $(wildcard src/vendor/*/*/*/*.c)

objects = $(sources:.cpp=.o)
api = $(wildcard src/api.cpp)
api += $(wildcard src/InputEvents.cpp)
api += $(wildcard src/scene.cpp)
api += $(wildcard src/ScriptEngine.cpp)
api += $(wildcard src/networking.cpp)
api += $(wildcard src/nativeScripts.cpp)
scripts = $(wildcard src/scripts/*.cpp)

api_obj = $(api:.cpp=.o)

# relatively link shared libraries
# -L$(cwd)/lib
MONO_LIB=-I"$(cwd)/mono/include/mono-2.0" -D_REENTRANT  -L"$(cwd)/mono/lib" -lmono-2.0
bullet_physics_linker_flags = -lBulletDynamics -lBulletCollision -lLinearMath
bullet_physics_linker_flags_windows = -lBulletDynamics.dll -lBulletCollision.dll -lLinearMath.dll
flags = -fno-stack-protector -std=c++20 -lstdc++fs -g -L"./lib" -lluajit-5.1 -I"./src/vendor" -I"./src/vendor/bullet/bullet" -I"./src/vendor/NoesisGUI" -lbacktrace -lfreetype -lGL -lbox2d -lGLU -lglfw -lm -lSDL2_mixer -lassimp -ltinyxml2 -lXrandr -lXi -lbox2d -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lzlib -lboost_stacktrace_basic -lXcursor -lGLEW -ldiscord-rpc $(bullet_physics_linker_flags)
win_flags = -lstdc++fs -L"./win_libs" -I"./src/lib" -I"./src/vendor/NoesisGUI" -I"./src/vendor" -I"./src/vendor/bullet/bullet" -lglfw3dll -lstdc++fs -lluajit-5.1 -lbox2d -lassimp.dll -lfreetype -lSDL2.dll -lSDL2_mixer.dll -ldiscord-rpc -ltinyxml2 $(bullet_physics_linker_flags_windows)

all:
	$(GNU_LINUX_COMPILER) $(sources) src/api.cpp -o $(exec) $(flags)

eng:
# set the .o files with objects variable
	$(GNU_LINUX_COMPILER) -c $(api) src/scripts.cpp  $(flags)
	mv *.o bin

libs:
	for i in $(sources); do \
		j=$${i/.cpp/.o}; \
		$(GNU_LINUX_COMPILER) -c $$i $(flags); \
		mv *.o bin; \
	done

scripts:
# DO NOT COMPILE SCRIPTS THIS WAS JUST A TEST
	for i in $(scripts); do \
		j=$${i/.cpp/.o}; \
		$(GNU_LINUX_COMPILER) -c -fPIC $$i $(flags) -o $$j; \
		g=$${j/.o/.so}; \
		$(GNU_LINUX_COMPILER) -shared $$j -o $$g; \
	done

app:
	$(GNU_LINUX_COMPILER) -c src/main.cpp $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) bin/*.o -o $(exec) $(flags)

projects:
	$(GNU_LINUX_COMPILER) -c src/main.cpp $(flags) -DPROJECT_MENU
	mv *.o bin
	$(GNU_LINUX_COMPILER) bin/*.o -o $(exec_proj) $(flags)

projects_win:
	$(MINGW_COMPILER) -static -g -std=c++20 $(sources) -o $(exec_win_proj) $(win_flags) -DPROJECT_MENU

bundle:
	mv src/*.o bin
linux:
	make eng
	make app

linux_game:
	g++ -c $(api) $(flags)
	mv *.o bin

	g++ -c src/main.cpp -DGAME_BUILD $(flags)
	mv *.o bin

	g++ bin/*.o -o $(exec_game) -DGAME_BUILD $(flags)
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
	$(MINGW_COMPILER) -static -g -std=c++20 $(sources) $(win_flags) -o $(win_exec)

win_game:
	$(MINGW_COMPILER) -static -g -std=c++20 $(sources) -DGAME_BUILD $(win_flags) -o $(exec_win_game)

clean:
	-rm src/*.o
	-rm *.o

# mono:
# 	g++ -c $(flags) src/ScriptEngine/ScriptEngine.cpp -o src/ScriptEngine/ScriptEngine.o
