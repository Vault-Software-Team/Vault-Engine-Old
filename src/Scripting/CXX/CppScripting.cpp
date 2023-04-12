#include "CppScripting.hpp"
#include "csharp.hpp"
#include "scene.hpp"
#include "../../Components/CppScriptManager.hpp"
#include "../../Components/GameObject.hpp"
#include "../../f_GameObject/f_GameObject.hpp"

namespace HyperAPI::CppScripting {
    std::vector<SharedObject> cpp_scripts;

    void LoadScripts() {
#ifndef _WIN32
        auto iter = fs::recursive_directory_iterator("assets");
        for (auto &dirEntry : iter) {
            if (G_END_WITH(dirEntry.path().string(), ".cpp")) {
                std::string file = dirEntry.path().string();
                file = std::regex_replace(file, std::regex("\\.cpp"), ".so");

                if (!fs::exists(file))
                    continue;
                SharedObject sharedObj;

                sharedObj.handle = dlopen(file.c_str(), RTLD_LAZY);
                sharedObj.create =
                    (Script * (*)()) dlsym(sharedObj.handle, "create_object");
                sharedObj.name = dirEntry.path().filename().string();
                cpp_scripts.push_back(sharedObj);

                auto view = Scene::m_Registry.view<Experimental::CppScriptManager>();
                for (auto e : view) {
                    auto *go = f_GameObject::FindGameObjectByEntt(e);
                    auto &comp = go->GetComponent<Experimental::CppScriptManager>();

                    for (auto *script : comp.addedScripts) {
                        for (auto scrpt : cpp_scripts) {
                            if (scrpt.name == script->name) {
                                if (script)
                                    delete script;
                                script = scrpt.create();
                                script->name = scrpt.name;
                            }
                        }
                    }
                }
            }
        }
#else
        auto iter = fs::recursive_directory_iterator("assets");
        for (auto &dirEntry : iter) {
            if (G_END_WITH(dirEntry.path().string(), ".cpp")) {
                std::string file = dirEntry.path().string();
                file = std::regex_replace(file, std::regex("\\.cpp"), ".dll");

                if (!fs::exists(file))
                    continue;
                SharedObject sharedObj;

                sharedObj.handle = LoadLibrary(file.c_str());
                sharedObj.create =
                    (Script * (*)()) GetProcAddress(sharedObj.handle, "create_object");
                sharedObj.name = dirEntry.path().filename().string();
                cpp_scripts.push_back(sharedObj);
            }
        }
#endif
    }

    void CompileLinuxScripts() {
        auto iter = fs::recursive_directory_iterator("assets");
        for (auto &dirEntry : iter) {
            if (G_END_WITH(dirEntry.path().string(), ".cpp")) {
                std::string file = dirEntry.path().string();
                file = std::regex_replace(file, std::regex("\\.cpp"), ".so");

                std::string objFile = dirEntry.path().string();
                objFile =
                    std::regex_replace(objFile, std::regex("\\.cpp"), ".o");

#ifdef _WIN32
                std::string cxx = CsharpVariables::oldCwd + "\\cxx";
                std::string headers =
                    "-I\"" + cxx + "\\headers\\vendor\" -I\"" + cxx + "\\headers\\vendor\\bullet\\bullet\" "
                                                                      "-I\"" +
                    cxx + "\\headers\\vendor\\NoesisGUI\" -I\"" + cxx + "\\headers\\lib\"";
#else
                std::string cxx = CsharpVariables::oldCwd + "/cxx";
                std::string headers =
                    "-I\"" + cxx + "/headers/vendor\" -I\"" + cxx + "/headers/vendor/bullet/bullet\" "
                                                                    "-I\"" +
                    cxx + "/headers/vendor/NoesisGUI\" -I\"" + cxx + "/headers/lib\"";
#endif
                std::cout << "what\n";
                std::cout << headers << std::endl;
                system((std::string(config.linuxCompiler) + " -c -fPIC " +
                        dirEntry.path().string() + " " + headers +
                        " -rdynamic -o " + objFile)
                           .c_str());

                system((std::string(config.linuxCompiler) + " -shared " +
                        objFile + " -o " + file)
                           .c_str());

                for (auto script : cpp_scripts) {
#ifdef _WIN32
                    FreeLibrary(script.handle);
#else
                    dlclose(script.handle);
#endif
                }
                cpp_scripts.clear();

                HYPER_LOG("C++ Scripts have been compiled (Linux Compiler)")
                LoadScripts();
            }
        }
    }

    void CompileWindowsScripts() {
        auto iter = fs::recursive_directory_iterator("assets");
        for (auto &dirEntry : iter) {
            if (G_END_WITH(dirEntry.path().string(), ".cpp")) {
                std::string file = dirEntry.path().string();
                file = std::regex_replace(file, std::regex("\\.cpp"), ".dll");

                std::string objFile = dirEntry.path().string();
                objFile =
                    std::regex_replace(objFile, std::regex("\\.cpp"), ".o");

#ifdef _WIN32
                std::string cxx = CsharpVariables::oldCwd + "\\cxx";
                std::string headers = "-lstdc++fs -L\"" + cxx + "\\windows\" "
                                                                "-I\"" +
                                      cxx + "\\headers\\lib\" -I\"" + cxx + "\\headers\\vendor\\NoesisGUI\" -I\"" + cxx + "\\headers\\vendor\" -I\"" + cxx + "\\headers\\vendor\\bullet\\bullet\" -lvault_api -lsndfile.dll -lopenal.dll -lmono-2.0.dll -lglfw3dll -lstdc++fs -lluajit-5.1 -lbox2d -lassimp.dll -lfreetype.dll -lSDL2.dll -lSDL2_mixer.dll -ldiscord-rpc  -ltinyxml2 -lBulletDynamics.dll -lBulletCollision.dll -lLinearMath.dll";
#else
                std::string cxx = CsharpVariables::oldCwd + "/cxx";
                std::string headers = "-lstdc++fs -L\"" + cxx + "/windows\" "
                                                                "-I\"" +
                                      cxx + "/headers/lib\" -I\"" + cxx + "/headers/vendor/NoesisGUI\" -I\"" + cxx + "/headers/vendor\" -I\"" + cxx + "/headers/vendor/bullet/bullet\" -lvault_api -lsndfile.dll -lopenal.dll -lmono-2.0.dll -lglfw3dll -lstdc++fs -lluajit-5.1 -lbox2d -lassimp.dll -lfreetype.dll -lSDL2.dll -lSDL2_mixer.dll -ldiscord-rpc  -ltinyxml2 -lBulletDynamics.dll -lBulletCollision.dll -lLinearMath.dll";
#endif

                system((std::string(config.windowsCompiler) +
                        " -c -static -g -Og -DBUILD_DLL -std=c++20 -Wa,-mbig-obj " + dirEntry.path().string() + " " + headers + " -o " + objFile)
                           .c_str());

                system((std::string(config.windowsCompiler) + " -shared -o " + file + " " + objFile + " " + headers).c_str());

                for (auto script : cpp_scripts) {
#ifdef _WIN32
                    FreeLibrary(script.handle);
#else
                    dlclose(script.handle);
#endif
                }

                HYPER_LOG("C++ Scripts have been compiled (Windows Compiler)")
                LoadScripts();
            }
        }
    }
} // namespace HyperAPI::CppScripting