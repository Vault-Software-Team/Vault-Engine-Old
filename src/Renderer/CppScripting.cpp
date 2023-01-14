#include "CppScripting.hpp"

namespace HyperAPI::CppScripting {
    std::vector<SharedObject> cpp_scripts;

    void LoadScripts() {
#ifndef _WIN32
        auto iter = fs::recursive_directory_iterator("assets");
        for (auto &dirEntry : iter) {
            if (G_END_WITH(dirEntry.path().string(), ".cpp")) {
                std::string file = dirEntry.path();
                file = std::regex_replace(file, std::regex("\\.cpp"), ".so");

                if (!fs::exists(file))
                    continue;
                SharedObject sharedObj;

                sharedObj.handle = dlopen(file.c_str(), RTLD_LAZY);
                sharedObj.create =
                    (Script * (*)()) dlsym(sharedObj.handle, "create_object");
                sharedObj.name = dirEntry.path().filename();
                cpp_scripts.push_back(sharedObj);
            }
        }
#endif
    }

    void CompileLinuxScripts() {
#ifndef _WIN32
        auto iter = fs::recursive_directory_iterator("assets");
        for (auto &dirEntry : iter) {
            if (G_END_WITH(dirEntry.path().string(), ".cpp")) {
                std::string file = dirEntry.path();
                file = std::regex_replace(file, std::regex("\\.cpp"), ".so");

                std::string objFile = dirEntry.path();
                objFile =
                    std::regex_replace(objFile, std::regex("\\.cpp"), ".o");

                std::string headers =
                    "-I\"./src/vendor\" -I\"./src/vendor/bullet/bullet\" "
                    "-I\"./src/vendor/NoesisGUI\"";
                system((std::string(config.linuxCompiler) + " -c -fPIC " +
                        dirEntry.path().string() + " " + headers +
                        " -rdynamic -o " + objFile)
                           .c_str());

                system((std::string(config.linuxCompiler) + " -shared " +
                        objFile + " -o " + file)
                           .c_str());

                for (auto script : cpp_scripts) {
                    dlclose(script.handle);
                }
                cpp_scripts.clear();

                HYPER_LOG("C++ Scripts have been compiled (Linux Compiler)")
                LoadScripts();
            }
        }
#endif
    }

    void CompileWindowsScripts() {
#ifndef _WIN32
        auto iter = fs::recursive_directory_iterator("assets");
        for (auto &dirEntry : iter) {
            if (G_END_WITH(dirEntry.path().string(), ".cpp")) {
                std::string file = dirEntry.path();
                file = std::regex_replace(file, std::regex("\\.cpp"), ".dll");

                std::string objFile = dirEntry.path();
                objFile =
                    std::regex_replace(objFile, std::regex("\\.cpp"), ".o");

                std::string headers =
                    "-I\"./src/vendor\" -I\"./src/vendor/bullet/bullet\" "
                    "-I\"./src/vendor/NoesisGUI\"";
                headers += " ";
                headers +=
                    "-lstdc++fs -L\"./win_libs\" -lglfw3dll -lstdc++fs "
                    "-lluajit-5.1 -lbox2d -lassimp.dll -lfreetype "
                    "-lSDL2.dll -lSDL2_mixer.dll -ldiscord-rpc -ltinyxml2";
                headers += " -lBulletDynamics.dll -lBulletCollision.dll "
                           "-lLinearMath.dll";
                system((std::string(config.windowsCompiler) +
                        " -c -DBUILD_DLL " + dirEntry.path().string() +
                        " src/api.cpp " + headers + " -o " + objFile)
                           .c_str());

                system((std::string(config.windowsCompiler) + " " + headers +
                        " -shared -o " + file + " " + objFile +
                        " -Wl,--out-implib,libshared_lib.a")
                           .c_str());

                // for(auto script : cpp_scripts) {
                //     dlclose(script.handle);
                // }
                // cpp_scripts.clear();

                HYPER_LOG("C++ Scripts have been compiled (Windows Compiler)")
            }
        }
#endif
    }
} // namespace HyperAPI::CppScripting