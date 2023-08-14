#include "CppScripting.hpp"
#include "csharp.hpp"
#include "scene.hpp"
#include "../../Components/CppScriptManager.hpp"
#include "../../Components/GameObject.hpp"
#include "../../f_GameObject/f_GameObject.hpp"

namespace HyperAPI::CppScripting {
    DLL_API std::vector<SharedObject> cpp_scripts;
    DLL_API std::vector<filewatch::FileWatch<std::string> *> FileWatches;

    void CompileScripts_Linux(fs::path dirEntry) {
        std::string file = dirEntry.string();
        file = std::regex_replace(file, std::regex("\\.cpp"), ".so");

        std::string objFile = dirEntry.string();
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
                dirEntry.string() + " " + headers +
                " -rdynamic -o " + objFile)
                   .c_str());

        system((std::string(config.linuxCompiler) + " -shared " +
                objFile + " -o " + file)
                   .c_str());

#ifndef _WIN32
        for (auto script : cpp_scripts) {
            dlclose(script.handle);
        }
        cpp_scripts.clear();
        LoadScripts();
#endif

        HYPER_LOG("C++ Scripts have been compiled (Linux Compiler)")
        LoadScripts();
    }

    void CompileScripts_Windows(fs::path dirEntry) {
        std::string file = dirEntry.string();
        file = std::regex_replace(file, std::regex("\\.cpp"), ".dll");

        std::string objFile = dirEntry.string();
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
        HYPER_LOG(headers);

        system((std::string(config.windowsCompiler) +
                " -c -static -g -Og -DBUILD_DLL -std=c++20 -Wa,-mbig-obj " + dirEntry.string() + " " + headers + " -o " + objFile)
                   .c_str());

        system((std::string(config.windowsCompiler) + " -shared -o " + file + " " + objFile + " " + headers).c_str());

#ifdef _WIN32
        for (auto script : cpp_scripts) {
            FreeLibrary(script.handle);
        }
        cpp_scripts.clear();
        LoadScripts();
#endif

        HYPER_LOG("C++ Scripts have been compiled (Windows Compiler)")
    }

    void LoadScripts() {
#ifndef _WIN32
        auto iter = fs::recursive_directory_iterator("assets");
        for (auto &dirEntry : iter) {
            // if (G_END_WITH(dirEntry.path().string(), ".cpp") || G_END_WITH(dirEntry.path().string(), ".hpp")) {
            //     HYPER_LOG("registered file watch at " << dirEntry.path().string())
            //     filewatch::FileWatch<std::string> watch((std::string("./") + ), [&](const std::string &filename, const filewatch::Event change_type) {
            //         HYPER_LOG("file watch fired at " << dirEntry.path().string())
            //         CompileScripts_Linux(dirEntry);
            //         CompileScripts_Windows(dirEntry);
            //     });
            //     FileWatches.push_back(&watch);
            // }

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
                HYPER_LOG(file);

                if (!fs::exists(file))
                    continue;
                HYPER_LOG("it exists");
                SharedObject sharedObj;

                sharedObj.handle = LoadLibrary(file.c_str());
                sharedObj.create =
                    (Script * (*)()) GetProcAddress(sharedObj.handle, "create_object");
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
                std::cout << headers << std::endl;
                system((std::string(config.linuxCompiler) + " -c -fPIC " +
                        dirEntry.path().string() + " " + headers +
                        " -rdynamic -o " + objFile)
                           .c_str());

                system((std::string(config.linuxCompiler) + " -shared " +
                        objFile + " -o " + file)
                           .c_str());

#ifndef _WIN32
                for (auto script : cpp_scripts) {
                    if (script.handle)
                        dlclose(script.handle);
                }
                cpp_scripts.clear();
                LoadScripts();
#endif

                HYPER_LOG("C++ Scripts have been compiled (Linux Compiler)")
            }
        }
    }

    void CompileWindowsScripts() {
        auto iter = fs::recursive_directory_iterator("assets");
        std::thread *compilerThread = new std::thread([&] {
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
                    HYPER_LOG(headers);

                    system((std::string(config.windowsCompiler) +
                            " -c -static -g -Og -DBUILD_DLL -std=c++20 -Wa,-mbig-obj " + dirEntry.path().string() + " " + headers + " -o " + objFile)
                               .c_str());

                    system((std::string(config.windowsCompiler) + " -shared -o " + file + " " + objFile + " " + headers).c_str());

#ifdef _WIN32
                    for (auto script : cpp_scripts) {
                        FreeLibrary(script.handle);
                    }
                    LoadScripts();
#endif

                    HYPER_LOG("C++ Scripts have been compiled (Windows Compiler)")
                }
            }
        });
    }
} // namespace HyperAPI::CppScripting