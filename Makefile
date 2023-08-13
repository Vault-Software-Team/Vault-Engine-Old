# SET THESE VARIABLES TO MATCH YOUR ENVIRONMENT
MINGW_COMPILER = x86_64-w64-mingw32-g++
GNU_LINUX_COMPILER = g++

# SET THESE VARIABLES TO MATCH YOUR ENVIRONMENT

exec = bin/build.out
exec_game = bin/game.out
exec_win_game = windows/bin/game.exe
win_exec = windows/bin/win_build.exe

exec_proj = bin/build_proj.out
exec_win_proj = windows/bin/win_proj.exe

cwd=$(shell pwd)
sources = $(wildcard src/*.cpp)
sources += $(wildcard src/vendor/*/*.cpp)
sources += $(wildcard src/vendor/*/*/*.cpp)
sources += $(wildcard src/vendor/*/*/*/*.cpp)
sources += $(wildcard src/vendor/*/*/*.c)
sources += $(wildcard src/vendor/*/*/*/*.c)
sources += $(wildcard src/*/*.cpp)
sources += $(wildcard src/*/*/*.cpp)
sources += $(wildcard src/*/*/*/*.cpp)
sources += $(wildcard src/*/*/*.c)
sources += $(wildcard src/*/*/*/*.c)

objects = $(sources:.cpp=.o)
api += $(wildcard src/*/*.cpp)
api += $(wildcard src/*/*/*.cpp)
api += $(wildcard src/*/*/*/*.cpp)
api += $(wildcard src/*/*/*.c)
api += $(wildcard src/*/*/*/*.c)
api += $(wildcard src/InputEvents.cpp)
api += $(wildcard src/scene.cpp)
api += $(wildcard src/Scripting/Lua/ScriptEngine.cpp)
api += $(wildcard src/networking.cpp)
api += $(wildcard src/nativeScripts.cpp)

components = $(wildcard src/Components/*.cpp)
renderer = $(wildcard src/Renderer/*.cpp)
audio = $(wildcard src/Audio/*.cpp)
other_stuff = $(wildcard src/Application/*.cpp)
other_stuff = $(wildcard src/lib/*.cpp)
other_stuff += $(wildcard src/Bloom/*.cpp)
other_stuff += $(wildcard src/f_GameObject/*.cpp)
other_stuff += $(wildcard src/Experimental/*.cpp)
other_stuff += $(wildcard src/scene.cpp)
other_stuff += $(wildcard src/InputEvents.cpp)

debugging = $(wildcard src/Debugging/*.cpp)
scripting = $(wildcard src/Scripting/*/*.cpp)

scripts = $(wildcard src/scripts/*.cpp)
batch = $(wildcard src/Batch/*.cpp)
rusty_cpp = $(wildcard src/Rusty/*.cpp)

api_obj = $(api:.cpp=.o)

# relatively link shared libraries
# -L$(cwd)/lib
MONO_LIB=-I"$(cwd)/mono/include/mono-2.0" -D_REENTRANT  -L"$(cwd)/mono/lib" -lmono-2.0
bullet_physics_linker_flags = -lBulletDynamics -lBulletCollision -lLinearMath
bullet_physics_linker_flags_windows = -lBulletDynamics.dll -lBulletCollision.dll -lLinearMath.dll
rusty = -lrusty_vault
flags = -w -fno-stack-protector -std=c++20 -lstdc++fs -g -L"./lib" -lluajit-5.1 -I"./src" -I"./src/vendor" -I"./src/vendor/bullet/bullet" -I"./src/vendor/NoesisGUI" -I"./src/lib" -lmono-2.0 -lbacktrace -lfreetype -lGL -lbox2d -lGLU -lglfw -lm -lSDL2_mixer -lassimp -ltinyxml2 -lXrandr -lXi -lbox2d -lX11 -lXxf86vm -lpthread -ldl -lsndfile -lopenal -lXinerama -lzlib -lXcursor -lGLEW -ldiscord-rpc $(bullet_physics_linker_flags) -rdynamic
win_flags = -lstdc++fs -L"./win_libs" -I"./src/lib" -I"./src" -I"./src/vendor/NoesisGUI" -I"./src/vendor" -I"./src/vendor/bullet/bullet" -lsndfile.dll -lopenal.dll -lmono-2.0.dll -lglfw3dll -lstdc++fs -lluajit-5.1 -lbox2d -lassimp.dll -lfreetype.dll -lSDL2.dll -lSDL2_mixer.dll -ltinyxml2 -ldiscord-rpc.dll $(bullet_physics_linker_flags_windows)

all:
	$(GNU_LINUX_COMPILER) $(sources) src/api.cpp -o $(exec) $(flags)

eng:
# set the .o files with objects variable
	$(GNU_LINUX_COMPILER) -c $(api) src/scripts.cpp  $(flags)
	mv *.o bin

deez:
	$(GNU_LINUX_COMPILER) -c src/Renderer/Mesh.cpp $(flags)
	mv *.o bin
	make app

components:
	$(GNU_LINUX_COMPILER) -c $(components) $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) -c src/main.cpp $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) bin/*.o -o $(exec) $(flags)

audio:
	$(GNU_LINUX_COMPILER) -c $(audio) $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) -c src/main.cpp $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) bin/*.o -o $(exec) $(flags)

other:
	$(GNU_LINUX_COMPILER) -c $(other_stuff) $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) -c src/main.cpp $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) bin/*.o -o $(exec) $(flags)

rusty_linux:
	$(GNU_LINUX_COMPILER) -c -fPIC $(rusty_cpp) $(flags)
	mv *.o bin
	make app

renderer:
	$(GNU_LINUX_COMPILER) -c $(renderer) $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) -c src/main.cpp $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) bin/*.o -o $(exec) $(flags)

debugging:
	$(GNU_LINUX_COMPILER) -c $(debugging) $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) -c src/main.cpp $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) bin/*.o -o $(exec) $(flags)

one_file:
	$(GNU_LINUX_COMPILER) -c src/main.cpp src/Application/Application.cpp src/Renderer/Base.cpp $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) bin/*.o -o $(exec) $(flags)

scripting:
	$(GNU_LINUX_COMPILER) -c $(scripting) $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) -c src/main.cpp $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) bin/*.o -o $(exec) $(flags)

cxxln:
	$(GNU_LINUX_COMPILER) -c src/Scripting/CXX/CppScripting.cpp $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) -c src/main.cpp $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) bin/*.o -o $(exec) $(flags)

batch:
	$(GNU_LINUX_COMPILER) -c $(batch) $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) -c src/main.cpp $(flags)
	mv *.o bin

	$(GNU_LINUX_COMPILER) bin/*.o -o $(exec) $(flags)

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
		$(GNU_LINUX_COMPILER) -c -fPIC $$i -I"./src/vendor" -I"./src/vendor/bullet/bullet" -I"./src/vendor/NoesisGUI" -rdynamic -o $$j; \
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
	$(MINGW_COMPILER) -c -static -g -Og -std=c++20 -Wa,-mbig-obj src/main.cpp $(win_flags) -DPROJECT_MENU
	mv *.o bin_win
	$(MINGW_COMPILER) -static -g -Og -std=c++20 -Wa,-mbig-obj bin_win/*.o -o $(exec_win_proj) $(win_flags) -DPROJECT_MENU

bundle:
	mv src/*.o bin
linux:
	make eng
	make app

linux_game:
	g++ -c src/main.cpp src/Scripting/C#/ImGuiFunctions.cpp -DGAME_BUILD $(flags)
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
	$(MINGW_COMPILER) -c -w -static -g -Og -std=c++20 -Wa,-mbig-obj $(sources) $(win_flags)
	mv *.o bin_win
	$(MINGW_COMPILER) -static -g -Og -std=c++20 -Wa,-mbig-obj bin_win/*.o -o $(win_exec) -Wl,--export-all-symbols,--out-implib,libhost.a $(win_flags)
	mv libhost.a cxx/windows/libvault_api.a

win_rusty:
	$(MINGW_COMPILER) -c -g -Og -static-libstdc++ -std=c++20 -Wa,-mbig-obj $(rusty_cpp) -static $(win_flags)
	mv *.o bin_win
	ar rcs ./win_libs/libcppvault.a bin_win/*.o

win_cxx:
	$(MINGW_COMPILER) -static -g -Og -std=c++20 -Wa,-mbig-obj bin_win/*.o -o $(win_exec) -Wl,--export-all-symbols,--out-implib,libhost.a $(win_flags)

win_assemble:
	$(MINGW_COMPILER) -c -static -g -Og -std=c++20 -Wa,-mbig-obj src/scene.cpp $(win_flags) -lrusty_vault
	# mv *.o bin_win
	$(MINGW_COMPILER) -c -static -g -Og -std=c++20 -Wa,-mbig-obj src/main.cpp $(win_flags) -lrusty_vault
	mv *.o bin_win

	$(MINGW_COMPILER) -static -g -Og -std=c++20 -Wa,-mbig-obj bin_win/*.o -o $(win_exec) -Wl,--export-all-symbols,--out-implib,libhost.a $(win_flags) -lrusty_vault
	mv libhost.a cxx/windows/libvault_api.a

win_components:
	$(MINGW_COMPILER) -c -static -g -Og -std=c++20 -Wa,-mbig-obj $(components) $(win_flags)
	mv *.o bin_win

	$(MINGW_COMPILER) -c -static -g -Og -std=c++20 -Wa,-mbig-obj src/main.cpp $(win_flags)
	mv *.o bin_win

	$(MINGW_COMPILER) -static -g -Og -std=c++20 -Wa,-mbig-obj bin_win/*.o -o $(win_exec) $(win_flags)

win_other:
	$(MINGW_COMPILER) -c -static -g -Og -std=c++20 -Wa,-mbig-obj $(other_stuff) $(win_flags)
	mv *.o bin_win

	$(MINGW_COMPILER) -c -static -g -Og -std=c++20 -Wa,-mbig-obj src/main.cpp $(win_flags)
	mv *.o bin_win

	$(MINGW_COMPILER) -static -g -Og -std=c++20 -Wa,-mbig-obj bin_win/*.o -o $(win_exec) $(win_flags)

win_renderer:
	$(MINGW_COMPILER) -c -static -g -Og -std=c++20 -Wa,-mbig-obj $(renderer) $(win_flags)
	mv *.o bin_win

	$(MINGW_COMPILER) -c -static -g -Og -std=c++20 -Wa,-mbig-obj src/main.cpp $(win_flags)
	mv *.o bin_win

	$(MINGW_COMPILER) -static -g -Og -std=c++20 -Wa,-mbig-obj bin_win/*.o -o $(win_exec) $(win_flags)

win_debugging:
	$(MINGW_COMPILER) -c -static -g -Og -std=c++20 -Wa,-mbig-obj $(debugging) $(win_flags)
	mv *.o bin_win

	$(MINGW_COMPILER) -c -static -g -Og -std=c++20 -Wa,-mbig-obj src/main.cpp $(win_flags)
	mv *.o bin_win

	$(MINGW_COMPILER) -static -g -Og -std=c++20 -Wa,-mbig-obj bin_win/*.o -o $(win_exec) $(win_flags)

win_scripting:
	$(MINGW_COMPILER) -c -static -g -Og -std=c++20 -Wa,-mbig-obj $(scripting) $(win_flags)
	mv *.o bin_win

	$(MINGW_COMPILER) -c -static -g -Og -std=c++20 -Wa,-mbig-obj src/main.cpp $(win_flags)
	mv *.o bin_win

	$(MINGW_COMPILER) -static -g -Og -std=c++20 -Wa,-mbig-obj bin_win/*.o -o $(win_exec) $(win_flags)

win_game:
	$(MINGW_COMPILER) -c -static -g -Og -std=c++20 -Wa,-mbig-obj src/main.cpp -DGAME_BUILD $(win_flags)
	mv *.o bin_win
	$(MINGW_COMPILER) -static -g -Og -std=c++20 -Wa,-mbig-obj bin_win/*.o -o $(exec_win_game) $(win_flags)

win_audio:
	$(MINGW_COMPILER) -c -static -g -Og -std=c++20 -Wa,-mbig-obj $(audio) $(win_flags)
	mv *.o bin_win

	# $(MINGW_COMPILER) -c -static -g -Og -std=c++20 -Wa,-mbig-obj src/main.cpp $(win_flags)
	# mv *.o bin_win
	# $(MINGW_COMPILER) -static -g -Og -std=c++20 -Wa,-mbig-obj bin_win/*.o -o $(win_exec) $(win_flags)

clean:
	-rm src/*.o
	-rm *.o

# mono:
# 	g++ -c $(flags) src/ScriptEngine/Scripting/Lua/ScriptEngine.cpp -o src/ScriptEngine/ScriptEngine.o
